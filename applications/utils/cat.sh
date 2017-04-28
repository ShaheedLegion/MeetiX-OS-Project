#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src-read
INC=src-read
OBJ=obj-read
ARTIFACT_NAME=cat
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
