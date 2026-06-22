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
 * @file      driver_ens160.c
 * @brief     driver ens160 source file
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

#include "driver_ens160.h"
#include <math.h>

/**
 * @brief chip register definition
 */
#define CHIP_NAME                 "ScioSense ENS160"        /**< chip name */
#define MANUFACTURER_NAME         "ScioSense"               /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.71f                     /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        1.98f                     /**< chip max supply voltage */
#define MAX_CURRENT               79.0f                     /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                    /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                     /**< chip max operating temperature */
#define DRIVER_VERSION            1000                      /**< driver version */

/**
 * @brief chip register definition
 */
#define ENS160_REG_PART_ID              0x00        /**< device identity register */
#define ENS160_REG_OPMODE               0x10        /**< operating mode register */
#define ENS160_REG_CONFIG               0x11        /**< interrupt pin configuration register */
#define ENS160_REG_COMMAND              0x12        /**< additional system command register */
#define ENS160_REG_TEMP_IN              0x13        /**< host ambient temperature information register */
#define ENS160_REG_RH_IN                0x15        /**< host relative humidity information register */
#define ENS160_REG_DEVICE_STATUS        0x20        /**< operating mode register */
#define ENS160_REG_DATA_AQI             0x21        /**< air quality index register */
#define ENS160_REG_DATA_TVOC            0x22        /**< tvoc concentration register */
#define ENS160_REG_DATA_ECO2            0x24        /**< equivalent co2 concentration register */
#define ENS160_REG_DATA_ETOH            0x22        /**< ethanol concentration register */
#define ENS160_REG_DATA_T               0x30        /**< temperature used in calculations register */
#define ENS160_REG_DATA_RH              0x32        /**< relative humidity used in calculations register */
#define ENS160_REG_DATA_MISR            0x38        /**< data integrity field register */
#define ENS160_REG_GPR_WRITE            0x40        /**< general purpose write register */
#define ENS160_REG_GPR_READ             0x48        /**< general purpose read register */

/**
 * @brief      iic or spi interface read bytes
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[in]  reg iic register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of data
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
static uint8_t a_ens160_iic_spi_read(ens160_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == ENS160_INTERFACE_IIC)                         /* iic interface */
    {
        if (handle->iic_read(handle->iic_addr, reg, buf, len) != 0)      /* read data */
        {
            return 1;                                                    /* return error */
        }
        
        return 0;                                                        /* success return 0 */
    }
    else                                                                 /* spi interface */
    {
        uint8_t reg_input;

        reg_input = reg << 1;                                            /* left shift */
        reg_input |= 1 << 0;                                             /* flag read */
        if (handle->spi_read(reg_input, buf, len) != 0)                  /* read data */
        {
            return 1;                                                    /* return error */
        }
        
        return 0;                                                        /* success return 0 */
    }
}

/**
 * @brief     iic or spi interface write bytes
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] reg iic register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of data
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_ens160_iic_spi_write(ens160_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == ENS160_INTERFACE_IIC)                          /* iic interface */
    {
        if (handle->iic_write(handle->iic_addr, reg, buf, len) != 0)      /* write data */
        {
            return 1;                                                     /* return error */
        }
        
        return 0;                                                         /* success return 0 */
    }
    else                                                                  /* spi interface */
    {
        uint8_t reg_index;
        uint8_t reg_input;
        uint16_t i;
        
        reg_index = reg;                                                  /* init reg index */
        for (i = 0; i < len; i++)                                         /* write all */
        {
            reg_input = reg_index << 1;                                   /* left shift */
            reg_input &= ~(1 << 0);                                       /* flag write */
            
            if (handle->spi_write(reg_input, &buf[i], 1) != 0)            /* write data */
            {
                return 1;                                                 /* return error */
            }
            reg_index++;                                                  /* index++ */
        }
        
        return 0;                                                         /* success return 0 */
    }
}

/**
 * @brief     set the chip interface
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] interface chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t ens160_set_interface(ens160_handle_t *handle, ens160_interface_t interface) 
{
    if (handle == NULL)                          /* check handle */
    {
        return 2;                                /* return error */
    }
    
    handle->iic_spi = (uint8_t)interface;        /* set interface */
    
    return 0;                                    /* success return 0 */
}

