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
 * @file      driver_ens160_interrupt.h
 * @brief     driver ens160 interrupt header file
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

#ifndef DRIVER_ENS160_INTERRUPT_H
#define DRIVER_ENS160_INTERRUPT_H

#include "driver_ens160_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @addtogroup ens160_example_driver
 * @{
 */

/**
 * @brief ens160 interrupt example default definition
 */
#define ENS160_INTERRUPT_DEFAULT_PIN_POLARITY                                       ENS160_PIN_POLARITY_LOW           /**< active low */
#define ENS160_INTERRUPT_DEFAULT_PIN_DRIVE                                          ENS160_PIN_DRIVE_PUSH_PULL        /**< push-pull */
#define ENS160_INTERRUPT_DEFAULT_GENERAL_PURPOSE_READ_INTERRUPT_PIN_ASSERTED        ENS160_BOOL_FALSE                 /**< disable */
#define ENS160_INTERRUPT_DEFAULT_DATA_INTERRUPT_PIN_ASSERTED                        ENS160_BOOL_TRUE                  /**< enable */

/**
 * @brief  interrupt irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t ens160_interrupt_irq_handler(void);

/**
 * @brief     interrupt example init
 * @param[in] interface chip interface
 * @param[in] addr_pin iic device address
 * @param[in] *callback pointer to a callback function
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t ens160_interrupt_init(ens160_interface_t interface, ens160_address_t addr_pin, void (*callback)(uint8_t type, uint16_t *dat));

/**
 * @brief  interrupt example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ens160_interrupt_deinit(void);

/**
 * @brief      interrupt example read
 * @param[out] *aqi_uba pointer to an aqi buffer
 * @param[out] *tvoc_ppb pointer to a tvoc buffer
 * @param[out] *eco2_ppm pointer to an eco2 buffer
 * @param[out] *etoh_ppb pointer to a etoh buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ens160_interrupt_read(uint8_t *aqi_uba, uint16_t *tvoc_ppb, uint16_t *eco2_ppm, uint16_t *etoh_ppb);

/**
 * @brief      interrupt example read raw
 * @param[out] *resistance_ohm pointer to a resistance ohm buffer
 * @return     status code
 *             - 0 success
 *             - 1 read raw failed
 * @note       none
 */
uint8_t ens160_interrupt_read_raw(float resistance_ohm[4]);

/**
 * @brief     interrupt example set compensation
 * @param[in] celsius_deg celsius degrees
 * @param[in] percentage humidity percentage
 * @return    status code
 *            - 0 success
 *            - 1 set compensation failed
 * @note      none
 */
uint8_t ens160_interrupt_set_compensation(float celsius_deg, float percentage);

/**
 * @brief      interrupt example get compensation
 * @param[out] *celsius_deg pointer to a celsius degrees buffer
 * @param[out] *percentage pointer to a humidity percentage buffer
 * @return     status code
 *             - 0 success
 *             - 1 get compensation failed
 * @note       none
 */
uint8_t ens160_interrupt_get_compensation(float *celsius_deg, float *percentage);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
