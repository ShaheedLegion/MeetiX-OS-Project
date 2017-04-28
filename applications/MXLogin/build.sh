#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src
INC=src
OBJ=obj
ARTIFACT_NAME=MXLogin
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11 -I$INC"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
