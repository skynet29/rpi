#!/bin/bash

# Ce script positionne le GPIO de la RPi 1 permettant d'activer le bus PROP
# gpio write 6 0 => Bus PROP ON
# gpio write 6 1 => Bus PROP OFF

# On positionne l'etat des sorties avant de configurer la direction du GPIO
gpio write 6 0

# On configure le GPIO en sortie
gpio mode 6 out
