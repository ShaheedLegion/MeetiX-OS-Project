#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src-write
INC=src-write
OBJ=obj-write
ARTIFACT_NAME=write
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
