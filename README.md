# Ardunio
## Flashen
* Arduino 1.0.5
* Tools/Board/Arduino NG or old w/ ATmega 8
* Sketch/überprüfen / Kompillieren
* Datei/Upload mit Programmer

## Konfiguration
/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/boards.txt:

##############################################################
```
atmega8.name=Arduino NG or older w/ ATmega8

atmega8.upload.protocol=arduino
atmega8.upload.maximum_size=7168
atmega8.upload.speed=19200

atmega8.bootloader.low_fuses=0xdf
atmega8.bootloader.high_fuses=0xca
atmega8.bootloader.path=atmega8

atmega8.build.mcu=atmega8
#atmega8.build.f_cpu=16000000L
# Wichtig! CPU Frequenz anpassen an Externen Quarz
atmega8.build.f_cpu=3686000L
atmega8.build.core=arduino
atmega8.build.variant=standard
```
__ Wichtig! CPU Frequenz anpassen an Externen Quarz __

