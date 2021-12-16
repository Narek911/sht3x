# Mixed STM32 library for ds18b20 and sht3x
## Development environment
  * STM32CubeIDE
  * Library: LL (low layer driver)
  * Library: HAL (hardware abstraction layer driver)
## Hardware connection (ds18b20 => STM32L071KB)
  * VDD ->   (+3.3V)
  * GND ->   (GND) 
  * DQ  ->   (PA9)
## Hardware connection (sht3x => STM32L071KB)
  * VDD ->   (+3.3V)
  * GND ->   (GND) 
  * SCL ->   I2C1 SCL (PA9)
  * SAA ->   I2C1 SDA (PA10)
## How to use
* The ds18b20 and sht3x librares are based on LL and you can migrate them to other STM32 boards with STM32CubeMX.
* The projects are built in STM32CubeIDE.
1.  connect the evaluation board to the host pc via usb cable
2.  connect ds18b20 OR sht3x as described above
3.  open the project sht3x in STM32CubeIDE.
4.  click "Build (OpenOCD)" to compile the project.
5.  press F11 to program the board
6.  open the Console tab in STM32CubeIDE to monitor the temperature readings
7.  Change the sensors  and monitor the reading in the console


