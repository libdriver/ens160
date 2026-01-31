[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver ENS160

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ens160/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

ENS160是一款基于金属氧化物（MOX）技术的数字式多气体传感器解决方案，包含四个传感器元件。其独立的加热板控制功能可检测包括乙醇、甲苯、氢气和氧化性气体在内的多种挥发性有机化合物（VOC），且具有出色的灵敏度。ENS160支持智能算法，可在芯片上处理原始传感器测量数据。这些算法可计算二氧化碳当量、总挥发性有机化合物（TVOC）、空气质量指数（AQI），并进行湿度和温度补偿以及基线管理，所有这些都在芯片上完成。

LibDriver ENS160是LibDriver推出的ENS160的全功能驱动，该驱动提供AQI读取，TVOC读取，eCO2读取，ETOH读取等功能并且它符合MISRA标准。

### 目录

  - [说明](#说明)
  - [安装](#安装)
  - [使用](#使用)
    - [example basic](#example-basic)
    - [example interrupt](#example-interrupt)
  - [文档](#文档)
  - [贡献](#贡献)
  - [版权](#版权)
  - [联系我们](#联系我们)

### 说明

/src目录包含了LibDriver ENS160的源文件。

/interface目录包含了LibDriver ENS160与平台无关的IIC、SPI总线模板。

/test目录包含了LibDriver ENS160驱动测试程序，该程序可以简单的测试芯片必要功能。

/example目录包含了LibDriver ENS160编程范例。

/doc目录包含了LibDriver ENS160离线文档。

/datasheet目录包含了ENS160数据手册。

/project目录包含了常用Linux与单片机开发板的工程样例。所有工程均采用shell脚本作为调试方法，详细内容可参考每个工程里面的README.md。

/misra目录包含了LibDriver MISRA代码扫描结果。

### 安装

参考/interface目录下与平台无关的IIC、SPI总线模板，完成指定平台的IIC、SPI总线驱动。

将/src目录，您使用平台的接口驱动和您开发的驱动加入工程，如果您想要使用默认的范例驱动，可以将/example目录加入您的工程。

### 使用

您可以参考/example目录下的编程范例完成适合您的驱动，如果您想要使用默认的编程范例，以下是它们的使用方法。

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

### 文档

在线文档: [https://www.libdriver.com/docs/ens160/index.html](https://www.libdriver.com/docs/ens160/index.html)。

离线文档: /doc/html/index.html。

### 贡献

请参考CONTRIBUTING.md。

### 版权

版权 (c) 2015 - 现在 LibDriver 版权所有

MIT 许可证（MIT）

特此免费授予任何获得本软件副本和相关文档文件（下称“软件”）的人不受限制地处置该软件的权利，包括不受限制地使用、复制、修改、合并、发布、分发、转授许可和/或出售该软件副本，以及再授权被配发了本软件的人如上的权利，须在下列条件下：

上述版权声明和本许可声明应包含在该软件的所有副本或实质成分中。

本软件是“如此”提供的，没有任何形式的明示或暗示的保证，包括但不限于对适销性、特定用途的适用性和不侵权的保证。在任何情况下，作者或版权持有人都不对任何索赔、损害或其他责任负责，无论这些追责来自合同、侵权或其它行为中，还是产生于、源于或有关于本软件以及本软件的使用或其它处置。

### 联系我们

请联系lishifenging@outlook.com。