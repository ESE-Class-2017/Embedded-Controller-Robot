#!/bin/bash

#this script applies known working serial settings to
#the serial port given in its first argument

sudo stty -F $1 `cat working-tty-settings`
