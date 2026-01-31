[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver ENS160

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ens160/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

ENS160は、4つのセンサー素子を備えた金属酸化物（MOX）技術をベースとしたデジタルマルチガスセンサーソリューションです。独立したホットプレート制御により、エタノール、トルエン、水素、酸化性ガスなど、幅広い揮発性有機化合物（VOC）を優れた感度で検出できます。ENS160は、センサーからの生の測定値をオンチップで処理するインテリジェントなアルゴリズムをサポートしています。これらのアルゴリズムは、CO2換算値、TVOC、空気質指標（AQI）を計算し、湿度と温度の補正、そしてベースライン管理をすべてオンチップで実行します。

LibDriver ENS160は、LibDriverがリリースしたENS160用のフル機能ドライバです。AQI、TVOC、eCO2、ETOHの測定機能に加え、追加機能も提供します。LibDriverはMISRAに準拠しています。

### 目次

  - [説明](#説明)
  - [インストール](#インストール)
  - [使用](#使用)
    - [example basic](#example-basic)
    - [example interrupt](#example-interrupt)
  - [ドキュメント](#ドキュメント)
  - [貢献](#貢献)
  - [著作権](#著作権)
  - [連絡して](#連絡して)

### 説明

/ srcディレクトリには、LibDriver ENS160のソースファイルが含まれています。

/ interfaceディレクトリには、LibDriver ENS160用のプラットフォームに依存しないIIC, SPIバステンプレートが含まれています。

/ testディレクトリには、チップの必要な機能を簡単にテストできるLibDriver ENS160ドライバーテストプログラムが含まれています。

/ exampleディレクトリには、LibDriver ENS160プログラミング例が含まれています。

/ docディレクトリには、LibDriver ENS160オフラインドキュメントが含まれています。

/ datasheetディレクトリには、ENS160データシートが含まれています。

/ projectディレクトリには、一般的に使用されるLinuxおよびマイクロコントローラー開発ボードのプロジェクトサンプルが含まれています。 すべてのプロジェクトは、デバッグ方法としてシェルスクリプトを使用しています。詳細については、各プロジェクトのREADME.mdを参照してください。

/ misraはLibDriver misraコードスキャン結果を含む。

### インストール

/ interfaceディレクトリにあるプラットフォームに依存しないIIC, SPIバステンプレートを参照して、指定したプラットフォームのIIC, SPIバスドライバを完成させます。

/src ディレクトリ、プラットフォームのインターフェイス ドライバー、および独自のドライバーをプロジェクトに追加します。デフォルトのサンプル ドライバーを使用する場合は、/example ディレクトリをプロジェクトに追加します。

### 使用

/example ディレクトリ内のサンプルを参照して、独自のドライバーを完成させることができます。 

/src ディレクトリ、プラットフォームのインターフェイス ドライバー、および独自のドライバーをプロジェクトに追加します。デフォルトのサンプル ドライバーを使用する場合は、/example ディレクトリをプロジェクトに追加します。

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

### ドキュメント

オンラインドキュメント: [https://www.libdriver.com/docs/ens160/index.html](https://www.libdriver.com/docs/ens160/index.html)。

オフラインドキュメント: /doc/html/index.html。

### 貢献

CONTRIBUTING.mdを参照してください。

### 著作権

著作権（c）2015-今 LibDriver 全著作権所有

MITライセンス（MIT）

このソフトウェアおよび関連するドキュメントファイル（「ソフトウェア」）のコピーを取得した人は、無制限の使用、複製、変更、組み込み、公開、配布、サブライセンスを含む、ソフトウェアを処分する権利を制限なく付与されます。ソフトウェアのライセンスおよび/またはコピーの販売、および上記のようにソフトウェアが配布された人の権利のサブライセンスは、次の条件に従うものとします。

上記の著作権表示およびこの許可通知は、このソフトウェアのすべてのコピーまたは実体に含まれるものとします。

このソフトウェアは「現状有姿」で提供され、商品性、特定目的への適合性、および非侵害の保証を含むがこれらに限定されない、明示または黙示を問わず、いかなる種類の保証もありません。 いかなる場合も、作者または著作権所有者は、契約、不法行為、またはその他の方法で、本ソフトウェアおよび本ソフトウェアの使用またはその他の廃棄に起因または関連して、請求、損害、またはその他の責任を負わないものとします。

### 連絡して

お問い合わせくださいlishifenging@outlook.com。