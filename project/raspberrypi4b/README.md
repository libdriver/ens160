### 1. Board

#### 1.1 Board Info

Board Name: Raspberry Pi 4B.

IIC Pin: SCL/SDA GPIO3/GPIO2.

SPI Pin: SCLK/MOSI/MISO/CS GPIO11/GPIO10/GPIO9/GPIO8.

GPIO Pin: INT GPIO17.

### 2. Install

#### 2.1 Dependencies

Install the necessary dependencies.

```shell
sudo apt-get install libgpiod-dev pkg-config cmake -y
```

#### 2.2 Makefile

Build the project.

```shell
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

#### 2.3 CMake

Build the project.

```shell
mkdir build && cd build 
cmake .. 
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

Test the project and this is optional.

```shell
make test
```

Find the compiled library in CMake. 

```cmake
find_package(ens160 REQUIRED)
```
### 3. ENS160

#### 3.1 Command Instruction

1. Show ens160 chip and driver information.

   ```shell
   ens160 (-i | --information)
   ```

2. Show ens160 help.

   ```shell
   ens160 (-h | --help)
   ```

3. Show ens160 pin connections of the current board.

   ```shell
   ens160 (-p | --port)
   ```

4. Run ens160 register test.

   ```shell
   ens160 (-t reg | --test=reg) [--addr=<0 | 1>] [--interface=<iic | spi>]
   ```

5. Run ens160 read test, num means the test times.

   ```shell
   ens160 (-t read | --test=read) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
   ```

6. Run ens160 interrupt test.

   ```shell
   ens160 (-t int | --test=int) [--addr=<0 | 1>] [--interface=<iic | spi>]
   ```

7. Run ens160 basic function, num is the read times.

   ```shell
   ens160 (-e basic | --example=basic) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
   ```

8. Run ens160 raw function, num is the read times.

   ```shell
   ens160 (-e raw | --example=raw) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
   ```

9. Run ens160 interrupt function,  num is the read times.

    ```shell
    ens160 (-e int | --example=int) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
    ```

#### 3.2 Command Example

```shell
./ens160 -i

ens160: chip is ScioSense ENS160.
ens160: manufacturer is ScioSense.
ens160: interface is IIC SPI.
ens160: driver version is 1.0.
ens160: min supply voltage is 1.7V.
ens160: max supply voltage is 2.0V.
ens160: max current is 79.00mA.
ens160: max temperature is 85.0C.
ens160: min temperature is -40.0C.
```

```shell
./ens160 -p

ens160: SPI interface SCK connected to GPIO11(BCM).
ens160: SPI interface MISO connected to GPIO9(BCM).
ens160: SPI interface MOSI connected to GPIO10(BCM).
ens160: SPI interface CS connected to GPIO8(BCM).
ens160: IIC interface SCL connected to GPIO3(BCM).
ens160: IIC interface SDA connected to GPIO2(BCM).
ens160: INT connected to GPIO17(BCM).
```