/**
 * @brief      get the chip interface
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *interface pointer to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ens160_get_interface(ens160_handle_t *handle, ens160_interface_t *interface) 
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }
    
    *interface = (ens160_interface_t)(handle->iic_spi);        /* get interface */
    
    return 0;                                                  /* success return 0 */
}

/**
 * @brief     set the iic address pin
 * @param[in] *handle pointer to an ens160 handle structure
 * @param[in] addr_pin address pin
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t ens160_set_addr_pin(ens160_handle_t *handle, ens160_address_t addr_pin)
{
    if (handle == NULL)                        /* check handle */
    {
        return 2;                              /* return error */
    }
    
    handle->iic_addr = (uint8_t)addr_pin;      /* set pin */
    
    return 0;                                  /* success return 0 */
}

/**
 * @brief      get the iic address pin
 * @param[in]  *handle pointer to an ens160 handle structure
 * @param[out] *addr_pin pointer to an address pin buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ens160_get_addr_pin(ens160_handle_t *handle, ens160_address_t *addr_pin)
{
    if (handle == NULL)                                      /* check handle */
    {
        return 2;                                            /* return error */
    }
    
    *addr_pin = (ens160_address_t)(handle->iic_addr);        /* get pin */
    
    return 0;                                                /* success return 0 */
}

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
uint8_t ens160_set_mode(ens160_handle_t *handle, ens160_mode_t mode)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    
    prev = (uint8_t)mode;                                                     /* set mode */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_OPMODE, &prev, 1);        /* write config */
    if (res != 0)                                                             /* check result */
    {
        handle->debug_print("ens160: set mode failed.\n");                    /* set mode failed */
        
        return 1;                                                             /* return error */
    }
    handle->delay_ms(10);                                                     /* delay 10ms */
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ens160_get_mode(ens160_handle_t *handle, ens160_mode_t *mode)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_OPMODE, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get mode failed.\n");                   /* get mode failed */
        
        return 1;                                                            /* return error */
    }
    *mode = (ens160_mode_t)(prev);                                           /* set mode */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_soft_reset(ens160_handle_t *handle)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    
    prev = (uint8_t)0xF0;                                                     /* set reset */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_OPMODE, &prev, 1);        /* write config */
    if (res != 0)                                                             /* check result */
    {
        handle->debug_print("ens160: set mode failed.\n");                    /* set mode failed */
        
        return 1;                                                             /* return error */
    }
    handle->delay_ms(100);                                                    /* delay 100ms */
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ens160_set_interrupt_pin_polarity(ens160_handle_t *handle, ens160_pin_polarity_t polarity)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    prev &= ~(1 << 6);                                                       /* clear settings */
    prev |= polarity << 6;                                                   /* set polarity */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_CONFIG, &prev, 1);       /* write config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: set config failed.\n");                 /* set config failed */
        
        return 1;                                                            /* return error */
    }
    handle->delay_ms(10);                                                    /* delay 10ms */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_get_interrupt_pin_polarity(ens160_handle_t *handle, ens160_pin_polarity_t *polarity)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    *polarity = (ens160_pin_polarity_t)((prev >> 6) & 0x01);                 /* set polarity */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_set_interrupt_pin_drive(ens160_handle_t *handle, ens160_pin_drive_t drive)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    prev &= ~(1 << 5);                                                       /* clear settings */
    prev |= drive << 5;                                                      /* set drive */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_CONFIG, &prev, 1);       /* write config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: set config failed.\n");                 /* set config failed */
        
        return 1;                                                            /* return error */
    }
    handle->delay_ms(10);                                                    /* delay 10ms */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_get_interrupt_pin_drive(ens160_handle_t *handle, ens160_pin_drive_t *drive)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    *drive = (ens160_pin_drive_t)((prev >> 5) & 0x01);                       /* set drive */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_set_general_purpose_read_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    prev &= ~(1 << 3);                                                       /* clear settings */
    prev |= enable << 3;                                                     /* set bool */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_CONFIG, &prev, 1);       /* write config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: set config failed.\n");                 /* set config failed */
        
        return 1;                                                            /* return error */
    }
    handle->delay_ms(10);                                                    /* delay 10ms */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_get_general_purpose_read_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t *enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    *enable = (ens160_bool_t)((prev >> 3) & 0x01);                           /* set bool */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_set_data_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    prev &= ~(1 << 1);                                                       /* clear settings */
    prev |= enable << 1;                                                     /* set bool */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_CONFIG, &prev, 1);       /* write config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: set config failed.\n");                 /* set config failed */
        
        return 1;                                                            /* return error */
    }
    handle->delay_ms(10);                                                    /* delay 10ms */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_get_data_interrupt_pin_asserted(ens160_handle_t *handle, ens160_bool_t *enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    *enable = (ens160_bool_t)((prev >> 1) & 0x01);                           /* set bool */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_set_interrupt(ens160_handle_t *handle, ens160_bool_t enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    prev &= ~(1 << 0);                                                       /* clear settings */
    prev |= enable << 0;                                                     /* set bool */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_CONFIG, &prev, 1);       /* write config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: set config failed.\n");                 /* set config failed */
        
        return 1;                                                            /* return error */
    }
    handle->delay_ms(10);                                                    /* delay 10ms */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_get_interrupt(ens160_handle_t *handle, ens160_bool_t *enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_CONFIG, &prev, 1);        /* read config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: get config failed.\n");                 /* get config failed */
        
        return 1;                                                            /* return error */
    }
    *enable = (ens160_bool_t)((prev >> 0) & 0x01);                           /* set bool */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_nop(ens160_handle_t *handle)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    
    prev = 0x00;                                                               /* set nop command */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_COMMAND, &prev, 1);        /* write config */
    if (res != 0)                                                              /* check result */
    {
        handle->debug_print("ens160: set command failed.\n");                  /* set command failed */
        
        return 1;                                                              /* return error */
    }
    handle->delay_ms(10);                                                      /* delay 10ms */
    
    return 0;                                                                  /* success return 0 */
}

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
uint8_t ens160_get_app_version(ens160_handle_t *handle, uint8_t *major, uint8_t *minor, uint8_t *release)
{
    uint8_t res;
    uint8_t prev;
    uint8_t buf[3];
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    
    prev = 0x00;                                                               /* set nop command */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_COMMAND, &prev, 1);        /* write config */
    if (res != 0)                                                              /* check result */
    {
        handle->debug_print("ens160: set command failed.\n");                  /* set command failed */
        
        return 1;                                                              /* return error */
    }
    handle->delay_ms(10);                                                      /* delay 10ms */
    prev = 0x0E;                                                               /* set get app version command */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_COMMAND, &prev, 1);        /* write config */
    if (res != 0)                                                              /* check result */
    {
        handle->debug_print("ens160: set command failed.\n");                  /* set command failed */
        
        return 1;                                                              /* return error */
    }
    handle->delay_ms(10);                                                      /* delay 10ms */
    res = a_ens160_iic_spi_read(handle, ENS160_REG_GPR_READ + 4, buf, 3);      /* read data */
    if (res != 0)                                                              /* check result */
    {
        handle->debug_print("ens160: read data failed.\n");                    /* read data failed */
        
        return 1;                                                              /* return error */
    }
    *major = buf[0];                                                           /* set major */
    *minor = buf[1];                                                           /* set minor */
    *release = buf[2];                                                         /* set release */
    
    return 0;                                                                  /* success return 0 */
}

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
uint8_t ens160_clear(ens160_handle_t *handle)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    
    prev = 0x00;                                                               /* set nop command */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_COMMAND, &prev, 1);        /* write config */
    if (res != 0)                                                              /* check result */
    {
        handle->debug_print("ens160: set command failed.\n");                  /* set command failed */
        
        return 1;                                                              /* return error */
    }
    handle->delay_ms(10);                                                      /* delay 10ms */
    prev = 0xCC;                                                               /* set clear command */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_COMMAND, &prev, 1);        /* write config */
    if (res != 0)                                                              /* check result */
    {
        handle->debug_print("ens160: set command failed.\n");                  /* set command failed */
        
        return 1;                                                              /* return error */
    }
    handle->delay_ms(10);                                                      /* delay 10ms */
    
    return 0;                                                                  /* success return 0 */
}

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
uint8_t ens160_set_temperature_compensation(ens160_handle_t *handle, uint16_t raw)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    buf[0] = (raw >> 0) & 0xFF;                                              /* set lsb */
    buf[1] = (raw >> 8) & 0xFF;                                              /* set msb */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_TEMP_IN, buf, 2);        /* write config */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("ens160: set temp in failed.\n");                /* set temp in failed */
        
        return 1;                                                            /* return error */
    }
    handle->delay_ms(10);                                                    /* delay 10ms */
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t ens160_get_temperature_compensation(ens160_handle_t *handle, uint16_t *raw)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                     /* check handle */
    {
        return 2;                                                           /* return error */
    }
    if (handle->inited != 1)                                                /* check handle initialization */
    {
        return 3;                                                           /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_TEMP_IN, buf, 2);        /* read config */
    if (res != 0)                                                           /* check result */
    {
        handle->debug_print("ens160: get temp in failed.\n");               /* get temp in failed */
        
        return 1;                                                           /* return error */
    }
    *raw = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                      /* set raw */
    
    return 0;                                                               /* success return 0 */
}

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
uint8_t ens160_set_humidity_compensation(ens160_handle_t *handle, uint16_t raw)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                    /* check handle */
    {
        return 2;                                                          /* return error */
    }
    if (handle->inited != 1)                                               /* check handle initialization */
    {
        return 3;                                                          /* return error */
    }
    
    buf[0] = (raw >> 0) & 0xFF;                                            /* set lsb */
    buf[1] = (raw >> 8) & 0xFF;                                            /* set msb */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_RH_IN, buf, 2);        /* write config */
    if (res != 0)                                                          /* check result */
    {
        handle->debug_print("ens160: set rh in failed.\n");                /* set rh in failed */
        
        return 1;                                                          /* return error */
    }
    handle->delay_ms(10);                                                  /* delay 10ms */
    
    return 0;                                                              /* success return 0 */
}

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
uint8_t ens160_get_humidity_compensation(ens160_handle_t *handle, uint16_t *raw)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                   /* check handle */
    {
        return 2;                                                         /* return error */
    }
    if (handle->inited != 1)                                              /* check handle initialization */
    {
        return 3;                                                         /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_RH_IN, buf, 2);        /* read config */
    if (res != 0)                                                         /* check result */
    {
        handle->debug_print("ens160: get rh in failed.\n");               /* get rh in failed */
        
        return 1;                                                         /* return error */
    }
    *raw = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                    /* set raw */
    
    return 0;                                                             /* success return 0 */
}

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
uint8_t ens160_get_status(ens160_handle_t *handle, uint8_t *status)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->inited != 1)                                                        /* check handle initialization */
    {
        return 3;                                                                   /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DEVICE_STATUS, &prev, 1);        /* read status */
    if (res != 0)                                                                   /* check result */
    {
        handle->debug_print("ens160: get status failed.\n");                        /* get status failed */
        
        return 1;                                                                   /* return error */
    }
    *status = prev;                                                                 /* set status */
    
    return 0;                                                                       /* success return 0 */
}

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
uint8_t ens160_read_aqi(ens160_handle_t *handle, uint8_t *uba)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_AQI, &prev, 1);        /* read aqi */
    if (res != 0)                                                              /* check result */
    {
        handle->debug_print("ens160: read aqi failed.\n");                     /* read aqi failed */
        
        return 1;                                                              /* return error */
    }
    *uba = prev & 0x7;                                                         /* set uba */
    
    return 0;                                                                  /* success return 0 */
}

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
uint8_t ens160_read_tvoc(ens160_handle_t *handle, uint16_t *tvoc_ppb)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_TVOC, buf, 2);        /* read tvoc */
    if (res != 0)                                                             /* check result */
    {
        handle->debug_print("ens160: read tvoc failed.\n");                   /* read tvoc failed */
        
        return 1;                                                             /* return error */
    }
    *tvoc_ppb = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                   /* set tvoc ppb */
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ens160_read_eco2(ens160_handle_t *handle, uint16_t *eco2_ppm)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_ECO2, buf, 2);        /* read eco2 */
    if (res != 0)                                                             /* check result */
    {
        handle->debug_print("ens160: read eco2 failed.\n");                   /* read eco2 failed */
        
        return 1;                                                             /* return error */
    }
    *eco2_ppm = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                   /* set eco2 ppm */
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ens160_read_etoh(ens160_handle_t *handle, uint16_t *etoh_ppb)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_ETOH, buf, 2);        /* read etoh */
    if (res != 0)                                                             /* check result */
    {
        handle->debug_print("ens160: read etoh failed.\n");                   /* read etoh failed */
        
        return 1;                                                             /* return error */
    }
    *etoh_ppb = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                   /* set etoh ppb */
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ens160_get_calculation_temperature(ens160_handle_t *handle, uint16_t *raw)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                    /* check handle */
    {
        return 2;                                                          /* return error */
    }
    if (handle->inited != 1)                                               /* check handle initialization */
    {
        return 3;                                                          /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_T, buf, 2);        /* read data */
    if (res != 0)                                                          /* check result */
    {
        handle->debug_print("ens160: get data failed.\n");                 /* get data failed */
        
        return 1;                                                          /* return error */
    }
    *raw = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                     /* set raw */
    
    return 0;                                                              /* success return 0 */
}

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
uint8_t ens160_get_calculation_humidity(ens160_handle_t *handle, uint16_t *raw)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                    /* check handle */
    {
        return 2;                                                          /* return error */
    }
    if (handle->inited != 1)                                               /* check handle initialization */
    {
        return 3;                                                          /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_RH, buf, 2);       /* read data */
    if (res != 0)                                                          /* check result */
    {
        handle->debug_print("ens160: get data failed.\n");                 /* get data failed */
        
        return 1;                                                          /* return error */
    }
    *raw = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                     /* set raw */
    
    return 0;                                                              /* success return 0 */
}

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
uint8_t ens160_get_checksum(ens160_handle_t *handle, uint8_t *checksum)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->inited != 1)                                                    /* check handle initialization */
    {
        return 3;                                                               /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_MISR, &prev, 1);        /* get checksum */
    if (res != 0)                                                               /* check result */
    {
        handle->debug_print("ens160: get checksum failed.\n");                  /* get checksum failed */
        
        return 1;                                                               /* return error */
    }
    *checksum = prev;                                                           /* set checksum */
    
    return 0;                                                                   /* success return 0 */
}

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
uint8_t ens160_set_params(ens160_handle_t *handle, uint8_t params[8])
{
    uint8_t res;
    
    if (handle == NULL)                                                           /* check handle */
    {
        return 2;                                                                 /* return error */
    }
    if (handle->inited != 1)                                                      /* check handle initialization */
    {
        return 3;                                                                 /* return error */
    }
    
    res = a_ens160_iic_spi_write(handle, ENS160_REG_GPR_WRITE, params, 8);        /* set params */
    if (res != 0)                                                                 /* check result */
    {
        handle->debug_print("ens160: set params failed.\n");                      /* set params failed */
        
        return 1;                                                                 /* return error */
    }
    
    return 0;                                                                     /* success return 0 */
}

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
uint8_t ens160_get_params(ens160_handle_t *handle, uint8_t params[8])
{
    uint8_t res;
    
    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_GPR_WRITE, params, 8);        /* get params */
    if (res != 0)                                                                /* check result */
    {
        handle->debug_print("ens160: get params failed.\n");                     /* get params failed */
        
        return 1;                                                                /* return error */
    }
    
    return 0;                                                                    /* success return 0 */
}

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
uint8_t ens160_get_params_output(ens160_handle_t *handle, uint8_t params[8])
{
    uint8_t res;
    
    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->inited != 1)                                                    /* check handle initialization */
    {
        return 3;                                                               /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_GPR_READ, params, 8);        /* get params */
    if (res != 0)                                                               /* check result */
    {
        handle->debug_print("ens160: get params output failed.\n");             /* get params output failed */
        
        return 1;                                                               /* return error */
    }
    
    return 0;                                                                   /* success return 0 */
}

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
uint8_t ens160_temperature_convert_to_register(ens160_handle_t *handle, float celsius_deg, uint16_t *reg)
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }
    if (handle->inited != 1)                                   /* check handle initialization */
    {
        return 3;                                              /* return error */
    }
    
    *reg = (uint16_t)((celsius_deg + 273.15f) * 64.0f);        /* convert real data to register data */
    
    return 0;                                                  /* success return 0 */
}

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
uint8_t ens160_temperature_convert_to_data(ens160_handle_t *handle, uint16_t reg, float *celsius_deg)
{
    if (handle == NULL)                                       /* check handle */
    {
        return 2;                                             /* return error */
    }
    if (handle->inited != 1)                                  /* check handle initialization */
    {
        return 3;                                             /* return error */
    }
    
    *celsius_deg = (((float)(reg) / 64.0f) - 273.15f);        /* convert raw data to real data */
    
    return 0;                                                 /* success return 0 */
}

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
uint8_t ens160_humidity_convert_to_register(ens160_handle_t *handle, float percentage, uint16_t *reg)
{
    if (handle == NULL)                            /* check handle */
    {
        return 2;                                  /* return error */
    }
    if (handle->inited != 1)                       /* check handle initialization */
    {
        return 3;                                  /* return error */
    }
    
    *reg = (uint16_t)(percentage * 512.0f);        /* convert real data to register data */
    
    return 0;                                      /* success return 0 */
}

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
uint8_t ens160_humidity_convert_to_data(ens160_handle_t *handle, uint16_t reg, float *percentage)
{
    if (handle == NULL)                           /* check handle */
    {
        return 2;                                 /* return error */
    }
    if (handle->inited != 1)                      /* check handle initialization */
    {
        return 3;                                 /* return error */
    }
    
    *percentage = ((float)(reg) / 512.0f);        /* convert raw data to real data */
    
    return 0;                                     /* success return 0 */
}

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
uint8_t ens160_resistance_convert_to_register(ens160_handle_t *handle, float ohm, uint16_t *reg)
{
    if (handle == NULL)                            /* check handle */
    {
        return 2;                                  /* return error */
    }
    if (handle->inited != 1)                       /* check handle initialization */
    {
        return 3;                                  /* return error */
    }
    
    *reg = (uint16_t)(log2f(ohm) * 2048.0f);       /* convert real data to register data */
    
    return 0;                                      /* success return 0 */
}

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
uint8_t ens160_resistance_convert_to_data(ens160_handle_t *handle, uint16_t reg, float *ohm)
{
    if (handle == NULL)                             /* check handle */
    {
        return 2;                                   /* return error */
    }
    if (handle->inited != 1)                        /* check handle initialization */
    {
        return 3;                                   /* return error */
    }
    
    *ohm = powf(2.0f, (float)reg / 2048.0f);        /* convert raw data to real data */
    
    return 0;                                       /* success return 0 */
}

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
                                uint16_t *resistance2, uint16_t *resistance3)
{
    if (handle == NULL)                                               /* check handle */
    {
        return 2;                                                     /* return error */
    }
    if (handle->inited != 1)                                          /* check handle initialization */
    {
        return 3;                                                     /* return error */
    }
    
    *resistance0 = (uint16_t)((uint16_t)raw[1] << 8) | raw[0];        /* set resistance0 */
    *resistance1 = (uint16_t)((uint16_t)raw[3] << 8) | raw[2];        /* set resistance1 */
    *resistance2 = (uint16_t)((uint16_t)raw[5] << 8) | raw[4];        /* set resistance2 */
    *resistance3 = (uint16_t)((uint16_t)raw[7] << 8) | raw[6];        /* set resistance3 */
    
    return 0;                                                         /* success return 0 */
}

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
uint8_t ens160_irq_handler(ens160_handle_t *handle)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->inited != 1)                                                        /* check handle initialization */
    {
        return 3;                                                                   /* return error */
    }
    
    res = a_ens160_iic_spi_read(handle, ENS160_REG_DEVICE_STATUS, &prev, 1);        /* read status */
    if (res != 0)                                                                   /* check result */
    {
        handle->debug_print("ens160: get status failed.\n");                        /* get status failed */
        
        return 1;                                                                   /* return error */
    }
    
    if ((prev & (uint8_t)ENS160_STATUS_NEWDAT) != 0)                                /* check new data */
    {
        uint8_t prev2;
        uint8_t buf[2];
        uint16_t raw[4];
        
        res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_AQI, &prev2, 1);        /* read aqi */
        if (res != 0)                                                               /* check result */
        {
            handle->debug_print("ens160: read aqi failed.\n");                      /* read aqi failed */
            
            return 1;                                                               /* return error */
        }
        raw[0] = prev2 & 0x7;                                                       /* set raw */
        
        res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_TVOC, buf, 2);          /* read tvoc */
        if (res != 0)                                                               /* check result */
        {
            handle->debug_print("ens160: read tvoc failed.\n");                     /* read tvoc failed */
            
            return 1;                                                               /* return error */
        }
        raw[1] = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                        /* set tvoc ppb */
        raw[3] = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                        /* set etoh ppb */
        
        res = a_ens160_iic_spi_read(handle, ENS160_REG_DATA_ECO2, buf, 2);          /* read eco2 */
        if (res != 0)                                                               /* check result */
        {
            handle->debug_print("ens160: read eco2 failed.\n");                     /* read eco2 failed */
            
            return 1;                                                               /* return error */
        }
        raw[2] = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                        /* set eco2 ppm */
    
        if (handle->receive_callback != NULL)                                       /* not null */
        {
            handle->receive_callback((uint8_t)ENS160_STATUS_NEWDAT, raw);           /* run the callback */
        }
    }
    if ((prev & (uint8_t)ENS160_STATUS_NEWGPR) != 0)                                /* check new gpr */
    {
        uint8_t params[8];
        uint16_t raw[4];
        
        res = a_ens160_iic_spi_read(handle, ENS160_REG_GPR_READ, params, 8);        /* get params */
        if (res != 0)                                                               /* check result */
        {
            handle->debug_print("ens160: get params output failed.\n");             /* get params output failed */
            
            return 1;                                                               /* return error */
        }
        raw[0] = (uint16_t)((uint16_t)params[1] << 8) | params[0];                  /* set resistance0 */
        raw[1] = (uint16_t)((uint16_t)params[3] << 8) | params[2];                  /* set resistance1 */
        raw[2] = (uint16_t)((uint16_t)params[5] << 8) | params[4];                  /* set resistance2 */
        raw[3] = (uint16_t)((uint16_t)params[7] << 8) | params[6];                  /* set resistance3 */
        
        if (handle->receive_callback != NULL)                                       /* not null */
        {
            handle->receive_callback((uint8_t)ENS160_STATUS_NEWGPR, raw);           /* run the callback */
        }
    }
    
    return 0;                                                                       /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle pointer to an ens160 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 close failed
 * @note      none
 */
