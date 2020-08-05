/***************************************************************************//**
* \file cyhal_hw_types.h
*
* \brief
* Provides a struct definitions for configuration resources in the PDL.
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

/**
* \addtogroup group_hal_psoc6 PSoC 6 Implementation Specific
* \{
* This section provides details about the PSoC 6 implementation of the Cypress HAL.
* All information within this section is platform specific and is provided for reference.
* Portable application code should depend only on the APIs and types which are documented
* in the @ref group_hal section.
*
* \section group_hal_psoc6_mapping HAL Resource Hardware Mapping
* The following table shows a mapping of each HAL driver to the lower level firmware driver
* and the corresponding hardware resource. This is intended to help understand how the HAL
* is implemented for PSoC 6 and what features the underlying hardware supports.
*
* | HAL Resource     | PDL Driver(s)       | PSoC 6 Hardware                  |
* | ---------------- | ------------------- | -------------------------------- |
* | ADC              | cy_adc              | SAR ADC                          |
* | Clock            | cy_sysclk           | All clocks (system & peripheral) |
* | CRC              | cy_crypto_core_crc  | Crypto                           |
* | DAC              | cy_ctdac            | DAC                              |
* | DMA              | cy_dma, cy_dmac     | DMA Controller                   |
* | EZI2C            | cy_scb_ezi2c        | SCB                              |
* | Flash            | cy_flash            | Flash                            |
* | GPIO             | cy_gpio             | GPIO                             |
* | Hardware Manager | NA                  | NA                               |
* | I2C              | cy_scb_i2c          | SCB                              |
* | I2S              | cy_i2s              | I2S                              |
* | LPTimer          | cy_mcwdt            | MCWDT                            |
* | PDM/PCM          | cy_pdm_pcm          | PDM-PCM                          |
* | PWM              | cy_pwm              | TCPWM                            |
* | QSPI             | cy_smif             | QSPI (SMIF)                      |
* | RTC              | cy_rtc              | RTC                              |
* | SDHC             | cy_sd_host          | SD Host                          |
* | SDIO             | cy_sd_host, or NA   | SD Host, or UDB                  |
* | SPI              | cy_scb_spi          | SCB                              |
* | SysPM            | cy_syspm            | System Power Resources           |
* | System           | cy_syslib           | System Resources                 |
* | Timer            | cy_tcpwm_counter    | TCPWM                            |
* | TRNG             | cy_crypto_core_trng | Crypto                           |
* | UART             | cy_scb_uart         | SCB                              |
* | USB Device       | cy_usbfs_dev_drv    | USB-FS                           |
* | WDT              | cy_wdt              | WDT                              |
*
* \section group_hal_psoc6_errors Device Specific Errors
* Error codes generated by the low level level PDL driver all use module IDs starting
* with \ref CY_RSLT_MODULE_DRIVERS_PDL_BASE. The exact errors are documented for each
* driver in the
* <a href="https://cypresssemiconductorco.github.io/psoc6pdl/pdl_api_reference_manual/html/index.html">
* psoc6pdl documentation</a>.
*/

/**
* \addtogroup group_hal_psoc6_hw_types PSoC 6 Specific Hardware Types
* \{
* Aliases for types which are part of the public HAL interface but whose representations
* need to vary per HAL implementation
*/

#pragma once

#include "cy_pdl.h"
#include "cyhal_general_types.h"
#include "cyhal_hw_resources.h"
#include "cyhal_pin_package.h"
#include "cyhal_triggers.h"
#include <stdbool.h>

#if defined(CYHAL_UDB_SDIO)
    #include "SDIO_HOST.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifndef CYHAL_ISR_PRIORITY_DEFAULT
/** Priority that is applied by default to all drivers when initalized. Priorities can be
 * overridden on each driver as part of enabling events.
 */
#define CYHAL_ISR_PRIORITY_DEFAULT  (7)
#endif

/**
* \cond INTERNAL
*/

