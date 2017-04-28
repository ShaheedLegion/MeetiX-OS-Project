#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src
INC=src
OBJ=obj
ARTIFACT_NAME=spawner.sv
ARTIFACT_TARGET=../../MXfs/sys/svs/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
