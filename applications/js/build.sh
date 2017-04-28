#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC=src
INC=src
OBJ=obj
ARTIFACT_NAME=javaScript
ARTIFACT_TARGET=../../MXfs/bin/$ARTIFACT_NAME
CFLAGS="-Os -pedantic -std=c++11 -Wall -fstrict-aliasing -fomit-frame-pointer -I./src -DDUK_OPT_SELF_TESTS -lm"
LDFLAGS=""

# Include application build tasks
. ../applications.sh
