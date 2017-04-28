#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src
INC=src
OBJ=obj
ARTIFACT_NAME=CandyTerminal
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$SRC -I$SYSROOT_INCLUDE/freetype2"
LDFLAGS="-lcairo -lfreetype -lpixman-1 -lpng -lz"

# Include application build tasks
. ../applications.sh