```shell
./ens160 -t reg --addr=0 --interface=iic

ens160: chip is ScioSense ENS160.
ens160: manufacturer is ScioSense.
ens160: interface is IIC SPI.
ens160: driver version is 1.0.
ens160: min supply voltage is 1.7V.
ens160: max supply voltage is 2.0V.
ens160: max current is 79.00mA.
ens160: max temperature is 85.0C.
ens160: min temperature is -40.0C.
ens160: start register test.
ens160: ens160_set_interface/ens160_get_interface test.
ens160: set interface iic.
ens160: check interface ok.
ens160: set interface spi.
ens160: check interface ok.
ens160: ens160_set_addr_pin/ens160_get_addr_pin test.
ens160: set address0.
ens160: check addr pin ok.
ens160: set address1.
ens160: check addr pin ok.
ens160: ens160_set_mode/ens160_get_mode test.
ens160: set deep sleep mode.
ens160: check mode ok.
ens160: set standard mode.
ens160: check mode ok.
ens160: set idle mode.
ens160: check mode ok.
ens160: ens160_set_interrupt_pin_polarity/ens160_get_interrupt_pin_polarity test.
ens160: set interrupt pin polarity high.
ens160: check interrupt pin polarity ok.
ens160: set interrupt pin polarity low.
ens160: check interrupt pin polarity ok.
ens160: ens160_set_interrupt_pin_drive/ens160_get_interrupt_pin_drive test.
ens160: set interrupt pin drive push-pull.
ens160: check interrupt pin drive ok.
ens160: set interrupt pin drive open drain.
ens160: check interrupt pin drive ok.
ens160: ens160_set_general_purpose_read_interrupt_pin_asserted/ens160_get_general_purpose_read_interrupt_pin_asserted test.
ens160: enable general purpose read interrupt pin asserted.
ens160: check general purpose read interrupt pin asserted ok.
ens160: disable general purpose read interrupt pin asserted.
ens160: check general purpose read interrupt pin asserted ok.
ens160: ens160_set_data_interrupt_pin_asserted/ens160_get_data_interrupt_pin_asserted test.
ens160: enable data interrupt pin asserted.
ens160: check data interrupt pin asserted ok.
ens160: disable data interrupt pin asserted.
ens160: check data interrupt pin asserted ok.
ens160: ens160_set_interrupt/ens160_get_interrupt test.
ens160: enable interrupt.
ens160: check interrupt ok.
ens160: disable interrupt.
ens160: check interrupt ok.
ens160: ens160_get_app_version test.
ens160: app version is 5.4.6.
ens160: ens160_set_temperature_compensation/ens160_get_temperature_compensation test.
ens160: set temperature compensation 0xB0F2.
ens160: check temperature compensation ok.
ens160: ens160_set_humidity_compensation/ens160_get_humidity_compensation test.
ens160: set humidity compensation 0x5641.
ens160: check humidity compensation ok.
ens160: ens160_set_params/ens160_get_params test.
ens160: set params[0] 0xA2.
ens160: set params[1] 0x55.
ens160: set params[2] 0x53.
ens160: set params[3] 0xBE.
ens160: set params[4] 0xF1.
ens160: set params[5] 0xFC.
ens160: set params[6] 0xF9.
ens160: set params[7] 0x79.
ens160: check params[0] ok.
ens160: check params[1] ok.
ens160: check params[2] ok.
ens160: check params[3] ok.
ens160: check params[4] ok.
ens160: check params[5] ok.
ens160: check params[6] ok.
ens160: check params[7] ok.
ens160: ens160_get_params_output test.
ens160: get params[0] output 0x50.
ens160: get params[1] output 0x00.
ens160: get params[2] output 0x00.
ens160: get params[3] output 0x00.
ens160: get params[4] output 0x05.
ens160: get params[5] output 0x04.
ens160: get params[6] output 0x06.
ens160: get params[7] output 0x0F.
ens160: ens160_get_status test.
ens160: status is 0x02.
ens160: ens160_get_calculation_temperature test.
ens160: calculation temperature is 0x0000.
ens160: ens160_get_calculation_humidity test.
ens160: calculation humidity is 0x0000.
ens160: ens160_get_checksum test.
ens160: checksum is 0x54.
ens160: ens160_nop test.
ens160: check nop ok.
ens160: ens160_clear test.
ens160: check clear ok.
ens160: ens160_soft_reset test.
ens160: check soft reset ok.
ens160: ens160_temperature_convert_to_register/ens160_temperature_convert_to_data test.
ens160: set temperature 18.10C.
ens160: check temperature 18.00C.
ens160: ens160_humidity_convert_to_register/ens160_humidity_convert_to_data test.
ens160: set humidity 2.70%.
ens160: check humidity 2.70%.
ens160: ens160_resistance_convert_to_register/ens160_resistance_convert_to_data test.
ens160: set resistance 69.00ohm.
ens160: check resistance 68.99ohm.
ens160: ens160_resistance_split test.
ens160: set raw[0] 0x13.
ens160: set raw[1] 0xE9.
ens160: set raw[2] 0xE2.
ens160: set raw[3] 0x2D.
ens160: set raw[4] 0x51.
ens160: set raw[5] 0x8E.
ens160: set raw[6] 0x1F.
ens160: set raw[7] 0x56.
ens160: resistance0 is 0xE913.
ens160: resistance1 is 0x2DE2.
ens160: resistance2 is 0x8E51.
ens160: resistance3 is 0x561F.
ens160: finish register test.
```

