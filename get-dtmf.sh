#!/bin/bash

#this script records a 6 second audio clip and detects
#any present dtmf tones

arecord -D hw:1,0 -d 6 -f S16_LE capture.wav

multimon -a DTMF -t wav capture.wav 2> /dev/null | grep DTMF:
