#!/bin/bash
PROGRAMMER=avrispmkII
PROGRAMMER_PORT=usb
DUDE="avrdude -p m8 -c $PROGRAMMER -P $PROGRAMMER_PORT -v"

$DUDE -U hfuse:w:0xca:m -U lfuse:w:0xdf:m
$DUDE
