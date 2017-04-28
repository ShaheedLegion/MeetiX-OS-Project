#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src-file
INC=src-file
OBJ=obj-file
ARTIFACT_NAME=file
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