#define CYHAL_CRC_IMPL_HEADER       "cyhal_crc_impl.h"      //!< Implementation specific header for CRC
#define CYHAL_DMA_IMPL_HEADER       "cyhal_dma_impl.h"      //!< Implementation specific header for DMA
#define CYHAL_CLOCK_IMPL_HEADER     "cyhal_clock_impl.h"    //!< Implementation specific header for Clocks
#define CYHAL_GPIO_IMPL_HEADER      "cyhal_gpio_impl.h"     //!< Implementation specific header for GPIO
#define CYHAL_PDMPCM_IMPL_HEADER    "cyhal_pdmpcm_impl.h"   //!< Implementation specific header for PDMPCM
#define CYHAL_PWM_IMPL_HEADER       "cyhal_pwm_impl.h"      //!< Implementation specific header for PWM
#define CYHAL_SYSTEM_IMPL_HEADER    "cyhal_system_impl.h"   //!< Implementation specific header for System
#define CYHAL_SYSPM_IMPL_HEADER     "cyhal_syspm_impl.h"    //!< Implementation specific header for System Power Management
#define CYHAL_TIMER_IMPL_HEADER     "cyhal_timer_impl.h"    //!< Implementation specific header for Timer
#define CYHAL_TRNG_IMPL_HEADER      "cyhal_trng_impl.h"     //!< Implementation specific header for TRNG

/** \endcond */

/**
*/
typedef uint32_t cyhal_source_t; //!< Routable signal source

/** Callbacks for Sleep and Deepsleep APIs */
#define cyhal_system_callback_t cy_stc_syspm_callback_t

/** @brief Event callback data object */
typedef struct {
    cy_israddress callback;
    void*         callback_arg;
} cyhal_event_callback_data_t;

/**
 * @brief Shared data between timer/counter and PWM
 *
 * Application code should not rely on the specific content of this struct.
 * They are considered an implementation detail which is subject to change
 * between platforms and/or HAL releases.
 */
typedef struct cyhal_tcpwm_common {
#ifdef CY_IP_MXTCPWM
    TCPWM_Type*                  base;
    cyhal_resource_inst_t        resource;
    cyhal_gpio_t                 pin;
    cyhal_clock_t                clock;
    bool                         dedicated_clock;
    uint32_t                     clock_hz;
    cyhal_event_callback_data_t  callback_data;
    cyhal_gpio_t                 pin_compl;     /* PWM Only */
    uint32_t                     default_value; /* Timer only */
#else
    void *empty
#endif
} cyhal_tcpwm_common_t;

