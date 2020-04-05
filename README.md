# MikroTickets DIY 

##Introduction.

MikroTickets DIY is a low cost project designed to build a coin acceptor Mikrotik compatible voucher hotspot seller machine.

Using the easiest to find components and the lowest possible connections, this project allow to sell a preloaded list of Voucher users on an SD card as CSV files (User,Passord). Up to 4 different 4  profiles on a LCD screen.

We prepared everything needed to load the project as a precompiled binary ready to be uploaded to the microcontroller, no need to configure any Arduino IDE nether to install complicated libraries. 

See Instructions: 


### Components:
* Arduino UNO SMD rev3.
* Micro SD card Reader.
* MicroSD card.
* LCD 16x2.
* LCD i2c Adapter.
* Push Buttom 12mm momentary.
* 1 pc Rectifier Diode (1N4001 or 1N4002 or 1N4003... or equivalent).
* Power Supply 12V(Max) @ 2A(Min).

## Connections:

### Micro SD card Reader > Arduino PIN
* CS   >  D4
* SCK  >  D13
* MOSI >  D12
* MISO >  D11
* VCC  >  5V
* GND  >  GND

### LCD i2c Adapter > Arduino PIN (i2C)
* GND > GND
* VCC > 5V
* SDA > SDA
* SCL > SCL

### Electronic Coin Aceptor (One coin Type) > Arduino PIN
* Coin Signal > Diode >  A0   (Diode white mark facing coin aceptor)
* 12 vcc      >          VIN
* GND         >          GND

### Push Buttom 12mm momentary > Arduino PIN
* 1 > GND
* 2 > A2

![picture](/Connections.png)
