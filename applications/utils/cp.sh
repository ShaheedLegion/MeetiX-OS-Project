#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src-cp
INC=src-cp
OBJ=obj-cp
ARTIFACT_NAME=cp
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
