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
 * @file      driver_ens160_register_test.c
 * @brief     driver ens160 register test source file
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

#include "driver_ens160_register_test.h"
#include <stdlib.h>

static ens160_handle_t gs_handle;        /**< ens160 handle */

/**
 * @brief     register test
 * @param[in] interface chip interface
 * @param[in] addr_pin iic device address
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t ens160_register_test(ens160_interface_t interface, ens160_address_t addr_pin)
{
    uint8_t res;
    uint8_t major;
    uint8_t minor;
    uint8_t release;
    uint8_t status;
    uint8_t checksum;
    uint16_t raw;
    uint16_t raw_check;
    uint16_t reg;
    uint16_t resistance0;
    uint16_t resistance1;
    uint16_t resistance2;
    uint16_t resistance3;
    float celsius_deg;
    float celsius_deg_check;
    float percentage;
    float percentage_check;
    float ohm;
    float ohm_check;
    uint8_t params[8];
    uint8_t params_check[8];
    ens160_info_t info;
    ens160_interface_t interface_check;
    ens160_address_t addr_check;
    ens160_mode_t mode;
    ens160_pin_polarity_t polarity;
    ens160_pin_drive_t drive;
    ens160_bool_t enable;
    
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
    
    /* start register test */
    ens160_interface_debug_print("ens160: start register test.\n");
    
    /* ens160_set_interface/ens160_get_interface test */
    ens160_interface_debug_print("ens160: ens160_set_interface/ens160_get_interface test.\n");
    
    /* set IIC interface */
    res = ens160_set_interface(&gs_handle, ENS160_INTERFACE_IIC);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interface failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: set interface iic.\n");
    res = ens160_get_interface(&gs_handle, &interface_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interface failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: check interface %s.\n", (interface_check == ENS160_INTERFACE_IIC) ? "ok" : "error");
    
    /* set SPI interface */
    res = ens160_set_interface(&gs_handle, ENS160_INTERFACE_SPI);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interface failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: set interface spi.\n");
    res = ens160_get_interface(&gs_handle, &interface_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interface failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: check interface %s.\n", (interface_check == ENS160_INTERFACE_SPI) ? "ok" : "error");
    
    /* ens160_set_addr_pin/ens160_get_addr_pin test */
    ens160_interface_debug_print("ens160: ens160_set_addr_pin/ens160_get_addr_pin test.\n");
    
    /* address0 */
    res = ens160_set_addr_pin(&gs_handle, ENS160_ADDRESS_0);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set addr pin failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: set address0.\n");
    res = ens160_get_addr_pin(&gs_handle, &addr_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get addr pin failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: check addr pin %s.\n", (addr_check == ENS160_ADDRESS_0) ? "ok" : "error");
    
    /* address1 */
    res = ens160_set_addr_pin(&gs_handle, ENS160_ADDRESS_1);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set addr pin failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: set address1.\n");
    res = ens160_get_addr_pin(&gs_handle, &addr_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get addr pin failed.\n");
       
        return 1;
    }
    ens160_interface_debug_print("ens160: check addr pin %s.\n", (addr_check == ENS160_ADDRESS_1) ? "ok" : "error");
    
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
    
    /* init the ens160 */
    res = ens160_init(&gs_handle);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: init failed.\n");
       
        return 1;
    }
    
    /* ens160_set_mode/ens160_get_mode test */
    ens160_interface_debug_print("ens160: ens160_set_mode/ens160_get_mode test.\n");
    
    /* set deep sleep mode */
    res = ens160_set_mode(&gs_handle, ENS160_MODE_DEEP_SLEEP);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set mode failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set deep sleep mode.\n");
    res = ens160_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get mode failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check mode %s.\n", (mode == ENS160_MODE_DEEP_SLEEP) ? "ok" : "error");
    
    /* set standard mode */
    res = ens160_set_mode(&gs_handle, ENS160_MODE_STANDARD);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set mode failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set standard mode.\n");
    res = ens160_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get mode failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check mode %s.\n", (mode == ENS160_MODE_STANDARD) ? "ok" : "error");
    
    /* set idle mode */
    res = ens160_set_mode(&gs_handle, ENS160_MODE_IDLE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set mode failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set idle mode.\n");
    res = ens160_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get mode failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check mode %s.\n", (mode == ENS160_MODE_IDLE) ? "ok" : "error");
    
    /* ens160_set_interrupt_pin_polarity/ens160_get_interrupt_pin_polarity test */
    ens160_interface_debug_print("ens160: ens160_set_interrupt_pin_polarity/ens160_get_interrupt_pin_polarity test.\n");
    
    /* set interrupt pin polarity high */
    res = ens160_set_interrupt_pin_polarity(&gs_handle, ENS160_PIN_POLARITY_HIGH);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt pin polarity failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set interrupt pin polarity high.\n");
    res = ens160_get_interrupt_pin_polarity(&gs_handle, &polarity);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interrupt pin polarity failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check interrupt pin polarity %s.\n", (polarity == ENS160_PIN_POLARITY_HIGH) ? "ok" : "error");
    
    /* set interrupt pin polarity low */
    res = ens160_set_interrupt_pin_polarity(&gs_handle, ENS160_PIN_POLARITY_LOW);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt pin polarity failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set interrupt pin polarity low.\n");
    res = ens160_get_interrupt_pin_polarity(&gs_handle, &polarity);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interrupt pin polarity failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check interrupt pin polarity %s.\n", (polarity == ENS160_PIN_POLARITY_LOW) ? "ok" : "error");
    
    /* ens160_set_interrupt_pin_drive/ens160_get_interrupt_pin_drive test */
    ens160_interface_debug_print("ens160: ens160_set_interrupt_pin_drive/ens160_get_interrupt_pin_drive test.\n");
    
    /* set interrupt pin drive push-pull */
    res = ens160_set_interrupt_pin_drive(&gs_handle, ENS160_PIN_DRIVE_PUSH_PULL);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt pin drive failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set interrupt pin drive push-pull.\n");
    res = ens160_get_interrupt_pin_drive(&gs_handle, &drive);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interrupt pin drive failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check interrupt pin drive %s.\n", (drive == ENS160_PIN_DRIVE_PUSH_PULL) ? "ok" : "error");
    
    /* set interrupt pin drive open drain */
    res = ens160_set_interrupt_pin_drive(&gs_handle, ENS160_PIN_DRIVE_OPEN_DRAIN);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt pin drive failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set interrupt pin drive open drain.\n");
    res = ens160_get_interrupt_pin_drive(&gs_handle, &drive);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interrupt pin drive failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check interrupt pin drive %s.\n", (drive == ENS160_PIN_DRIVE_OPEN_DRAIN) ? "ok" : "error");
    
    /* ens160_set_general_purpose_read_interrupt_pin_asserted/ens160_get_general_purpose_read_interrupt_pin_asserted test */
    ens160_interface_debug_print("ens160: ens160_set_general_purpose_read_interrupt_pin_asserted/");
    ens160_interface_debug_print("ens160_get_general_purpose_read_interrupt_pin_asserted test.\n");
    
    /* enable general purpose read interrupt pin asserted */
    res = ens160_set_general_purpose_read_interrupt_pin_asserted(&gs_handle, ENS160_BOOL_TRUE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set general purpose read interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: enable general purpose read interrupt pin asserted.\n");
    res = ens160_get_general_purpose_read_interrupt_pin_asserted(&gs_handle, &enable);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get general purpose read interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check general purpose read interrupt pin asserted %s.\n", (enable == ENS160_BOOL_TRUE) ? "ok" : "error");
    
    /* disable general purpose read interrupt pin asserted */
    res = ens160_set_general_purpose_read_interrupt_pin_asserted(&gs_handle, ENS160_BOOL_FALSE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set general purpose read interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: disable general purpose read interrupt pin asserted.\n");
    res = ens160_get_general_purpose_read_interrupt_pin_asserted(&gs_handle, &enable);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get general purpose read interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check general purpose read interrupt pin asserted %s.\n", (enable == ENS160_BOOL_FALSE) ? "ok" : "error");
    
    /* ens160_set_data_interrupt_pin_asserted/ens160_get_data_interrupt_pin_asserted test */
    ens160_interface_debug_print("ens160: ens160_set_data_interrupt_pin_asserted/ens160_get_data_interrupt_pin_asserted test.\n");
    
    /* enable data interrupt pin asserted */
    res = ens160_set_data_interrupt_pin_asserted(&gs_handle, ENS160_BOOL_TRUE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set data interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: enable data interrupt pin asserted.\n");
    res = ens160_get_data_interrupt_pin_asserted(&gs_handle, &enable);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get data interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check data interrupt pin asserted %s.\n", (enable == ENS160_BOOL_TRUE) ? "ok" : "error");
    
    /* disable data interrupt pin asserted */
    res = ens160_set_data_interrupt_pin_asserted(&gs_handle, ENS160_BOOL_FALSE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set data interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: disable data interrupt pin asserted.\n");
    res = ens160_get_data_interrupt_pin_asserted(&gs_handle, &enable);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get data interrupt pin asserted failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check data interrupt pin asserted %s.\n", (enable == ENS160_BOOL_FALSE) ? "ok" : "error");
    
    /* ens160_set_interrupt/ens160_get_interrupt test */
    ens160_interface_debug_print("ens160: ens160_set_interrupt/ens160_get_interrupt test.\n");
    
    /* enable interrupt */
    res = ens160_set_interrupt(&gs_handle, ENS160_BOOL_TRUE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: enable interrupt.\n");
    res = ens160_get_interrupt(&gs_handle, &enable);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interrupt failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check interrupt %s.\n", (enable == ENS160_BOOL_TRUE) ? "ok" : "error");
    
    /* disable interrupt */
    res = ens160_set_interrupt(&gs_handle, ENS160_BOOL_FALSE);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set interrupt failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: disable interrupt.\n");
    res = ens160_get_interrupt(&gs_handle, &enable);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get interrupt failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check interrupt %s.\n", (enable == ENS160_BOOL_FALSE) ? "ok" : "error");
    
    /* ens160_get_app_version test */
    ens160_interface_debug_print("ens160: ens160_get_app_version test.\n");
    
    /* get app version */
    res = ens160_get_app_version(&gs_handle, &major, &minor, &release);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get app version failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: app version is %d.%d.%d.\n", major, minor, release);
    
    /* ens160_set_temperature_compensation/ens160_get_temperature_compensation test */
    ens160_interface_debug_print("ens160: ens160_set_temperature_compensation/ens160_get_temperature_compensation test.\n");
    
    raw = rand() % 0xFFFFU;
    res = ens160_set_temperature_compensation(&gs_handle, raw);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set temperature compensation failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set temperature compensation 0x%04X.\n", raw);
    res = ens160_get_temperature_compensation(&gs_handle, &raw_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get temperature compensation failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check temperature compensation %s.\n", (raw == raw_check) ? "ok" : "error");
    
    /* ens160_set_humidity_compensation/ens160_get_humidity_compensation test */
    ens160_interface_debug_print("ens160: ens160_set_humidity_compensation/ens160_get_humidity_compensation test.\n");
    
    /* set humidity compensation */
    raw = rand() % 0xFFFFU;
    res = ens160_set_humidity_compensation(&gs_handle, raw);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set humidity compensation failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set humidity compensation 0x%04X.\n", raw);
    res = ens160_get_humidity_compensation(&gs_handle, &raw_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get humidity compensation failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check humidity compensation %s.\n", (raw == raw_check) ? "ok" : "error");
    
    /* ens160_set_params/ens160_get_params test */
    ens160_interface_debug_print("ens160: ens160_set_params/ens160_get_params test.\n");
    
    params[0] = rand() % 0xFFU;
    params[1] = rand() % 0xFFU;
    params[2] = rand() % 0xFFU;
    params[3] = rand() % 0xFFU;
    params[4] = rand() % 0xFFU;
    params[5] = rand() % 0xFFU;
    params[6] = rand() % 0xFFU;
    params[7] = rand() % 0xFFU;
    res = ens160_set_params(&gs_handle, params);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: set params failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 0, params[0]);
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 1, params[1]);
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 2, params[2]);
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 3, params[3]);
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 4, params[4]);
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 5, params[5]);
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 6, params[6]);
    ens160_interface_debug_print("ens160: set params[%d] 0x%02X.\n", 7, params[7]);
    res = ens160_get_params(&gs_handle, params_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get params failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 0, (params[0] == params_check[0]) ? "ok" : "error");
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 1, (params[1] == params_check[1]) ? "ok" : "error");
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 2, (params[2] == params_check[2]) ? "ok" : "error");
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 3, (params[3] == params_check[3]) ? "ok" : "error");
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 4, (params[4] == params_check[4]) ? "ok" : "error");
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 5, (params[5] == params_check[5]) ? "ok" : "error");
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 6, (params[6] == params_check[6]) ? "ok" : "error");
    ens160_interface_debug_print("ens160: check params[%d] %s.\n", 7, (params[7] == params_check[7]) ? "ok" : "error");
    
    /* ens160_get_params_output test */
    ens160_interface_debug_print("ens160: ens160_get_params_output test.\n");
    
    /* get params output */
    res = ens160_get_params_output(&gs_handle, params);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get params output failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 0, params[0]);
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 1, params[1]);
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 2, params[2]);
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 3, params[3]);
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 4, params[4]);
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 5, params[5]);
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 6, params[6]);
    ens160_interface_debug_print("ens160: get params[%d] output 0x%02X.\n", 7, params[7]);
    
    /* ens160_get_status test */
    ens160_interface_debug_print("ens160: ens160_get_status test.\n");
    
    /* get status */
    res = ens160_get_status(&gs_handle, &status);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get status failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: status is 0x%02X.\n", status);
    
    /* ens160_get_calculation_temperature test */
    ens160_interface_debug_print("ens160: ens160_get_calculation_temperature test.\n");
    
    /* get calculation temperature */
    res = ens160_get_calculation_temperature(&gs_handle, &raw);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get calculation temperature failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: calculation temperature is 0x%04X.\n", raw);
    
    /* ens160_get_calculation_humidity test */
    ens160_interface_debug_print("ens160: ens160_get_calculation_humidity test.\n");
    
    /* get calculation humidity */
    res = ens160_get_calculation_humidity(&gs_handle, &raw);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get calculation humidity failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: calculation humidity is 0x%04X.\n", raw);
    
    /* ens160_get_checksum test */
    ens160_interface_debug_print("ens160: ens160_get_checksum test.\n");
    
    /* get checksum */
    res = ens160_get_checksum(&gs_handle, &checksum);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: get checksum failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: checksum is 0x%02X.\n", checksum);
    
    /* ens160_nop test */
    ens160_interface_debug_print("ens160: ens160_nop test.\n");
    
    /* nop */
    res = ens160_nop(&gs_handle);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: nop failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check nop %s.\n", (res == 0) ? "ok" : "error");
    
    /* ens160_clear test */
    ens160_interface_debug_print("ens160: ens160_clear test.\n");
    
    /* clear */
    res = ens160_clear(&gs_handle);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: clear failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check clear %s.\n", (res == 0) ? "ok" : "error");
    
    /* ens160_soft_reset test */
    ens160_interface_debug_print("ens160: ens160_soft_reset test.\n");
    
    /* soft reset */
    res = ens160_soft_reset(&gs_handle);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: soft reset failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check soft reset %s.\n", (res == 0) ? "ok" : "error");
    
    /* ens160_temperature_convert_to_register/ens160_temperature_convert_to_data test */
    ens160_interface_debug_print("ens160: ens160_temperature_convert_to_register/ens160_temperature_convert_to_data test.\n");
    
    celsius_deg = (float)(rand() % 1000) / 20.0f;
    res = ens160_temperature_convert_to_register(&gs_handle, celsius_deg, &reg);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: temperature convert to register failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set temperature %0.2fC.\n", celsius_deg);
    res = ens160_temperature_convert_to_data(&gs_handle, reg, &celsius_deg_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: temperature convert to data failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check temperature %0.2fC.\n", celsius_deg_check);
    
    /* ens160_humidity_convert_to_register/ens160_humidity_convert_to_data test */
    ens160_interface_debug_print("ens160: ens160_humidity_convert_to_register/ens160_humidity_convert_to_data test.\n");
    
    percentage = (float)(rand() % 1000) / 10.0f;
    res = ens160_humidity_convert_to_register(&gs_handle, percentage, &reg);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: humidity convert to register failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set humidity %0.2f%%.\n", percentage);
    res = ens160_humidity_convert_to_data(&gs_handle, reg, &percentage_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: humidity convert to data failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check humidity %0.2f%%.\n", percentage_check);
    
    /* ens160_resistance_convert_to_register/ens160_resistance_convert_to_data test */
    ens160_interface_debug_print("ens160: ens160_resistance_convert_to_register/ens160_resistance_convert_to_data test.\n");
    
    ohm = (float)(rand() % 1000) / 10.0f;
    res = ens160_resistance_convert_to_register(&gs_handle, ohm, &reg);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: resistance convert to register failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set resistance %0.2fohm.\n", ohm);
    res = ens160_resistance_convert_to_data(&gs_handle, reg, &ohm_check);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: resistance convert to data failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: check resistance %0.2fohm.\n", ohm_check);
    
    /* ens160_resistance_split test */
    ens160_interface_debug_print("ens160: ens160_resistance_split test.\n");
    
    params[0] = rand() % 0xFFU;
    params[1] = rand() % 0xFFU;
    params[2] = rand() % 0xFFU;
    params[3] = rand() % 0xFFU;
    params[4] = rand() % 0xFFU;
    params[5] = rand() % 0xFFU;
    params[6] = rand() % 0xFFU;
    params[7] = rand() % 0xFFU;
    res = ens160_resistance_split(&gs_handle, params,
                                  &resistance0, &resistance1,
                                  &resistance2, &resistance3);
    if (res != 0)
    {
        ens160_interface_debug_print("ens160: resistance split failed.\n");
        (void)ens160_deinit(&gs_handle);
        
        return 1;
    }
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 0, params[0]);
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 1, params[1]);
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 2, params[2]);
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 3, params[3]);
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 4, params[4]);
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 5, params[5]);
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 6, params[6]);
    ens160_interface_debug_print("ens160: set raw[%d] 0x%02X.\n", 7, params[7]);
    ens160_interface_debug_print("ens160: resistance0 is 0x%04X.\n", resistance0);
    ens160_interface_debug_print("ens160: resistance1 is 0x%04X.\n", resistance1);
    ens160_interface_debug_print("ens160: resistance2 is 0x%04X.\n", resistance2);
    ens160_interface_debug_print("ens160: resistance3 is 0x%04X.\n", resistance3);
    
    /* finish register test */
    ens160_interface_debug_print("ens160: finish register test.\n");
    (void)ens160_deinit(&gs_handle);
    
    return 0;
}
