# light_system_iot

## Get started
### 1. Build and flash firmware to ESP32
#### Build project
```bash
# Use command line in Platform IO CLI
pio run
```
Use the build button of Platform IO in vscode

#### Flash firmware
1. Prepare 2 modules USB to TTL
   1. GPIO3 -> TX of USB UART for flash
   2. GPIO1 -> RX of USB UART for flash
   3. GPIO16 -> TX of USB UART for debug
   4. GPIO17 -> RX of USB UART for debug
2. Set the pin BOOT to GND and press Reset button
3. After booting successfully, unplug USB UART for booting and connect module LoRa.
4. Module LoRa
   1. GPIO3 -> TX of USB UART of LoRa
   2. GPIO1 -> RX of USB UART of LoRa
5. Set module LoRa to Config Mode and wait it config 

```text
Importance note: Make USB UART for debug as power supply of ESP32
```