#!/bin/bash
# Ce script genere les impulsions periodiques (1 sec) de securite

# Initialisation : on configure la pin en sortie
gpio mode 7 out 

# Boucle de generation des impulsions
while true
do
	gpio write 7 1
	sleep 0.5
	gpio write 7 0
	sleep 0.5
done
