#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src
INC=src
OBJ=obj
ARTIFACT_NAME=init
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-std=c++11"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
