#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC="src"
INC="src"
OBJ="obj"
ARTIFACT_NAME="wr"
ARTIFACT_TYPE="command"
COMPILER="i686-mx-gcc"
CFLAGS="-std=c11 -I$SRC"
LDFLAGS=""

# Include build tasks
. ../../builder.sh
