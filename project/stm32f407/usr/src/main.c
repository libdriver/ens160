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
 * @file      main.c
 * @brief     main source file
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
#include "driver_ens160_read_test.h"
#include "driver_ens160_interrupt_test.h"
#include "driver_ens160_basic.h"
#include "driver_ens160_interrupt.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"
#include "getopt.h"
#include <stdlib.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                        /**< uart buffer */
volatile uint16_t g_len;                   /**< uart buffer length */
static volatile uint8_t gs_flag;           /**< local flag */
uint8_t (*g_gpio_irq)(void) = NULL;        /**< gpio irq */

/**
 * @brief exti 0 irq
 * @note  none
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief     gpio exti callback
 * @param[in] pin gpio pin
 * @note      none
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0)
    {
        /* run the callback */
        if (g_gpio_irq != NULL)
        {
            g_gpio_irq();
        }
    }
}

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
 * @brief     ens160 full function
 * @param[in] argc arg numbers
 * @param[in] **argv arg address
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 5 param is invalid
 * @note      none
 */
uint8_t ens160(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    const char short_options[] = "hipe:t:";
    const struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"addr", required_argument, NULL, 1},
        {"interface", required_argument, NULL, 2},
        {"times", required_argument, NULL, 3},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";
    uint32_t times = 3;
    ens160_address_t addr = ENS160_ADDRESS_0;
    ens160_interface_t interface = ENS160_INTERFACE_IIC;
    
    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }
    
    /* init 0 */
    optind = 0;
    
    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);
        
        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");
                
                break;
            }
            
            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");
                
                break;
            }
            
            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");
                
                break;
            }
            
            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);
                
                break;
            }
            
            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);
                
                break;
            }
            
            /* addr */
            case 1 :
            {
                /* set the addr pin */
                if (strcmp("0", optarg) == 0)
                {
                    addr = ENS160_ADDRESS_0;
                }
                else if (strcmp("1", optarg) == 0)
                {
                    addr = ENS160_ADDRESS_1;
                }
                else
                {
                    return 5;
                }
                
                break;
            }
            
            /* interface */
            case 2 :
            {
                /* set the interface */
                if (strcmp("iic", optarg) == 0)
                {
                    interface = ENS160_INTERFACE_IIC;
                }
                else if (strcmp("spi", optarg) == 0)
                {
                    interface = ENS160_INTERFACE_SPI;
                }
                else
                {
                    return 5;
                }
                
                break;
            }
            
            /* running times */
            case 3 :
            {
                /* set the times */
                times = atol(optarg);
                
                break;
            } 
            
            /* the end */
            case -1 :
            {
                break;
            }
            
            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* run the function */
    if (strcmp("t_reg", type) == 0)
    {
        uint8_t res;
        
        /* run register test */
        res = ens160_register_test(interface, addr);
        if (res != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_read", type) == 0)
    {
        uint8_t res;
        
        /* read test */
        res = ens160_read_test(interface, addr, times);
        if (res != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_int", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* set the gpio irq */
        g_gpio_irq = ens160_interrupt_test_irq_handler;
        
        /* run interrupt test */
        res = ens160_interrupt_test(interface, addr, times);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_basic", type) == 0)
    {
        uint8_t res;
        uint32_t i;
        uint8_t aqi_uba;
        uint16_t tvoc_ppb;
        uint16_t eco2_ppm;
        uint16_t etoh_ppb;
        
        /* basic init */
        res = ens160_basic_init(interface, addr);
        if (res != 0)
        {
            return 1;
        }
        
        /* loop */
        for (i = 0; i < times; i++)
        {
            /* delay 1000ms */
            ens160_interface_delay_ms(1000);
            
            /* read data */
            res = ens160_basic_read(&aqi_uba, &tvoc_ppb, &eco2_ppm, &etoh_ppb);
            if (res != 0)
            {
                (void)ens160_basic_deinit();
                
                return 1;
            }
            
            /* output */
            ens160_interface_debug_print("ens160: aqi is %d.\n", aqi_uba);
            ens160_interface_debug_print("ens160: tvoc is %d ppb.\n", tvoc_ppb);
            ens160_interface_debug_print("ens160: eco2 is %d ppm.\n", eco2_ppm);
            ens160_interface_debug_print("ens160: etoh is %d ppb.\n", etoh_ppb);
        }
        
        /* basic deinit */
        (void)ens160_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_raw", type) == 0)
    {
        uint8_t res;
        uint32_t i;
        float resistance_ohm[4];
        
        /* basic init */
        res = ens160_basic_init(interface, addr);
        if (res != 0)
        {
            return 1;
        }
        
        /* loop */
        for (i = 0; i < times; i++)
        {
            /* delay 1000ms */
            ens160_interface_delay_ms(1000);
            
            /* read data */
            res = ens160_basic_read_raw(resistance_ohm);
            if (res != 0)
            {
                (void)ens160_basic_deinit();
                
                return 1;
            }
            
            /* output */
            ens160_interface_debug_print("ens160: resistance0 is %0.2f ohm.\n", resistance_ohm[0]);
            ens160_interface_debug_print("ens160: resistance1 is %0.2f ohm.\n", resistance_ohm[1]);
            ens160_interface_debug_print("ens160: resistance2 is %0.2f ohm.\n", resistance_ohm[2]);
            ens160_interface_debug_print("ens160: resistance3 is %0.2f ohm.\n", resistance_ohm[3]);
        }
        
        /* basic deinit */
        (void)ens160_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_int", type) == 0)
    {
        uint8_t res;
        uint32_t i;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* set the interrupt irq */
        g_gpio_irq = ens160_interrupt_irq_handler;
        
        /* interrupt init */
        res = ens160_interrupt_init(interface, addr, a_callback);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
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
        
        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        
        /* interrupt deinit */
        (void)ens160_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        ens160_interface_debug_print("Usage:\n");
        ens160_interface_debug_print("  ens160 (-i | --information)\n");
        ens160_interface_debug_print("  ens160 (-h | --help)\n");
        ens160_interface_debug_print("  ens160 (-p | --port)\n");
        ens160_interface_debug_print("  ens160 (-t reg | --test=reg) [--addr=<0 | 1>] [--interface=<iic | spi>]\n");
        ens160_interface_debug_print("  ens160 (-t read | --test=read) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]\n");
        ens160_interface_debug_print("  ens160 (-t int | --test=int) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]\n");
        ens160_interface_debug_print("  ens160 (-e basic | --example=basic) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]\n");
        ens160_interface_debug_print("  ens160 (-e raw | --example=raw) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]\n");
        ens160_interface_debug_print("  ens160 (-e int | --example=int) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]\n");
        ens160_interface_debug_print("\n");
        ens160_interface_debug_print("Options:\n");
        ens160_interface_debug_print("      --addr=<0 | 1>                 Set the chip address.([default: 0])\n");
        ens160_interface_debug_print("  -e <basic | raw | int>, --example=<basic | raw | int>\n");
        ens160_interface_debug_print("                                     Run the driver example.\n");
        ens160_interface_debug_print("  -h, --help                         Show the help.\n");
        ens160_interface_debug_print("  -i, --information                  Show the chip information.\n");
        ens160_interface_debug_print("      --interface=<iic | spi>        Set the chip interface.([default: iic])\n");
        ens160_interface_debug_print("  -p, --port                         Display the pin connections of the current board.\n");
        ens160_interface_debug_print("  -t <reg | read | int>, --test=<reg | read | int>\n");
        ens160_interface_debug_print("                                     Run the driver test.\n");
        ens160_interface_debug_print("      --times=<num>                  Set the running times.([default: 3])\n");
        
        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        ens160_info_t info;
        
        /* print ens160 info */
        ens160_info(&info);
        ens160_interface_debug_print("ens160: chip is %s.\n", info.chip_name);
        ens160_interface_debug_print("ens160: manufacturer is %s.\n", info.manufacturer_name);
        ens160_interface_debug_print("ens160: interface is %s.\n", info.interface);
        ens160_interface_debug_print("ens160: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ens160_interface_debug_print("ens160: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        ens160_interface_debug_print("ens160: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        ens160_interface_debug_print("ens160: max current is %0.2fmA.\n", info.max_current_ma);
        ens160_interface_debug_print("ens160: max temperature is %0.1fC.\n", info.temperature_max);
        ens160_interface_debug_print("ens160: min temperature is %0.1fC.\n", info.temperature_min);
        
        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* print pin connection */
        ens160_interface_debug_print("ens160: SPI interface SCK connected to GPIOA PIN5.\n");
        ens160_interface_debug_print("ens160: SPI interface MISO connected to GPIOA PIN6.\n");
        ens160_interface_debug_print("ens160: SPI interface MOSI connected to GPIOA PIN7.\n");
        ens160_interface_debug_print("ens160: SPI interface CS connected to GPIOA PIN4.\n");
        ens160_interface_debug_print("ens160: IIC interface SCL connected to GPIOB PIN8.\n");
        ens160_interface_debug_print("ens160: IIC interface SDA connected to GPIOB PIN9.\n");
        ens160_interface_debug_print("ens160: INT connected to GPIOB PIN0.\n");
        
        return 0;
    }
    else
    {
        return 5;
    }
}

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    uint8_t res;
    
    /* stm32f407 clock init and hal init */
    clock_init();
    
    /* delay init */
    delay_init();
    
    /* uart init */
    uart_init(115200);
    
    /* shell init && register ens160 function */
    shell_init();
    shell_register("ens160", ens160);
    uart_print("ens160: welcome to libdriver ens160.\n");
    
    while (1)
    {
        /* read uart */
        g_len = uart_read(g_buf, 256);
        if (g_len != 0)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart_print("ens160: run failed.\n");
            }
            else if (res == 2)
            {
                uart_print("ens160: unknown command.\n");
            }
            else if (res == 3)
            {
                uart_print("ens160: length is too long.\n");
            }
            else if (res == 4)
            {
                uart_print("ens160: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart_print("ens160: param is invalid.\n");
            }
            else
            {
                uart_print("ens160: unknown status code.\n");
            }
            uart_flush();
        }
        delay_ms(100);
    }
}
