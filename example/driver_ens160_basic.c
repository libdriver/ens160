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
 * @file      driver_ens160_basic.c
 * @brief     driver ens160 basic source file
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

#include "driver_ens160_basic.h"

static ens160_handle_t gs_handle;        /**< ens160 handle */

/**
 * @brief     basic example init
 * @param[in] interface chip interface
 * @param[in] addr_pin iic device address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t ens160_basic_init(ens160_interface_t interface, ens160_address_t addr_pin)
{
    uint8_t res;
    
    /* link interface function */
    DRIVER_ENS160_LINK_INIT(&gs_handle, ens160_handle_t);
    DRIVER_ENS160_LINK_IIC_INIT(&gs_handle, ens160_interface_iic_init);
    DRIVER_ENS160_LINK_IIC_DEINIT(&gs_handle, ens160_interface_iic_deinit);
    DRIVER_ENS160_LINK_IIC_READ(&gs_handle, ens160_interface_iic_read);
    DRIVER_ENS160_LINK_IIC_WRITE(&gs_handle, ens160_interface_iic_write);
    DRIVER_ENS160_LINK_SPI_INIT(&gs_handle, ens160_interface_spi_init);
    DRIVER_ENS160_LINK_SPI_DEINIT(&gs_handle, ens160_interface_spi_deinit);
    DRIVER_ENS160_LINK_SPI_READ(&gs_handle, ens160_interface_spi_read);
    DRIVER_ENS160_LINK_SPI_WRITE(&gs_handle, ens160_interface_spi_write);
    DRIVER_ENS160_LINK_DELAY_MS(&gs_handle, ens160_interface_delay_ms);
    DRIVER_ENS160_LINK_DEBUG_PRINT(&gs_handle, ens160_interface_debug_print);
    DRIVER_ENS160_LINK_RECEIVE_CALLBACK(&gs_handle, ens160_interface_receive_callback);
    
    /* set the interface */
    res = ens160_set_interface(&gs_handle, interface);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interface failed.\n");
       
        return 1;
    }
    
    /* set addr pin */
    res = ens160_set_addr_pin(&gs_handle, addr_pin);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set addr pin failed.\n");
       
        return 1;
    }
    
    /* ens160 initialization */
    res = ens160_init(&gs_handle);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: init failed.\n");
       
        return 1;
    }
    
    /* soft reset */
    res = ens160_soft_reset(&gs_handle);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: soft reset failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    
    /* disable interrupt */
    res = ens160_set_interrupt(&gs_handle, ENS160_BOOL_FALSE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set standard mode */
    res = ens160_set_mode(&gs_handle, ENS160_MODE_STANDARD);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set mode failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example read
 * @param[out] *aqi_uba pointer to an aqi buffer
 * @param[out] *tvoc_ppb pointer to a tvoc buffer
 * @param[out] *eco2_ppm pointer to a eco2 buffer
 * @param[out] *etoh_ppb pointer to a etoh buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ens160_basic_read(uint8_t *aqi_uba, uint16_t *tvoc_ppb, uint16_t *eco2_ppm, uint16_t *etoh_ppb)
{
    /* read aqi */
    if (ens160_read_aqi(&gs_handle, aqi_uba) != 0)
    {
        return 1;
    }
    
    /* read tvoc */
    if (ens160_read_tvoc(&gs_handle, tvoc_ppb) != 0)
    {
        return 1;
    }
    
    /* read eco2 */
    if (ens160_read_eco2(&gs_handle, eco2_ppm) != 0)
    {
        return 1;
    }
    
    /* read etoh */
    if (ens160_read_etoh(&gs_handle, etoh_ppb) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ens160_basic_deinit(void)
{
    if (ens160_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example read raw
 * @param[out] *resistance_ohm pointer to a resistance ohm buffer
 * @return     status code
 *             - 0 success
 *             - 1 read raw failed
 * @note       none
 */
uint8_t ens160_basic_read_raw(float resistance_ohm[4])
{
    uint8_t res;
    uint8_t params[8];
    uint16_t resistance0;
    uint16_t resistance1;
    uint16_t resistance2;
    uint16_t resistance3;
    
    /* get params output */
    res = ens160_get_params_output(&gs_handle, params);
    if (res != 0)
    {
        return 1;
    }
    
    /* resistance split */
    res = ens160_resistance_split(&gs_handle, params, &resistance0, &resistance1, &resistance2, &resistance3);
    if (res != 0)
    {
        return 1;
    }
    
    /* resistance convert to data */
    res = ens160_resistance_convert_to_data(&gs_handle, resistance0, &resistance_ohm[0]);
    if (res != 0)
    {
        return 1;
    }
    
    /* resistance convert to data */
    res = ens160_resistance_convert_to_data(&gs_handle, resistance1, &resistance_ohm[1]);
    if (res != 0)
    {
        return 1;
    }
    
    res = ens160_resistance_convert_to_data(&gs_handle, resistance2, &resistance_ohm[2]);
    if (res != 0)
    {
        return 1;
    }
    
    /* resistance convert to data */
    res = ens160_resistance_convert_to_data(&gs_handle, resistance3, &resistance_ohm[3]);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     basic example set compensation
 * @param[in] celsius_deg celsius degress
 * @param[in] percentage humidity percentage
 * @return    status code
 *            - 0 success
 *            - 1 set compensation failed
 * @note      none
 */
uint8_t ens160_basic_set_compensation(float celsius_deg, float percentage)
{
    uint8_t res;
    uint16_t raw;
    
    /* temperature convert to register */
    res = ens160_temperature_convert_to_register(&gs_handle, celsius_deg, &raw);
    if (res != 0)
    {
        return 1;
    }
    
    /* set temperature compensation */
    res = ens160_set_temperature_compensation(&gs_handle, raw);
    if (res != 0)
    {
        return 1;
    }
    
    /* humidity convert to register */
    res = ens160_humidity_convert_to_register(&gs_handle, percentage, &raw);
    if (res != 0)
    {
        return 1;
    }
    
    /* set humidity compensation */
    res = ens160_set_humidity_compensation(&gs_handle, raw);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example get compensation
 * @param[out] *celsius_deg pointer to a celsius degress buffer
 * @param[out] *percentage pointer to a humidity percentage buffer
 * @return     status code
 *             - 0 success
 *             - 1 get compensation failed
 * @note       none
 */
uint8_t ens160_basic_get_compensation(float *celsius_deg, float *percentage)
{
    uint8_t res;
    uint16_t raw;
    
    /* get temperature compensation */
    res = ens160_get_temperature_compensation(&gs_handle, &raw);
    if (res != 0)
    {
        return 1;
    }
    
    /* temperature convert to data */
    res = ens160_temperature_convert_to_data(&gs_handle, raw, celsius_deg);
    if (res != 0)
    {
        return 1;
    }
    
    /* get humidity compensation */
    res = ens160_get_humidity_compensation(&gs_handle, &raw);
    if (res != 0)
    {
        return 1;
    }
    
    /* humidity convert to data */
    res = ens160_humidity_convert_to_data(&gs_handle, raw, percentage);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}
