#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC="src"
INC="src"
OBJ="obj"
ARTIFACT_NAME="exec"
ARTIFACT_TYPE="application"
COMPILER="i686-mx-g++"
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include build tasks
. ../../builder.sh
