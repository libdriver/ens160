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
 * @file      driver_ens160_read_test.c
 * @brief     driver ens160 read test source file
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

#include "driver_ens160_read_test.h"

static ens160_handle_t gs_handle;        /**< ens160 handle */

/**
 * @brief     read test
 * @param[in] interface chip interface
 * @param[in] addr_pin iic device address
 * @param[in] times test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t ens160_read_test(ens160_interface_t interface, ens160_address_t addr_pin, uint32_t times)
{
    uint8_t res;
    uint32_t i;
    ens160_info_t info;
    
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
    
    /* get information */
    res = ens160_info(&info);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get info failed.\n");
       
        return 1;
    }
    else
    {
        /* print chip info */
        ens160_interface_debug_print("ens160: chip is %s.\n", info.chip_name);
        ens160_interface_debug_print("ens160: manufacturer is %s.\n", info.manufacturer_name);
        ens160_interface_debug_print("ens160: interface is %s.\n", info.interface);
        ens160_interface_debug_print("ens160: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ens160_interface_debug_print("ens160: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        ens160_interface_debug_print("ens160: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        ens160_interface_debug_print("ens160: max current is %0.2fmA.\n", info.max_current_ma);
        ens160_interface_debug_print("ens160: max temperature is %0.1fC.\n", info.temperature_max);
        ens160_interface_debug_print("ens160: min temperature is %0.1fC.\n", info.temperature_min);
    }
    
    /* start read test */
    ens160_interface_debug_print("ens160: start read test.\n");
    
    /* set the interface */
    res = ens160_set_interface(&gs_handle, interface);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interface failed.\n");
       
        return 1;
    }
    
    /* set the address pin */
    res = ens160_set_addr_pin(&gs_handle, addr_pin);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set addr pin failed.\n");
       
        return 1;
    }
    
    /* init ens160 */
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
    
    /* set interrupt pin polarity low */
    res = ens160_set_interrupt_pin_polarity(&gs_handle, ENS160_PIN_POLARITY_LOW);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt pin polarity failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set interrupt pin drive push-pull */
    res = ens160_set_interrupt_pin_drive(&gs_handle, ENS160_PIN_DRIVE_PUSH_PULL);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt pin drive failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    
    /* enable general purpose read interrupt pin asserted */
    res = ens160_set_general_purpose_read_interrupt_pin_asserted(&gs_handle, ENS160_BOOL_TRUE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set general purpose read interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    
    /* enable data interrupt pin asserted */
    res = ens160_set_data_interrupt_pin_asserted(&gs_handle, ENS160_BOOL_TRUE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set data interrupt pin asserted failed.\n");
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
    
    /* output */
    ens160_interface_debug_print("ens160: ens160_read_aqi test.\n");
    
    for (i = 0; i < times; i++)
    {
        uint8_t uba;
        
        /* delay 1000ms */
        ens160_interface_delay_ms(1000);
        
        /* read aqi */
        res = ens160_read_aqi(&gs_handle, &uba);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: read aqi failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* output */
        ens160_interface_debug_print("ens160: aqi is %d.\n", uba);
    }
    
    /* output */
    ens160_interface_debug_print("ens160: ens160_read_tvoc test.\n");
    
    for (i = 0; i < times; i++)
    {
        uint16_t tvoc_ppb;
        
        /* delay 1000ms */
        ens160_interface_delay_ms(1000);
        
        /* read tvoc */
        res = ens160_read_tvoc(&gs_handle, &tvoc_ppb);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: read tvoc failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* output */
        ens160_interface_debug_print("ens160: tvoc is %d ppb.\n", tvoc_ppb);
    }
    
    /* output */
    ens160_interface_debug_print("ens160: ens160_read_eco2 test.\n");
    
    for (i = 0; i < times; i++)
    {
        uint16_t eco2_ppm;
        
        /* delay 1000ms */
        ens160_interface_delay_ms(1000);
        
        /* read eco2 */
        res = ens160_read_eco2(&gs_handle, &eco2_ppm);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: read eco2 failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* output */
        ens160_interface_debug_print("ens160: eco2 is %d ppm.\n", eco2_ppm);
    }
    
    /* output */
    ens160_interface_debug_print("ens160: ens160_read_etoh test.\n");
    
    for (i = 0; i < times; i++)
    {
        uint16_t etoh_ppb;
        
        /* delay 1000ms */
        ens160_interface_delay_ms(1000);
        
        /* read etoh */
        res = ens160_read_etoh(&gs_handle, &etoh_ppb);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: read etoh failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* output */
        ens160_interface_debug_print("ens160: etoh is %d ppb.\n", etoh_ppb);
    }
    
    /* output */
    ens160_interface_debug_print("ens160: ens160_get_params_output test.\n");
    
    for (i = 0; i < times; i++)
    {
        uint8_t params[8];
        uint16_t resistance0;
        uint16_t resistance1;
        uint16_t resistance2;
        uint16_t resistance3;
        float resistance0_ohm;
        float resistance1_ohm;
        float resistance2_ohm;
        float resistance3_ohm;
        
        /* delay 1000ms */
        ens160_interface_delay_ms(1000);
        
        /* get params output */
        res = ens160_get_params_output(&gs_handle, params);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: get params output failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* resistance split */
        res = ens160_resistance_split(&gs_handle, params,
                                      &resistance0, &resistance1,
                                      &resistance2, &resistance3);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: resistance split failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* resistance convert to data */
        res = ens160_resistance_convert_to_data(&gs_handle, resistance0, &resistance0_ohm);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: resistance convert to data failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* resistance convert to data */
        res = ens160_resistance_convert_to_data(&gs_handle, resistance1, &resistance1_ohm);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: resistance convert to data failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* resistance convert to data */
        res = ens160_resistance_convert_to_data(&gs_handle, resistance2, &resistance2_ohm);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: resistance convert to data failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* resistance convert to data */
        res = ens160_resistance_convert_to_data(&gs_handle, resistance3, &resistance3_ohm);
        if (res != 0)
        {
            ens160_interface_debug_print("ens160: resistance convert to data failed.\n");
            (void)ens160_deinit(&gs_handle);
            
            return 1;
        }
        
        /* output */
        ens160_interface_debug_print("ens160: resistance0 is %0.2f ohm.\n", resistance0_ohm);
        ens160_interface_debug_print("ens160: resistance1 is %0.2f ohm.\n", resistance1_ohm);
        ens160_interface_debug_print("ens160: resistance2 is %0.2f ohm.\n", resistance2_ohm);
        ens160_interface_debug_print("ens160: resistance3 is %0.2f ohm.\n", resistance3_ohm);
    }
    
    /* finish read test */
    ens160_interface_debug_print("ens160: finish read test.\n");
    (void)ens160_deinit(&gs_handle);
    
    return 0;
}
