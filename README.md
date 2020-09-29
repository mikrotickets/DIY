# MikroTickets DIY 

MikroTickets DIY is a low cost project designed to build a coin acceptor Mikrotik compatible voucher hotspot seller machine.

Using the easiest to find components and the lowest possible connections, this project allow to directly generate Voucher users to any RB Mikrotik via Wifi and Telnet communication


## Instructions: 
We prepared everything needed to load the project as a precompiled binary ready to be uploaded to the microcontroller, no need to configure any Arduino IDE neither to install complicated libraries. 


### Components:
* NodeMCU Ver 0.1.
* LCD 16x2.
* LCD i2c Adapter.
* Push Buttom 12mm momentary.
* 1 pc Rectifier Diode (1N4001 or 1N4002 or 1N4003... or equivalent).
* Power Supply 12V(Max) @ 2A(Min).

## Connections:

### NodeMCU PIN  to  > LCD i2c adapter (soldered to LCD 16*2)
* VU   >  VCC (LCD i2c adapter).
* GND  >  GND (LCD i2c adapter).
* D2   >  SDA (LCD i2c adapter).
* D1   >  SCL (LCD i2c adapter).

### Electronic Coin Aceptor > Power Source 12 VCC @ 2 amp.
* Coin Signal > Diode >  D6   (Diode white mark facing coin aceptor).
* 12 vcc      >  12 Vcc (+).
* GND         >   GND   >   D6 NodeMCU GND any Pin.

### Push Buttom 12mm momentary > Arduino PIN
* D5   >  PIN momentary Push Buttom.
* GND  >  PIN momentary Push Buttom.

![picture](/Connections.png)
