#!/bin/bash

# Libération des GPIOS
# Liste des GPIO : 4,17,18,22,23,24,25,27
cd /sys/class/gpio
echo 4 > unexport
echo 17 > unexport
echo 18 > unexport
echo 22 > unexport
echo 23 > unexport
echo 24 > unexport
echo 25 > unexport
echo 27 > unexport

# Lancement du module d'acquisition
sudo rmmod gpio-freq
sudo insmod /home/pi/Desktop/gpio-freq/gpio-freq.ko gpios=4,17,18,22,23

# Lancement du gateway
cd /home/pi/Desktop/gpio-freq
./send_period 5 100 0 -v