static uint8_t a_ens160_close(ens160_handle_t *handle)
{
    if (handle->iic_spi == ENS160_INTERFACE_IIC)                        /* iic interface */
    {
        if (handle->iic_deinit() != 0)                                  /* iic deinit */
        {
            handle->debug_print("ens160: iic deinit failed.\n");        /* iic deinit failed */
       
            return 1;                                                   /* return error */
        }
        
        return 0;                                                       /* success return 0 */
    }
    else                                                                /* spi interface */
    {
        if (handle->spi_deinit() != 0)                                  /* spi deinit */
        {
            handle->debug_print("ens160: spi deinit failed.\n");        /* spi deinit failed */
       
            return 1;                                                   /* return error */
        }
        
        return 0;                                                       /* success return 0 */
    }
}

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
uint8_t ens160_init(ens160_handle_t *handle)
{
    uint8_t buf[2];
    uint16_t id;
  
    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->debug_print == NULL)                                            /* check debug_print */
    {
        return 3;                                                               /* return error */
    }
    if (handle->iic_init == NULL)                                               /* check iic_init */
    {
        handle->debug_print("ens160: iic_init is null.\n");                     /* iic_init is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->iic_deinit == NULL)                                             /* check iic_deinit */
    {
        handle->debug_print("ens160: iic_deinit is null.\n");                   /* iic_deinit is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->iic_read == NULL)                                               /* check iic_read */
    {
        handle->debug_print("ens160: iic_read is null.\n");                     /* iic_read is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->iic_write == NULL)                                              /* check iic_write */
    {
        handle->debug_print("ens160: iic_write is null.\n");                    /* iic_write is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->spi_init == NULL)                                               /* check spi_init */
    {
        handle->debug_print("ens160: spi_init is null.\n");                     /* spi_init is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->spi_deinit == NULL)                                             /* check spi_deinit */
    {
        handle->debug_print("ens160: spi_deinit is null.\n");                   /* spi_deinit is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->spi_read == NULL)                                               /* check spi_read */
    {
        handle->debug_print("ens160: spi_read is null.\n");                     /* spi_read is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->spi_write == NULL)                                              /* check spi_write */
    {
        handle->debug_print("ens160: spi_write is null.\n");                    /* spi_write is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->delay_ms == NULL)                                               /* check delay_ms */
    {
        handle->debug_print("ens160: delay_ms is null.\n");                     /* delay_ms is null */
       
        return 3;                                                               /* return error */
    }
    if (handle->receive_callback == NULL)                                       /* check receive_callback */
    {
        handle->debug_print("ens160: receive_callback is null.\n");             /* receive_callback is null */
       
        return 3;                                                               /* return error */
    }
    
    if (handle->iic_spi == ENS160_INTERFACE_IIC)                                /* iic interface */
    {
        if (handle->iic_init() != 0)                                            /* initialize iic bus */
        {
            handle->debug_print("ens160: iic init failed.\n");                  /* iic init failed */
            
            return 1;                                                           /* return error */
        }
    }
    else                                                                        /* spi interface */
    {
        if (handle->spi_init() != 0)                                            /* initialize spi bus */
        {
            handle->debug_print("ens160: spi init failed.\n");                  /* spi init failed */
           
            return 1;                                                           /* return error */
        }
    }
    
    if (a_ens160_iic_spi_read(handle, ENS160_REG_PART_ID , buf, 2) != 0)        /* read id */
    {
        handle->debug_print("ens160: read failed.\n");                          /* read failed */
        (void)a_ens160_close(handle);                                           /* close */
        
        return 4;                                                               /* return error */
    }
    id = (uint16_t)((uint16_t)buf[1] << 8) | buf[0];                            /* set id */
    if (id != 0x0160)                                                           /* check id */
    {
        handle->debug_print("ens160: id is invalid.\n");                        /* id is invalid */
        (void)a_ens160_close(handle);                                           /* close */
       
        return 4;                                                               /* return error */
    }
    handle->inited = 1;                                                         /* flag finish initialization */
    
    return 0;                                                                   /* success return 0 */
}

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
uint8_t ens160_deinit(ens160_handle_t *handle)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    
    prev = 0x00;                                                              /* set mode */
    res = a_ens160_iic_spi_write(handle, ENS160_REG_OPMODE, &prev, 1);        /* write config */
    if (res != 0)                                                             /* check result */
    {
        handle->debug_print("ens160: set mode failed.\n");                    /* set mode failed */
        
        return 4;                                                             /* return error */
    }
    res = a_ens160_close(handle);                                             /* close */
    if (res != 0)                                                             /* check result */
    {
        return 1;                                                             /* return error */
    }
    handle->inited = 0;                                                       /* flag close */
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ens160_set_reg(ens160_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                          /* check handle */
    {
        return 2;                                                /* return error */
    }
    if (handle->inited != 1)                                     /* check handle initialization */
    {
        return 3;                                                /* return error */
    }
    
    return a_ens160_iic_spi_write(handle, reg, buf, len);        /* write data */
}

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
uint8_t ens160_get_reg(ens160_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                         /* check handle */
    {
        return 2;                                               /* return error */
    }
    if (handle->inited != 1)                                    /* check handle initialization */
    {
        return 3;                                               /* return error */
    }
    
    return a_ens160_iic_spi_read(handle, reg, buf, len);        /* read data */
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to an ens160 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ens160_info(ens160_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(ens160_info_t));                         /* initialize ens160 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "IIC SPI", 8);                         /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