```shell
./ens160 -t read --addr=0 --interface=iic --times=3

ens160: chip is ScioSense ENS160.
ens160: manufacturer is ScioSense.
ens160: interface is IIC SPI.
ens160: driver version is 1.0.
ens160: min supply voltage is 1.7V.
ens160: max supply voltage is 2.0V.
ens160: max current is 79.00mA.
ens160: max temperature is 85.0C.
ens160: min temperature is -40.0C.
ens160: start read test.
ens160: ens160_read_aqi test.
ens160: aqi is 1.
ens160: aqi is 1.
ens160: aqi is 1.
ens160: ens160_read_tvoc test.
ens160: tvoc is 27 ppb.
ens160: tvoc is 23 ppb.
ens160: tvoc is 16 ppb.
ens160: ens160_read_eco2 test.
ens160: eco2 is 423 ppm.
ens160: eco2 is 409 ppm.
ens160: eco2 is 425 ppm.
ens160: ens160_read_etoh test.
ens160: etoh is 22 ppb.
ens160: etoh is 17 ppb.
ens160: etoh is 21 ppb.
ens160: ens160_get_params_output test.
ens160: resistance0 is 20018.72 ohm.
ens160: resistance1 is 1.00 ohm.
ens160: resistance2 is 7289.19 ohm.
ens160: resistance3 is 13139.66 ohm.
ens160: resistance0 is 19696.14 ohm.
ens160: resistance1 is 1.00 ohm.
ens160: resistance2 is 7331.25 ohm.
ens160: resistance3 is 13251.31 ohm.
ens160: resistance0 is 19477.38 ohm.
ens160: resistance1 is 1.00 ohm.
ens160: resistance2 is 7242.47 ohm.
ens160: resistance3 is 12723.91 ohm.
ens160: finish read test.
```

```shell
./ens160 -t int --addr=0 --interface=iic --times=3

ens160: chip is ScioSense ENS160.
ens160: manufacturer is ScioSense.
ens160: interface is IIC SPI.
ens160: driver version is 1.0.
ens160: min supply voltage is 1.7V.
ens160: max supply voltage is 2.0V.
ens160: max current is 79.00mA.
ens160: max temperature is 85.0C.
ens160: min temperature is -40.0C.
ens160: start interrupt test.
ens160: irq newdat.
ens160: aqi is 1.
ens160: tvoc is 10 ppb.
ens160: eco2 is 400 ppm.
ens160: etoh is 10 ppb.
ens160: irq newgpr.
ens160: resistance0 is 0x6EE2
ens160: resistance1 is 0x0000.
ens160: resistance2 is 0x6279.
ens160: resistance3 is 0x6CEC.
ens160: find interrupt.
ens160: irq newdat.
ens160: aqi is 1.
ens160: tvoc is 1 ppb.
ens160: eco2 is 400 ppm.
ens160: etoh is 1 ppb.
ens160: irq newgpr.
ens160: resistance0 is 0x6F8C
ens160: resistance1 is 0x0000.
ens160: resistance2 is 0x6377.
ens160: resistance3 is 0x6D8B.
ens160: find interrupt.
ens160: irq newdat.
ens160: aqi is 1.
ens160: tvoc is 19 ppb.
ens160: eco2 is 400 ppm.
ens160: etoh is 19 ppb.
ens160: irq newgpr.
ens160: resistance0 is 0x7094
ens160: resistance1 is 0x0000.
ens160: resistance2 is 0x6525.
ens160: resistance3 is 0x6E7D.
ens160: find interrupt.
ens160: finish interrupt test.
```

