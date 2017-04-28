#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src-reverse
INC=src-reverse
OBJ=obj-reverse
ARTIFACT_NAME=reverse
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
