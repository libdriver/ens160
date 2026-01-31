[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver ENS160

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ens160/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

ENS160是一款基於金屬氧化物（MOX）技術的數位式多氣體傳感器解決方案，包含四個傳感器元件。其獨立的加熱板控制功能可檢測包括乙醇、甲苯、氫氣和氧化性氣體在內的多種揮發性有機化合物（VOC），且具有出色的靈敏度。ENS160支持智能演算法，可在晶片上處理原始傳感器測量數據。這些演算法可計算二氧化碳當量、總揮發性有機化合物（TVOC）、空氣質量指數（AQI），並進行濕度和溫度補償以及基線管理，所有這些都在晶片上完成。

LibDriver ENS160是LibDriver推出的ENS160的全功能驅動，該驅動提供AQI讀取，TVOC讀取，eCO2讀取，ETOH讀取等功能並且它符合MISRA標準。

### 目錄

  - [說明](#說明)
  - [安裝](#安裝)
  - [使用](#使用)
    - [example basic](#example-basic)
    - [example interrupt](#example-interrupt)
  - [文檔](#文檔)
  - [貢獻](#貢獻)
  - [版權](#版權)
  - [聯繫我們](#聯繫我們)

### 說明

/src目錄包含了LibDriver ENS160的源文件。

/interface目錄包含了LibDriver ENS160與平台無關的IIC、SPI總線模板。

/test目錄包含了LibDriver ENS160驅動測試程序，該程序可以簡單的測試芯片必要功能。

/example目錄包含了LibDriver ENS160編程範例。

/doc目錄包含了LibDriver ENS160離線文檔。

/datasheet目錄包含了ENS160數據手冊。

/project目錄包含了常用Linux與單片機開發板的工程樣例。所有工程均採用shell腳本作為調試方法，詳細內容可參考每個工程裡面的README.md。

/misra目錄包含了LibDriver MISRA程式碼掃描結果。

### 安裝

參考/interface目錄下與平台無關的IIC、SPI總線模板，完成指定平台的IIC、SPI總線驅動。

將/src目錄，您使用平臺的介面驅動和您開發的驅動加入工程，如果您想要使用默認的範例驅動，可以將/example目錄加入您的工程。

### 使用

您可以參考/example目錄下的程式設計範例完成適合您的驅動，如果您想要使用默認的程式設計範例，以下是它們的使用方法。

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

### 文檔

在線文檔: [https://www.libdriver.com/docs/ens160/index.html](https://www.libdriver.com/docs/ens160/index.html)。

離線文檔: /doc/html/index.html。

### 貢獻

請參攷CONTRIBUTING.md。

### 版權

版權 (c) 2015 - 現在 LibDriver 版權所有

MIT 許可證（MIT）

特此免費授予任何獲得本軟件副本和相關文檔文件（下稱“軟件”）的人不受限制地處置該軟件的權利，包括不受限制地使用、複製、修改、合併、發布、分發、轉授許可和/或出售該軟件副本，以及再授權被配發了本軟件的人如上的權利，須在下列條件下：

上述版權聲明和本許可聲明應包含在該軟件的所有副本或實質成分中。

本軟件是“如此”提供的，沒有任何形式的明示或暗示的保證，包括但不限於對適銷性、特定用途的適用性和不侵權的保證。在任何情況下，作者或版權持有人都不對任何索賠、損害或其他責任負責，無論這些追責來自合同、侵權或其它行為中，還是產生於、源於或有關於本軟件以及本軟件的使用或其它處置。

### 聯繫我們

請聯繫lishifenging@outlook.com。