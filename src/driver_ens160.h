/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_ens160.h
 * @brief     driver ens160 header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2026-02-23
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2026/02/23  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef DRIVER_ENS160_H
#define DRIVER_ENS160_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ens160_driver ens160 driver function
 * @brief    ens160 driver modules
 * @{
 */

/**
 * @addtogroup ens160_basic_driver
 * @{
 */

/**
 * @brief ens160 interface enumeration definition
 */
typedef enum
{
    ENS160_INTERFACE_IIC = 0x00,        /**< iic interface */
    ENS160_INTERFACE_SPI = 0x01,        /**< spi interface */
} ens160_interface_t;

/**
 * @brief ens160 address enumeration definition
 */
typedef enum
{
    ENS160_ADDRESS_0 = (0x52 << 1),        /**< MISO connected to the GND */
    ENS160_ADDRESS_1 = (0x53 << 1),        /**< MISO connected to the VCC */
} ens160_address_t;

/**
 * @brief ens160 bool enumeration definition
 */
typedef enum
{
    ENS160_BOOL_FALSE = 0x00,        /**< false */
    ENS160_BOOL_TRUE  = 0x01,        /**< true */
} ens160_bool_t;

/**
 * @brief ens160 mode enumeration definition
 */
typedef enum
{
    ENS160_MODE_DEEP_SLEEP = 0x00,        /**< deep sleep mode */
    ENS160_MODE_IDLE       = 0x01,        /**< idle mode */
    ENS160_MODE_STANDARD   = 0x02,        /**< standard mode */
} ens160_mode_t;

/**
 * @brief ens160 pin polarity enumeration definition
 */
typedef enum
{
    ENS160_PIN_POLARITY_LOW  = 0x00,        /**< active low */
    ENS160_PIN_POLARITY_HIGH = 0x01,        /**< active high */
} ens160_pin_polarity_t;

/**
 * @brief ens160 pin drive enumeration definition
 */
typedef enum
{
    ENS160_PIN_DRIVE_OPEN_DRAIN = 0x00,        /**< open drain */
    ENS160_PIN_DRIVE_PUSH_PULL  = 0x01,        /**< push-pull */
} ens160_pin_drive_t;

/**
 * @brief ens160 status enumeration definition
 */
typedef enum
{
    ENS160_STATUS_RUNNING  = (1 << 7),        /**< running */
    ENS160_STATUS_ERROR    = (1 << 6),        /**< an error is detected */
    ENS160_STATUS_NORMAL   = (0 << 2),        /**< normal operation */
    ENS160_STATUS_WARM_UP  = (1 << 2),        /**< warm - up phase */
    ENS160_STATUS_START_UP = (2 << 2),        /**< initial start - up phase */
    ENS160_STATUS_INVALID  = (3 << 2),        /**< invalid output */
    ENS160_STATUS_NEWDAT   = (1 << 1),        /**< a new data is available in the data register */
    ENS160_STATUS_NEWGPR   = (1 << 0),        /**< a new data is available in the gpr read register */
} ens160_status_t;

/**
 * @brief ens160 handle structure definition
 */
typedef struct ens160_handle_s
{
    uint8_t iic_addr;                                                                   /**< iic address */
    uint8_t (*iic_init)(void);                                                          /**< point to an iic_init function address */
    uint8_t (*iic_deinit)(void);                                                        /**< point to an iic_deinit function address */
    uint8_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);         /**< point to an iic_read function address */
    uint8_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);        /**< point to an iic_write function address */
    uint8_t (*spi_init)(void);                                                          /**< point to a spi_init function address */
    uint8_t (*spi_deinit)(void);                                                        /**< point to a spi_deinit function address */
    uint8_t (*spi_read)(uint8_t reg, uint8_t *buf, uint16_t len);                       /**< point to a spi_read function address */
    uint8_t (*spi_write)(uint8_t reg, uint8_t *buf, uint16_t len);                      /**< point to a spi_write function address */
    void (*receive_callback)(uint8_t type, uint16_t *dat);                              /**< point to a receive_callback function address */
    void (*delay_ms)(uint32_t ms);                                                      /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);                                    /**< point to a debug_print function address */
    uint8_t inited;                                                                     /**< inited flag */
    uint8_t iic_spi;                                                                    /**< iic spi interface type */
} ens160_handle_t;

/**
 * @brief ens160 information structure definition
 */
typedef struct ens160_info_s
{
    char chip_name[32];                /**< chip name */
    char manufacturer_name[32];        /**< manufacturer name */
    char interface[8];                 /**< chip interface name */
    float supply_voltage_min_v;        /**< chip min supply voltage */
    float supply_voltage_max_v;        /**< chip max supply voltage */
    float max_current_ma;              /**< chip max current */
    float temperature_min;             /**< chip min operating temperature */
    float temperature_max;             /**< chip max operating temperature */
    uint32_t driver_version;           /**< driver version */
} ens160_info_t;

/**
 * @}
 */

/**
 * @defgroup ens160_link_driver ens160 link driver function
 * @brief    ens160 link driver modules
 * @ingroup  ens160_driver
 * @{
 */

/**
 * @brief     initialize ens160_handle_t structure
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] STRUCTURE ens160_handle_t
 * @note      none
 */
#define DRIVER_ENS160_LINK_INIT(HANDLE, STRUCTURE)         memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link iic_init function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to an iic_init function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_IIC_INIT(HANDLE, FUC)          (HANDLE)->iic_init = FUC

/**
 * @brief     link iic_deinit function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to an iic_deinit function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_IIC_DEINIT(HANDLE, FUC)        (HANDLE)->iic_deinit = FUC

/**
 * @brief     link iic_read function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to an iic_read function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_IIC_READ(HANDLE, FUC)          (HANDLE)->iic_read = FUC

/**
 * @brief     link iic_write function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to an iic_write function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_IIC_WRITE(HANDLE, FUC)         (HANDLE)->iic_write = FUC

/**
 * @brief     link spi_init function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to a spi_init function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_SPI_INIT(HANDLE, FUC)          (HANDLE)->spi_init = FUC

/**
 * @brief     link spi_deinit function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to a spi_deinit function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_SPI_DEINIT(HANDLE, FUC)        (HANDLE)->spi_deinit = FUC

/**
 * @brief     link spi_read function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to a spi_read function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_SPI_READ(HANDLE, FUC)          (HANDLE)->spi_read = FUC

/**
 * @brief     link spi_write function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to a spi_write function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_SPI_WRITE(HANDLE, FUC)         (HANDLE)->spi_write = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to a delay_ms function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_DELAY_MS(HANDLE, FUC)          (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to a debug_print function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_DEBUG_PRINT(HANDLE, FUC)       (HANDLE)->debug_print = FUC

/**
 * @brief     link receive_callback function
 * @param[in] HANDLE pointer to an ens160 handle structure
 * @param[in] FUC pointer to a receive_callback function address
 * @note      none
 */
#define DRIVER_ENS160_LINK_RECEIVE_CALLBACK(HANDLE, FUC)  (HANDLE)->receive_callback = FUC

/**
 * @}
 */

/**
 * @defgroup ens160_basic_driver ens160 basic driver function
 * @brief    ens160 basic driver modules
 * @ingroup  ens160_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info pointer to an ens160 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ens160_info(ens160_info_t *info);

/**
 * @brief     set the chip interface
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] interface chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t ens160_set_interface(ens160_handle_t *handle, ens160_interface_t interface);

/**
 * @brief      get the chip interface
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *interface pointer to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ens160_get_interface(ens160_handle_t *handle, ens160_interface_t *interface);

/**
 * @brief     set the iic address pin
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] addr_pin address pin
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t ens160_set_addr_pin(ens160_handle_t *handle, ens160_address_t addr_pin);

/**
 * @brief      get the iic address pin
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *addr_pin pointer to an address pin buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ens160_get_addr_pin(ens160_handle_t *handle, ens160_address_t *addr_pin);

/**
 * @brief     irq handler
 * @param[in] *handle pointer to an ens160 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      ENS160_STATUS_NEWDAT
 *            dat[0] AQI
 *            dat[1] TVOC in ppb
 *            dat[2] ECO2 in ppm
 *            dat[3] ETOH in ppb
 *            ENS160_STATUS_NEWGPR
 *            dat[0] resistance0 raw
 *            dat[1] resistance1 raw
 *            dat[2] resistance2 raw
 *            dat[3] resistance3 raw
 */
uint8_t ens160_irq_handler(ens160_handle_t *handle);

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to an ens160 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 spi or iic initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 id is invalid
 * @note      none
 */