/**
  * @brief ADC object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXS40PASS_SAR
    SAR_Type*                   base;
    cyhal_resource_inst_t       resource;
    cyhal_clock_t               clock;
    bool                        dedicated_clock;
    // channel_used is a bit field. The maximum channel count
    // supported by the SAR IP is 16
    uint16_t                    channel_used;
#else
    void *empty;
#endif
} cyhal_adc_t;

/**
  * @brief ADC channel object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXS40PASS_SAR
    cyhal_adc_t*                adc;
    cyhal_gpio_t                pin;
    uint8_t                     channel_idx;
#else
    void *empty;
#endif
} cyhal_adc_channel_t;

/**
  * @brief CRC object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#if defined(CY_IP_MXCRYPTO_INSTANCES) || defined(CPUSS_CRYPTO_PRESENT)
    CRYPTO_Type*                base;
    cyhal_resource_inst_t       resource;
    uint32_t                    crc_width;
#endif
} cyhal_crc_t;

/**
  * @brief DAC object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXS40PASS_CTDAC
    CTDAC_Type*                 base;
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin;
#else
    void *empty;
#endif
} cyhal_dac_t;

/**
  * @brief DMA object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#if defined(CY_IP_M4CPUSS_DMAC) || defined(CY_IP_M4CPUSS_DMA)
    cyhal_resource_inst_t          resource;
    union
    {
#ifdef CY_IP_M4CPUSS_DMA
        cy_stc_dma_channel_config_t    dw;
#endif
#ifdef CY_IP_M4CPUSS_DMAC
        cy_stc_dmac_channel_config_t   dmac;
#endif
    } channel_config;
    union
    {
#ifdef CY_IP_M4CPUSS_DMA
        cy_stc_dma_descriptor_config_t dw;
#endif
#ifdef CY_IP_M4CPUSS_DMAC
        cy_stc_dmac_descriptor_config_t dmac;
#endif
    } descriptor_config;
    union
    {
#ifdef CY_IP_M4CPUSS_DMA
        cy_stc_dma_descriptor_t        dw;
#endif
#ifdef CY_IP_M4CPUSS_DMAC
        cy_stc_dmac_descriptor_t        dmac;
#endif
    } descriptor;
    uint32_t                       irq_cause;
    cyhal_event_callback_data_t    callback_data;
#else
    void *empty;
#endif
} cyhal_dma_t;

/**
  * @brief Flash object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
    void *empty;
} cyhal_flash_t;

/**
  * @brief I2C object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXSCB
    CySCB_Type*                               base;
    cyhal_resource_inst_t                     resource;
    cyhal_gpio_t                              pin_sda;
    cyhal_gpio_t                              pin_scl;
    cyhal_clock_t                             clock;
    bool                                      is_shared_clock;
    cy_stc_scb_i2c_context_t                  context;
    cy_stc_scb_i2c_master_xfer_config_t       rx_config;
    cy_stc_scb_i2c_master_xfer_config_t       tx_config;
    bool                                      is_slave;
    uint32_t                                  address;
    uint32_t                                  irq_cause;
    uint16_t                                  pending;
    uint16_t                                  events;
    cyhal_event_callback_data_t               callback_data;
#else
    void *empty;
#endif
} cyhal_i2c_t;

/**
  * @brief EZI2C object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXSCB
    CySCB_Type*                         base;
    cyhal_resource_inst_t               resource;
    cyhal_gpio_t                        pin_sda;
    cyhal_gpio_t                        pin_scl;
    cyhal_clock_t                       clock;
    bool                                is_shared_clock;
    cy_stc_scb_ezi2c_context_t          context;
    uint32_t                            irq_cause;
    cyhal_event_callback_data_t         callback_data;
#else
    void *empty;
#endif
} cyhal_ezi2c_t;

/**
  * @brief I2S object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXAUDIOSS
    I2S_Type                        *base;
    cyhal_resource_inst_t           resource;
    cyhal_gpio_t                    pin_tx_sck;
    cyhal_gpio_t                    pin_tx_ws;
    cyhal_gpio_t                    pin_tx_sdo;
    cyhal_gpio_t                    pin_rx_sck;
    cyhal_gpio_t                    pin_rx_ws;
    cyhal_gpio_t                    pin_rx_sdi;
    cyhal_gpio_t                    pin_mclk;
    bool                            is_tx_slave;
    bool                            is_rx_slave;
    uint32_t                        mclk_hz;
    uint8_t                         channel_length;
    uint8_t                         word_length;
    uint32_t                        sample_rate_hz;
    cyhal_clock_t                   clock;
    bool                            is_clock_owned;
    uint16_t                        user_enabled_events;
    cyhal_event_callback_data_t     callback_data;
    cyhal_async_mode_t              async_mode;
    uint8_t                         async_dma_priority;
    cyhal_dma_t                     tx_dma;
    cyhal_dma_t                     rx_dma;
    // Note: When the async DMA mode is in use, these variables will always reflect the state
    // that the transfer will be in after the in-progress DMA transfer, if any, is complete
    const void                      *async_tx_buff;
    size_t                          async_tx_length;
    void                            *async_rx_buff;
    size_t                          async_rx_length;
    volatile bool                   pm_transition_ready;
    cyhal_syspm_callback_data_t     pm_callback;
#else
    void *empty;
#endif
} cyhal_i2s_t;

/**
  * @brief LPTIMER object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXS40SRSS_MCWDT_INSTANCES
    MCWDT_STRUCT_Type                *base;
    cyhal_resource_inst_t            resource;
    cyhal_event_callback_data_t      callback_data;
#else
    void *empty;
#endif
} cyhal_lptimer_t;

/**
  * @brief PDM-PCM object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXAUDIOSS_INSTANCES
    PDM_Type                                 *base;
    cyhal_resource_inst_t                    resource;
    cyhal_gpio_t                             pin_data;
    cyhal_gpio_t                             pin_clk;
    /** User requested irq, see cyhal_pdm_pcm_event_t */
    uint32_t                                 irq_cause;
    cyhal_event_callback_data_t              callback_data;
    uint8_t                                  word_size;
    cyhal_dma_t                              dma;
    volatile bool                            stabilized;
    volatile bool                            pm_transition_ready;
    cyhal_syspm_callback_data_t              pm_callback;
    void                                     *async_buffer;
    size_t                                   async_read_remaining;
