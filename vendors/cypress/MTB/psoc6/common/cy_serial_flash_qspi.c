/***************************************************************************//**
* \file cy_serial_flash_qspi.c
*
* \brief
* Provides APIs for interacting with an external flash connected to the SPI or
* QSPI interface, uses SFDP to auto-discover memory properties if SFDP is
* enabled in the configuration.
*
********************************************************************************
* \copyright
* Copyright 2018-2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include <stdbool.h>
#include "cy_serial_flash_qspi.h"
#include "cyhal_qspi.h"
#include "cy_utils.h"

#ifdef CY_IP_MXSMIF

#if defined(__cplusplus)
extern "C" {
#endif


/** \cond INTERNAL */

/** Timeout to apply while polling the memory for its ready status after quad
 * enable command has been sent out. Quad enable is a non-volatile write.
*/
#define CY_SERIAL_FLASH_QUAD_ENABLE_TIMEOUT_US      (5000lu) /* in microseconds */

/* Number of memories supported by the driver */
#define SMIF_MEM_DEVICES                            (1u)

/* SMIF slot from which the memory configuration is picked up - fixed to 0 as the driver
 * supports only one device */
#define MEM_SLOT                                    (0u)

/** \endcond */

#define HAL_HIGH_ISR_PRIORITY                       (1u)

static cyhal_qspi_t qspi_obj;

static cy_stc_smif_mem_config_t * qspi_mem_config[SMIF_MEM_DEVICES];

static cy_stc_smif_block_config_t qspi_block_config =
{
    /* The number of SMIF memories defined. */
    .memCount = SMIF_MEM_DEVICES,
    /* The pointer to the array of memory config structures of size memCount. */
    .memConfig = (cy_stc_smif_mem_config_t**)qspi_mem_config,
    /* The version of the SMIF driver. */
    .majorVersion = CY_SMIF_DRV_VERSION_MAJOR,
    /* The version of the SMIF driver. */
    .minorVersion = CY_SMIF_DRV_VERSION_MINOR
};

/* The driver supports only one memory. When multiple memory configurations
 * are generated by the SMIF configurator tool, provide only the
 * configuration for memory that need to be supported by the driver.
 * Memory configuration can be changed by deinit followed by init with new
 * configuration */
cy_rslt_t cy_serial_flash_qspi_init(
    const cy_stc_smif_mem_config_t* mem_config,
    cyhal_gpio_t io0,
    cyhal_gpio_t io1,
    cyhal_gpio_t io2,
    cyhal_gpio_t io3,
    cyhal_gpio_t io4,
    cyhal_gpio_t io5,
    cyhal_gpio_t io6,
    cyhal_gpio_t io7,
    cyhal_gpio_t sclk,
    cyhal_gpio_t ssel,
    uint32_t hz)
{
    cy_en_smif_status_t smifStatus = CY_SMIF_SUCCESS;


    cy_rslt_t result = cyhal_qspi_init(&qspi_obj, io0, io1, io2, io3, io4, io5, io6, io7,
                                        sclk, ssel, hz, 0);

    qspi_mem_config[MEM_SLOT] = (cy_stc_smif_mem_config_t*) mem_config;

    if(CY_RSLT_SUCCESS == result)
    {
        /* Change SIMF interrupt priority */
        cyhal_qspi_enable_event(&qspi_obj, CYHAL_QSPI_EVENT_NONE, HAL_HIGH_ISR_PRIORITY, false);

        /* Perform SFDP detection and XIP register configuration depending on the
         * memory configuration.
         */
        smifStatus = Cy_SMIF_MemInit(qspi_obj.base, &qspi_block_config, &qspi_obj.context);
        if(CY_SMIF_SUCCESS == smifStatus)
        {
            /* Enable Quad mode (1-1-4 or 1-4-4 modes) to use all the four I/Os during
            * communication.
            */

            if(qspi_block_config.memConfig[MEM_SLOT]->deviceCfg->readCmd->dataWidth == CY_SMIF_WIDTH_QUAD
                    || qspi_block_config.memConfig[MEM_SLOT]->deviceCfg->programCmd->dataWidth == CY_SMIF_WIDTH_QUAD)
            {
                bool isQuadEnabled = false;
                smifStatus = Cy_SMIF_MemIsQuadEnabled(qspi_obj.base, qspi_block_config.memConfig[MEM_SLOT], &isQuadEnabled, &qspi_obj.context);
                if(CY_SMIF_SUCCESS == smifStatus)
                {
                    if(!isQuadEnabled)
                    {
                        smifStatus = Cy_SMIF_MemEnableQuadMode(qspi_obj.base, qspi_block_config.memConfig[MEM_SLOT], CY_SERIAL_FLASH_QUAD_ENABLE_TIMEOUT_US, &qspi_obj.context);
                    }
                }
            }
        }
    }

    if((CY_RSLT_SUCCESS != result) || (CY_SMIF_SUCCESS != smifStatus))
    {
        cy_serial_flash_qspi_deinit();

        if(CY_SMIF_SUCCESS != smifStatus)
        {
            result = (cy_rslt_t)smifStatus;
        }
    }

    return result;
}

