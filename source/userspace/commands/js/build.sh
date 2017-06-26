#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC="src"
INC="src"
OBJ="obj"
ARTIFACT_NAME="js"
ARTIFACT_TYPE="command"
COMPILER="i686-mx-g++"
CFLAGS="-Os -pedantic -std=c++11 -Wall -fstrict-aliasing -Wunused-variable -fomit-frame-pointer -I./src -DDUK_OPT_SELF_TESTS -lm"
LDFLAGS=""

# Include build tasks
. ../../builder.sh