uint8_t ens160_init(ens160_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle pointer to an ens160 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic or spi deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 power down failed
 * @note      none
 */
uint8_t ens160_deinit(ens160_handle_t *handle);

/**
 * @brief      read aqi
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *uba pointer to a uba buffer
 * @return     status code
 *             - 0 success
 *             - 1 read aqi failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_read_aqi(ens160_handle_t *handle, uint8_t *uba);

/**
 * @brief      read tvoc
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *tvoc_ppb pointer to a tvoc buffer
 * @return     status code
 *             - 0 success
 *             - 1 read tvoc failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_read_tvoc(ens160_handle_t *handle, uint16_t *tvoc_ppb);

/**
 * @brief      read eco2
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *eco2_ppm pointer to a eco2 buffer
 * @return     status code
 *             - 0 success
 *             - 1 read eco2 failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_read_eco2(ens160_handle_t *handle, uint16_t *eco2_ppm);

/**
 * @brief      read etoh
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *etoh_ppb pointer to a etoh buffer
 * @return     status code
 *             - 0 success
 *             - 1 read etoh failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_read_etoh(ens160_handle_t *handle, uint16_t *etoh_ppb);

/**
 * @brief     set mode
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] mode chip mode
 * @return    status code
 *            - 0 success
 *            - 1 set mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_mode(ens160_handle_t *handle, ens160_mode_t mode);

/**
 * @brief      get mode
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *mode pointer to a chip mode buffer
 * @return     status code
 *             - 0 success
 *             - 1 get mode failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_mode(ens160_handle_t *handle, ens160_mode_t *mode);

/**
 * @brief     soft reset
 * @param[in] *handle pointer to an ens160 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 soft reset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_soft_reset(ens160_handle_t *handle);

/**
 * @brief     set interrupt pin polarity
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] polarity interrupt polarity
 * @return    status code
 *            - 0 success
 *            - 1 set interrupt pin polarity failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_interrupt_pin_polarity(ens160_handle_t *handle, ens160_pin_polarity_t polarity);

/**
 * @brief      get interrupt pin polarity
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *polarity pointer to an interrupt polarity buffer
 * @return     status code
 *             - 0 success
 *             - 1 get interrupt pin polarity failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_interrupt_pin_polarity(ens160_handle_t *handle, ens160_pin_polarity_t *polarity);

/**
 * @brief     set interrupt pin drive
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] drive pin drive
 * @return    status code
 *            - 0 success
 *            - 1 set interrupt pin drive failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_interrupt_pin_drive(ens160_handle_t *handle, ens160_pin_drive_t drive);

/**
 * @brief      get interrupt pin drive
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *drive pointer to a pin drive buffer
 * @return     status code
 *             - 0 success
 *             - 1 get interrupt pin drive failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_interrupt_pin_drive(ens160_handle_t *handle, ens160_pin_drive_t *drive);

/**
 * @brief     set general purpose read interrupt pin asserted
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set general purpose read interrupt pin asserted failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_general_purpose_read_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t enable);

/**
 * @brief      get general purpose read interrupt pin asserted
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get general purpose read interrupt pin asserted failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_general_purpose_read_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t *enable);

/**
 * @brief     set data interrupt pin asserted
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set data interrupt pin asserted failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_data_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t enable);

/**
 * @brief      get data interrupt pin asserted
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get data interrupt pin asserted failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_data_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t *enable);

/**
 * @brief     enable or disable interrupt
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set interrupt failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_interrupt(ens160_handle_t *handle, ens160_bool_t enable);

/**
 * @brief      get interrupt
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get interrupt failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_interrupt(ens160_handle_t *handle, ens160_bool_t *enable);

/**
 * @brief     nop
 * @param[in] *handle pointer to an ens160 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 nop failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_nop(ens160_handle_t *handle);

/**
 * @brief      get app version
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *major pointer to a major buffer
 * @param[out] *minor pointer to a minor buffer
 * @param[out] *release pointer to a release buffer
 * @return     status code
 *             - 0 success
 *             - 1 get app version failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_app_version(ens160_handle_t *handle, uint8_t *major, uint8_t *minor, uint8_t *release);

/**
 * @brief     clear
 * @param[in] *handle pointer to an ens160 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 clear failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_clear(ens160_handle_t *handle);

/**
 * @brief     set temperature compensation
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] raw raw data
 * @return    status code
 *            - 0 success
 *            - 1 set temperature compensation failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_temperature_compensation(ens160_handle_t *handle, uint16_t raw);

/**
 * @brief      get temperature compensation
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *raw pointer to a raw data buffer
 * @return     status code
 *             - 0 success
 *             - 1 get temperature compensation failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_temperature_compensation(ens160_handle_t *handle, uint16_t *raw);

/**
 * @brief     set humidity compensation
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] raw raw data
 * @return    status code
 *            - 0 success
 *            - 1 set humidity compensation failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_humidity_compensation(ens160_handle_t *handle, uint16_t raw);

/**
 * @brief      get humidity compensation
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *raw pointer to a raw data buffer
 * @return     status code
 *             - 0 success
 *             - 1 get humidity compensation failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_humidity_compensation(ens160_handle_t *handle, uint16_t *raw);

/**
 * @brief      get status
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *status pointer to a status buffer
 * @return     status code
 *             - 0 success
 *             - 1 get status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_status(ens160_handle_t *handle, uint8_t *status);

/**
 * @brief      get calculation temperature
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *raw pointer to a raw data buffer
 * @return     status code
 *             - 0 success
 *             - 1 get calculation temperature failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_calculation_temperature(ens160_handle_t *handle, uint16_t *raw);

/**
 * @brief      get calculation humidity
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *raw pointer to a raw data buffer
 * @return     status code
 *             - 0 success
 *             - 1 get calculation humidity failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_calculation_humidity(ens160_handle_t *handle, uint16_t *raw);

/**
 * @brief      get checksum
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *checksum pointer to a checksum buffer
 * @return     status code
 *             - 0 success
 *             - 1 get checksum failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_checksum(ens160_handle_t *handle, uint8_t *checksum);

/**
 * @brief     set params
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] *params pointer to a params buffer
 * @return    status code
 *            - 0 success
 *            - 1 set params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_params(ens160_handle_t *handle, uint8_t params[8]);

/**
 * @brief      get params
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *params pointer to a params buffer
 * @return     status code
 *             - 0 success
 *             - 1 get params failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_params(ens160_handle_t *handle, uint8_t params[8]);

/**
 * @brief      get params output
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *params pointer to a params buffer
 * @return     status code
 *             - 0 success
 *             - 1 get params output failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_params_output(ens160_handle_t *handle, uint8_t params[8]);

/**
 * @brief      convert the temperature to the register raw data
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  celsius_deg celsius degrees
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_temperature_convert_to_register(ens160_handle_t *handle, float celsius_deg, uint16_t *reg);

/**
 * @brief      convert the register raw data to the temperature
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  reg register raw data
 * @param[out] *celsius_deg pointer to a celsius degrees buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_temperature_convert_to_data(ens160_handle_t *handle, uint16_t reg, float *celsius_deg);

/**
 * @brief      convert the humidity to the register raw data
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  percentage humidity percentage
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_humidity_convert_to_register(ens160_handle_t *handle, float percentage, uint16_t *reg);

/**
 * @brief      convert the register raw data to the humidity
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  reg register raw data
 * @param[out] *percentage pointer to a humidity percentage buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_humidity_convert_to_data(ens160_handle_t *handle, uint16_t reg, float *percentage);

/**
 * @brief      convert the resistance to the register raw data
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  ohm resistance in ohm
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_resistance_convert_to_register(ens160_handle_t *handle, float ohm, uint16_t *reg);

/**
 * @brief      convert the register raw data to the resistance
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  reg register raw data
 * @param[out] *ohm pointer to an ohm buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_resistance_convert_to_data(ens160_handle_t *handle, uint16_t reg, float *ohm);

/**
 * @brief      resistance split
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  *raw pointer to a raw data buffer
 * @param[out] *resistance0 pointer to a resistance0 buffer
 * @param[out] *resistance1 pointer to a resistance1 buffer
 * @param[out] *resistance2 pointer to a resistance2 buffer
 * @param[out] *resistance3 pointer to a resistance3 buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_resistance_split(ens160_handle_t *handle, uint8_t raw[8],
                                uint16_t *resistance0, uint16_t *resistance1,
                                uint16_t *resistance2, uint16_t *resistance3);

/**
 * @}
 */

/**
 * @defgroup ens160_extern_driver ens160 extern driver function
 * @brief    ens160 extern driver modules
 * @ingroup  ens160_driver
 * @{
 */

/**
 * @brief     set the chip register
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ens160_set_reg(ens160_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief      get the chip register
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ens160_get_reg(ens160_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
