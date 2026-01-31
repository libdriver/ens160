[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver ENS160

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ens160/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE) 

Der ENS160 ist ein digitaler Multigassensor auf Basis von Metalloxid-Technologie (MOX) mit vier Sensorelementen. Die unabhängige Heizplattensteuerung ermöglicht die hochempfindliche Detektion einer Vielzahl flüchtiger organischer Verbindungen (VOCs), darunter Ethanol, Toluol, Wasserstoff und oxidierende Gase. Der ENS160 unterstützt intelligente Algorithmen zur direkten Verarbeitung der Rohmesswerte. Diese Algorithmen berechnen CO₂-Äquivalente, TVOC, Luftqualitätsindizes (AQI) und führen Feuchtigkeits- und Temperaturkompensation sowie Baseline-Management durch – alles auf dem Chip.

LibDriver ENS160 ist ein umfassender Treiber für das ENS160-System, entwickelt von LibDriver. Er ermöglicht die Messung von Luftqualitätsindex (AQI), TVOC, eCO₂ und Ethanol (ETOH) sowie weitere Funktionen. LibDriver ist MISRA-konform.

### Inhaltsverzeichnis

  - [Anweisung](#Anweisung)
  - [Installieren](#Installieren)
  - [Nutzung](#Nutzung)
    - [example basic](#example-basic)
    - [example interrupt](#example-interrupt)
  - [Dokument](#Dokument)
  - [Beitrag](#Beitrag)
  - [Lizenz](#Lizenz)
  - [Kontaktieren Sie uns](#Kontaktieren-Sie-uns)

### Anweisung

/src enthält LibDriver ENS160-Quelldateien.

/interface enthält die plattformunabhängige Vorlage LibDriver ENS160 IIC, SPI.

/test enthält den Testcode des LibDriver ENS160-Treibers und dieser Code kann die erforderliche Funktion des Chips einfach testen.

/example enthält LibDriver ENS160-Beispielcode.

/doc enthält das LibDriver ENS160-Offlinedokument.

/Datenblatt enthält ENS160-Datenblatt.

/project enthält den allgemeinen Beispielcode für Linux- und MCU-Entwicklungsboards. Alle Projekte verwenden das Shell-Skript, um den Treiber zu debuggen, und die detaillierten Anweisungen finden Sie in der README.md jedes Projekts.

/misra enthält die Ergebnisse des LibDriver MISRA Code Scans.

### Installieren

Verweisen Sie auf eine plattformunabhängige IIC, SPI-Schnittstellenvorlage und stellen Sie Ihren Plattform-IIC, SPI-Treiber fertig.

Fügen Sie das Verzeichnis /src, den Schnittstellentreiber für Ihre Plattform und Ihre eigenen Treiber zu Ihrem Projekt hinzu. Wenn Sie die Standardbeispieltreiber verwenden möchten, fügen Sie das Verzeichnis /example zu Ihrem Projekt hinzu.

### Nutzung

Sie können auf die Beispiele im Verzeichnis /example zurückgreifen, um Ihren eigenen Treiber zu vervollständigen. Wenn Sie die Standardprogrammierbeispiele verwenden möchten, erfahren Sie hier, wie Sie diese verwenden.

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

### Dokument

Online-Dokumente: [https://www.libdriver.com/docs/ens160/index.html](https://www.libdriver.com/docs/ens160/index.html).

Offline-Dokumente: /doc/html/index.html.

### Beitrag

Bitte beachten Sie CONTRIBUTING.md.

### Lizenz

Urheberrechte © (c) 2015 - Gegenwart LibDriver Alle Rechte vorbehalten



Die MIT-Lizenz (MIT)



Hiermit wird jeder Person kostenlos die Erlaubnis erteilt, eine Kopie zu erhalten

dieser Software und zugehörigen Dokumentationsdateien (die „Software“) zu behandeln

in der Software ohne Einschränkung, einschließlich, aber nicht beschränkt auf die Rechte

zu verwenden, zu kopieren, zu modifizieren, zusammenzuführen, zu veröffentlichen, zu verteilen, unterzulizenzieren und/oder zu verkaufen

Kopien der Software und Personen, denen die Software gehört, zu gestatten

dazu eingerichtet werden, unter folgenden Bedingungen:



Der obige Urheberrechtshinweis und dieser Genehmigungshinweis müssen in allen enthalten sein

Kopien oder wesentliche Teile der Software.



DIE SOFTWARE WIRD "WIE BESEHEN" BEREITGESTELLT, OHNE JEGLICHE GEWÄHRLEISTUNG, AUSDRÜCKLICH ODER

STILLSCHWEIGEND, EINSCHLIESSLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT,

EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND NICHTVERLETZUNG VON RECHTEN DRITTER. IN KEINEM FALL DARF DAS

AUTOREN ODER URHEBERRECHTSINHABER HAFTEN FÜR JEGLICHE ANSPRÜCHE, SCHÄDEN ODER ANDERE

HAFTUNG, OB AUS VERTRAG, DELIKT ODER ANDERWEITIG, ENTSTEHEND AUS,

AUS ODER IM ZUSAMMENHANG MIT DER SOFTWARE ODER DER VERWENDUNG ODER ANDEREN HANDLUNGEN MIT DER

SOFTWARE.

### Kontaktieren Sie uns

Bitte senden Sie eine E-Mail an lishifenging@outlook.com.