#else
    void *empty;
#endif
} cyhal_pdm_pcm_t;

/**
  * @brief PWM object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef cyhal_tcpwm_common_t cyhal_pwm_t;

/**
  * @brief SMIF object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXSMIF
    SMIF_Type*                       base;
    cyhal_resource_inst_t            resource;
    cyhal_gpio_t                     pin_ios[8];
    cyhal_gpio_t                     pin_sclk;
    cyhal_gpio_t                     pin_ssel;
    uint32_t                         frequency;
    uint8_t                          mode;
    cy_stc_smif_context_t            context;
    cy_en_smif_slave_select_t        slave_select;
    cy_en_smif_data_select_t         data_select;
    uint32_t                         irq_cause;
    cyhal_event_callback_data_t      callback_data;
    cyhal_syspm_callback_data_t      pm_callback;
    bool                             pm_transition_pending;
#else
    void *empty;
#endif /* ifdef CY_IP_MXSMIF */
} cyhal_qspi_t;

/**
  * @brief RNG object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#if defined(CY_IP_MXCRYPTO_INSTANCES) || defined(CPUSS_CRYPTO_PRESENT)
    CRYPTO_Type*                base;
    cyhal_resource_inst_t       resource;
#else
    void *empty;
#endif
} cyhal_trng_t;

/**
  * @brief RTC object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXS40SRSS_RTC
    cy_stc_rtc_dst_t dst;
#else
    void *empty;
#endif
} cyhal_rtc_t;

/**
  * @brief SDHC object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXSDHC
    SDHC_Type*                       base;
    cyhal_resource_inst_t            resource;
    bool                             emmc;
    cy_en_sd_host_dma_type_t         dmaType;
    bool                             enableLedControl;
    cy_stc_sd_host_context_t         context;
    cyhal_gpio_t                     pin_clk;
    cyhal_gpio_t                     pin_cmd;
    cyhal_gpio_t                     pin_data[8];
    cyhal_gpio_t                     pin_cardDetect;
    cyhal_gpio_t                     pin_ioVoltSel;
    cyhal_gpio_t                     pin_cardIfPwrEn;
    cyhal_gpio_t                     pin_cardMechWriteProt;
    cyhal_gpio_t                     pin_ledCtrl;
    cyhal_gpio_t                     pin_cardEmmcReset;
    uint32_t                         irq_cause;
    cyhal_event_callback_data_t      callback_data;
    bool                             pm_transition_pending;
    cyhal_syspm_callback_data_t      pm_callback_data;
#else
    void *empty;
#endif
} cyhal_sdhc_t;

/**
  * @brief SDIO object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#if defined(CY_IP_MXSDHC)
    SDHC_Type*                                base;
    bool                                      emmc;
    cy_en_sd_host_dma_type_t                  dmaType;
    cy_stc_sd_host_context_t                  context;
#elif defined(CYHAL_UDB_SDIO)
    cyhal_clock_t                             clock;
    cyhal_dma_t                               dma0Ch0;
    cyhal_dma_t                               dma0Ch1;
    cyhal_dma_t                               dma1Ch1;
    cyhal_dma_t                               dma1Ch3;
    stc_sdio_irq_cb_t*                        pfuCb;
#endif /* defined(CY_IP_MXSDHC) */

#if defined(CYHAL_UDB_SDIO) || defined(CY_IP_MXSDHC)
    cyhal_resource_inst_t                     resource;
    cyhal_gpio_t                              pin_clk;
    cyhal_gpio_t                              pin_cmd;
    cyhal_gpio_t                              pin_data0;
    cyhal_gpio_t                              pin_data1;
    cyhal_gpio_t                              pin_data2;
    cyhal_gpio_t                              pin_data3;

    uint32_t                                  frequencyhal_hz;
    uint16_t                                  block_size;
    uint32_t                                  irq_cause;

    uint32_t                                  events;
    cyhal_event_callback_data_t               callback_data;

    bool                                      pm_transition_pending;
    cyhal_syspm_callback_data_t               pm_callback_data;
#else
    void *empty;
#endif /* defined(CY_IP_MXSDHC) || defined(CY_IP_MXSDHC) */
} cyhal_sdio_t;

/**
  * @brief SPI object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXSCB
    CySCB_Type*                               base;
    cyhal_resource_inst_t                     resource;
    cyhal_gpio_t                              pin_miso;
    cyhal_gpio_t                              pin_mosi;
    cyhal_gpio_t                              pin_sclk;
    cyhal_gpio_t                              pin_ssel;
    cyhal_clock_t                             clock;
    cy_en_scb_spi_sclk_mode_t                 clk_mode;
    uint8_t                                   mode;
    uint8_t                                   data_bits;
    bool                                      is_slave;
    bool                                      alloc_clock;
    uint8_t                                   oversample_value;
    bool                                      msb_first;
    cy_stc_scb_spi_context_t                  context;
    uint32_t                                  irq_cause;
    uint16_t volatile                         pending;
    uint8_t                                   write_fill;
    void                                      *rx_buffer;
    uint32_t                                  rx_buffer_size;
    const void                                *tx_buffer;
    uint32_t                                  tx_buffer_size;
    bool                                      is_async;
    cyhal_event_callback_data_t               callback_data;
#else
    void *empty;
#endif
} cyhal_spi_t;

/**
  * @brief Timer object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef cyhal_tcpwm_common_t cyhal_timer_t;

/**
  * @brief UART object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXSCB
    CySCB_Type*                      base;
    cyhal_resource_inst_t            resource;
    cyhal_gpio_t                     pin_rx;
    cyhal_gpio_t                     pin_tx;
    cyhal_gpio_t                     pin_cts;
    cyhal_gpio_t                     pin_rts;
    bool                             is_user_clock;
    cyhal_clock_t                    clock;
    cy_stc_scb_uart_context_t        context;
    cy_stc_scb_uart_config_t         config;
    uint32_t                         irq_cause;
    en_hsiom_sel_t                   saved_tx_hsiom;
    en_hsiom_sel_t                   saved_rts_hsiom;
    cyhal_event_callback_data_t      callback_data;
#else
    void *empty;
#endif
} cyhal_uart_t;

/**
  * @brief USB Device object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
#ifdef CY_IP_MXUSBFS
    USBFS_Type*                     base;
    cy_stc_usbfs_dev_drv_context_t  context;
    cyhal_resource_inst_t           resource;
    cyhal_resource_inst_t           pll_resource;
    cyhal_clock_t                   clock;
    bool                            shared_clock;
    cyhal_gpio_t                    pin_dp;
    cyhal_gpio_t                    pin_dm;
    cyhal_syspm_callback_data_t     pm_callback;
    uint8_t *rd_data[CY_USBFS_DEV_DRV_NUM_EPS_MAX];
    uint32_t rd_size[CY_USBFS_DEV_DRV_NUM_EPS_MAX];
#else
    void *empty;
#endif
} cyhal_usb_dev_t;

/**
  * @brief WDT object
  *
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct {
    uint8_t placeholder;
} cyhal_wdt_t;

/* For backwards compatability. */
#include "cyhal_deprecated.h"

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** \} group_hal_psoc6_hw_types */
/** \} group_hal_psoc6 */
