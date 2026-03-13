#!/bin/sh -e

#xinput --map-to-output $(xinput list --id-only "Edamak LG TS2009F-USB") DP-1

#xinput --map-to-output $(xinput list --id-only "eGalax Inc. USB TouchController") DP-1

#xinput --map-to-output $(xinput list --id-only "Edamak LG TS2009F-USB") DP-1

#xinput --map-to-output $(xinput list --id-only "eGalax Inc. USB TouchController") DP-1

export DISPLAY=:0.0
xhost local:root

#date > /home/user/on-boot.txt

cd /home/user/mert/bin