```shell
./ens160 -e basic --addr=0 --interface=iic --times=3

ens160: aqi is 1.
ens160: tvoc is 43 ppb.
ens160: eco2 is 443 ppm.
ens160: etoh is 43 ppb.
ens160: aqi is 1.
ens160: tvoc is 39 ppb.
ens160: eco2 is 434 ppm.
ens160: etoh is 39 ppb.
ens160: aqi is 1.
ens160: tvoc is 47 ppb.
ens160: eco2 is 451 ppm.
ens160: etoh is 47 ppb.
```

```shell
./ens160 -e raw --addr=0 --interface=iic --times=3

ens160: resistance0 is 19497.16 ohm.
ens160: resistance1 is 1.00 ohm.
ens160: resistance2 is 7249.83 ohm.
ens160: resistance3 is 14275.66 ohm.
ens160: resistance0 is 19649.53 ohm.
ens160: resistance1 is 1.00 ohm.
ens160: resistance2 is 7353.62 ohm.
ens160: resistance3 is 14017.12 ohm.
ens160: resistance0 is 20236.71 ohm.
ens160: resistance1 is 1.00 ohm.
ens160: resistance2 is 7316.38 ohm.
ens160: resistance3 is 14078.93 ohm.
```

```shell
./ens160 -e int --addr=0 --interface=iic --times=3

ens160: irq newdat.
ens160: aqi is 1.
ens160: tvoc is 24 ppb.
ens160: eco2 is 400 ppm.
ens160: etoh is 24 ppb.
ens160: irq newgpr.
ens160: resistance0 is 0x6E1C
ens160: resistance1 is 0x0000.
ens160: resistance2 is 0x62B3.
ens160: resistance3 is 0x6D00.
ens160: find interrupt.
ens160: irq newdat.
ens160: aqi is 1.
ens160: tvoc is 28 ppb.
ens160: eco2 is 410 ppm.
ens160: etoh is 28 ppb.
ens160: irq newgpr.
ens160: resistance0 is 0x7188
ens160: resistance1 is 0x0000.
ens160: resistance2 is 0x6689.
ens160: resistance3 is 0x6E75.
ens160: find interrupt.
ens160: irq newdat.
ens160: aqi is 1.
ens160: tvoc is 18 ppb.
ens160: eco2 is 400 ppm.
ens160: etoh is 18 ppb.
ens160: irq newgpr.
ens160: resistance0 is 0x71BC
ens160: resistance1 is 0x0000.
ens160: resistance2 is 0x66B0.
ens160: resistance3 is 0x6EB7.
ens160: find interrupt.
```

```shell
./ens160 -h

Usage:
  ens160 (-i | --information)
  ens160 (-h | --help)
  ens160 (-p | --port)
  ens160 (-t reg | --test=reg) [--addr=<0 | 1>] [--interface=<iic | spi>]
  ens160 (-t read | --test=read) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
  ens160 (-t int | --test=int) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
  ens160 (-e basic | --example=basic) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
  ens160 (-e raw | --example=raw) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]
  ens160 (-e int | --example=int) [--addr=<0 | 1>] [--interface=<iic | spi>] [--times=<num>]

Options:
      --addr=<0 | 1>                 Set the chip address.([default: 0])
  -e <basic | raw | int>, --example=<basic | raw | int>
                                     Run the driver example.
  -h, --help                         Show the help.
  -i, --information                  Show the chip information.
      --interface=<iic | spi>        Set the chip interface.([default: iic])
  -p, --port                         Display the pin connections of the current board.
  -t <reg | read | int>, --test=<reg | read | int>
                                     Run the driver test.
      --times=<num>                  Set the running times.([default: 3])
```