void cy_serial_flash_qspi_deinit(void)
{
    if (qspi_obj.base != NULL)
    {
        Cy_SMIF_MemDeInit(qspi_obj.base);
    }

    cyhal_qspi_free(&qspi_obj);
}

size_t cy_serial_flash_qspi_get_size(void)
{
    return (size_t)qspi_block_config.memConfig[MEM_SLOT]->deviceCfg->memSize;
}

/* address is ignored for the memory with uniform sector size. Currently,
 * QSPI Configurator does not support memories with hybrid sectors.
 */
size_t cy_serial_flash_qspi_get_erase_size(uint32_t addr)
{
    CY_UNUSED_PARAMETER(addr);
    return (size_t)qspi_block_config.memConfig[MEM_SLOT]->deviceCfg->eraseSize;
}

cy_rslt_t cy_serial_flash_qspi_read(uint32_t addr, size_t length, uint8_t *buf)
{
    /* Cy_SMIF_MemRead() returns error if (addr + length) > total flash size. */
    return (cy_rslt_t)Cy_SMIF_MemRead(qspi_obj.base, qspi_block_config.memConfig[MEM_SLOT], addr, buf, length, &qspi_obj.context);
}

cy_rslt_t cy_serial_flash_qspi_write(uint32_t addr, size_t length, const uint8_t *buf)
{
    /* Cy_SMIF_MemWrite() returns error if (addr + length) > total flash size. */
    return (cy_rslt_t)Cy_SMIF_MemWrite(qspi_obj.base, qspi_block_config.memConfig[MEM_SLOT], addr, (uint8_t *)buf, length, &qspi_obj.context);
}

/* Does not support hybrid sectors, sector size must be uniform on the entire
 * chip. Use cy_serial_flash_qspi_get_erase_size(addr) to implement hybrid sector
 * support when QSPI Configurator and PDL supports memories with hybrid sectors.
 */
cy_rslt_t cy_serial_flash_qspi_erase(uint32_t addr, size_t length)
{
    cy_en_smif_status_t smifStatus;

    /* If the erase is for the entire chip, use chip erase command */
    if((addr == 0u) && (length == cy_serial_flash_qspi_get_size()))
    {
        smifStatus = Cy_SMIF_MemEraseChip(qspi_obj.base, qspi_block_config.memConfig[MEM_SLOT], &qspi_obj.context);
    }
    else
    {
        /* Cy_SMIF_MemEraseSector() returns error if (addr + length) > total flash size
         * or if addr is not aligned to erase sector size or if (addr + length)
         * is not aligned to erase sector size.
         */
        smifStatus = Cy_SMIF_MemEraseSector(qspi_obj.base, qspi_block_config.memConfig[MEM_SLOT], addr, length, &qspi_obj.context);
    }

    return (cy_rslt_t)smifStatus;
}

// This function enables or disables XIP on the MCU, does not send any command
// to the serial flash. XIP register configuration is already done as part of
// cy_serial_flash_qspi_init() if MMIO mode is enabled in the QSPI
// Configurator.
cy_rslt_t cy_serial_flash_qspi_enable_xip(bool enable)
{
    if(enable)
    {
        Cy_SMIF_SetMode(qspi_obj.base, CY_SMIF_MEMORY);
    }
    else
    {
        Cy_SMIF_SetMode(qspi_obj.base, CY_SMIF_NORMAL);
    }

    return CY_RSLT_SUCCESS;
}

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXSMIF */

