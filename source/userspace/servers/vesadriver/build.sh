#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC="src"
INC="src"
OBJ="obj"
ARTIFACT_NAME="vbedriver.sv"
ARTIFACT_TYPE="server"
COMPILER="i686-mx-g++"
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS=""

# Include application build tasks
. ../../builder.sh
