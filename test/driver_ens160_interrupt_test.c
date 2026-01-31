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
 * @file      driver_ens160_interrupt_test.c
 * @brief     driver ens160 interrupt test source file
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

#include "driver_ens160_interrupt_test.h"

static ens160_handle_t gs_handle;        /**< ens160 handle */
static volatile uint8_t gs_flag;         /**< local flag */

/**
 * @brief     interface receive callback
 * @param[in] type irq type
 * @param[in] *dat pointer to a data buffer
 * @note      none
 */
static void a_callback(uint8_t type, uint16_t *dat)
{
    switch (type)
    {
        case ENS160_STATUS_NEWDAT :
        {
            ens160_interface_debug_print("ens160: irq newdat.\n");
            ens160_interface_debug_print("ens160: aqi is %d.\n", dat[0]);
            ens160_interface_debug_print("ens160: tvoc is %d ppb.\n", dat[1]);
            ens160_interface_debug_print("ens160: eco2 is %d ppm.\n", dat[2]);
            ens160_interface_debug_print("ens160: etoh is %d ppb.\n", dat[3]);
            gs_flag = 1;
            
            break;
        }
        case ENS160_STATUS_NEWGPR :
        {
            ens160_interface_debug_print("ens160: irq newgpr.\n");
            ens160_interface_debug_print("ens160: resistance0 is 0x%04X\n", dat[0]);
            ens160_interface_debug_print("ens160: resistance1 is 0x%04X.\n", dat[1]);
            ens160_interface_debug_print("ens160: resistance2 is 0x%04X.\n", dat[2]);
            ens160_interface_debug_print("ens160: resistance3 is 0x%04X.\n", dat[3]);
            
            break;
        }
        default :
        {
            ens160_interface_debug_print("ens160: unknown code.\n");
            
            break;
        }
    }
}

/**
 * @brief  interrupt test irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t ens160_interrupt_test_irq_handler(void)
{
    if (ens160_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     interrupt test
 * @param[in] interface chip interface
 * @param[in] addr_pin iic device address
 * @param[in] times test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t ens160_interrupt_test(ens160_interface_t interface, ens160_address_t addr_pin, uint32_t times)
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
    DRIVER_ENS160_LINK_RECEIVE_CALLBACK(&gs_handle, a_callback);
    
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
    
    /* start interrupt test */
    ens160_interface_debug_print("ens160: start interrupt test.\n");
    
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
    
    /* disable general purpose read interrupt pin asserted */
    res = ens160_set_general_purpose_read_interrupt_pin_asserted(&gs_handle, ENS160_BOOL_FALSE);
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
    
    /* enable interrupt */
    res = ens160_set_interrupt(&gs_handle, ENS160_BOOL_TRUE);
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
    
    for (i = 0; i < times; i++)
    {
        /* wait for one group output */
        gs_flag = 0;
        while (gs_flag == 0)
        {
        }
        gs_flag = 0;
        
        ens160_interface_debug_print("ens160: find interrupt.\n");
    }
    
    /* finish interrupt test */
    ens160_interface_debug_print("ens160: finish interrupt test.\n");
    (void)ens160_deinit(&gs_handle);
    
    return 0;
}
