#!/bin/bash
PROGRAMMER=avrispmkII
PROGRAMMER_PORT=usb
DUDE="avrdude -p m8 -c $PROGRAMMER -P $PROGRAMMER_PORT -v"

$DUDE
