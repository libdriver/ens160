[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver ENS160

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ens160/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

The ENS160 is a digital multi-gas sensor solution, based on metal oxide (MOX) technology with four sensor elements. The independent hotplate control allows the detection of a wide range of volatile organic compounds (VOCs) including ethanol, toluene, hydrogen and oxidizing gases with superior sensitivity. The ENS160 supports intelligent algorithms to process raw sensor measurements on-chip. These algorithms calculate CO2-equivalents, TVOC, air quality indices (AQIs) and perform humidity and temperature compensation, as well as baseline management, all on chip.

LibDriver ENS160 is a full-featured driver for ENS160, launched by LibDriver. It provides AQI reading, TVOC reading, eCO2 reading, ETOH reading and additional features. LibDriver is MISRA compliant.

### Table of Contents

  - [Instruction](#Instruction)
  - [Install](#Install)
  - [Usage](#Usage)
    - [example basic](#example-basic)
    - [example interrupt](#example-interrupt)
  - [Document](#Document)
  - [Contributing](#Contributing)
  - [License](#License)
  - [Contact Us](#Contact-Us)

### Instruction

/src includes LibDriver ENS160 source files.

/interface includes LibDriver ENS160 IIC and SPI platform independent template.

/test includes LibDriver ENS160 driver test code and this code can test the chip necessary function simply.

/example includes LibDriver ENS160 sample code.

/doc includes LibDriver ENS160 offline document.

/datasheet includes ENS160 datasheet.

/project includes the common Linux and MCU development board sample code. All projects use the shell script to debug the driver and the detail instruction can be found in each project's README.md.

/misra includes the LibDriver MISRA code scanning results.

### Install

Reference /interface IIC and SPI platform independent template and finish your platform IIC and SPI driver.

Add the /src directory, the interface driver for your platform, and your own drivers to your project, if you want to use the default example drivers, add the /example directory to your project.

### Usage

You can refer to the examples in the /example directory to complete your own driver. If you want to use the default programming examples, here's how to use them.

#### example basic

```C
#include "driver_ens160_basic.h"

uint8_t res;
uint32_t i;
uint8_t aqi_uba;
uint16_t tvoc_ppb;
uint16_t eco2_ppm;
uint16_t etoh_ppb;
float resistance_ohm[4];

/* basic init */
res = ens160_basic_init(ENS160_INTERFACE_IIC, ENS160_ADDRESS_0);
if (res != 0)
{
    return 1;
}

...
    
/* loop */
for (i = 0; i < 3; i++)
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
    
    ...
        
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
    
    ...
}

...
    
/* basic deinit */
(void)ens160_basic_deinit();

return 0;
```

#### example interrupt

```C
#include "driver_ens160_interrupt.h"

uint8_t res;
uint32_t i;
static volatile uint8_t gs_flag;
uint8_t (*g_gpio_irq)(void) = NULL;

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

/* gpio init */
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}

/* set the interrupt irq */
g_gpio_irq = ens160_interrupt_irq_handler;

/* interrupt init */
res = ens160_interrupt_init(ENS160_INTERFACE_IIC, ENS160_ADDRESS_0, a_callback);
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;
}

...
    
for (i = 0; i < 3; i++)
{
    /* wait for one group output */
    gs_flag = 0;
    while (gs_flag == 0)
    {
    }
    gs_flag = 0;

    ens160_interface_debug_print("ens160: find interrupt.\n");
    
    ...
}

...
    
/* gpio deinit */
(void)gpio_interrupt_deinit();

/* interrupt deinit */
(void)ens160_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### Document

Online documents: [https://www.libdriver.com/docs/ens160/index.html](https://www.libdriver.com/docs/ens160/index.html).

Offline documents: /doc/html/index.html.

### Contributing

Please refer to CONTRIBUTING.md.

### License

Copyright (c) 2015 - present LibDriver All rights reserved



The MIT License (MIT) 



Permission is hereby granted, free of charge, to any person obtaining a copy

of this software and associated documentation files (the "Software"), to deal

in the Software without restriction, including without limitation the rights

to use, copy, modify, merge, publish, distribute, sublicense, and/or sell

copies of the Software, and to permit persons to whom the Software is

furnished to do so, subject to the following conditions: 



The above copyright notice and this permission notice shall be included in all

copies or substantial portions of the Software. 



THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,

FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE

AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER

LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

SOFTWARE. 

### Contact Us

Please send an e-mail to lishifenging@outlook.com.