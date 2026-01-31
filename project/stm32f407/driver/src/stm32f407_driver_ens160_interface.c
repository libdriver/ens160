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
 * @file      stm32f407_driver_ens160_interface.c
 * @brief     stm32f407 driver ens160 interface source file
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

#include "driver_ens160_interface.h"
#include "delay.h"
#include "iic.h"
#include "spi.h"
#include "uart.h"
#include <stdarg.h>

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t ens160_interface_iic_init(void)
{
    return iic_init();
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t ens160_interface_iic_deinit(void)
{
    return iic_deinit();
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr iic device write address
 * @param[in]  reg iic register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ens160_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    return iic_read(addr, reg, buf, len);
}

/**
 * @brief     interface iic bus write
 * @param[in] addr iic device write address
 * @param[in] reg iic register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ens160_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    return iic_write(addr, reg, buf, len);
}

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t ens160_interface_spi_init(void)
{
    return spi_init(SPI_MODE_3);
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t ens160_interface_spi_deinit(void)
{
    return spi_deinit();
}

/**
 * @brief      interface spi bus read
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ens160_interface_spi_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
    return spi_read(reg, buf, len);
}

/**
 * @brief     interface spi bus write
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ens160_interface_spi_write(uint8_t reg, uint8_t *buf, uint16_t len)
{
    return spi_write(reg, buf, len);
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void ens160_interface_delay_ms(uint32_t ms)
{
    delay_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void ens160_interface_debug_print(const char *const fmt, ...)
{
    char str[256];
    uint16_t len;
    va_list args;

    memset((char *)str, 0, sizeof(char) * 256);
    va_start(args, fmt);
    vsnprintf((char *)str, 255, (char const *)fmt, args);
    va_end(args);

    len = strlen((char *)str);
    (void)uart_write((uint8_t *)str, len);
}

/**
 * @brief     interface receive callback
 * @param[in] type irq type
 * @param[in] *dat pointer to a data buffer
 * @note      none
 */
void ens160_interface_receive_callback(uint8_t type, uint16_t *dat)
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
