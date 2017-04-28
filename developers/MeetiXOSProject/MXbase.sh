#!/bin/bash
# MeetiX common build functionality

#
# Prints a header
#
function headline() 
{
	printf "\e[0;1m$1\e[0m:\n"
}

function targetHeadline() 
{
	printf "TARGET: \e[0;7m$1\e[0m\n\n"	
}

#
# Overrides a variable if it is not set
#
#   with CC "g++"
#
function with() 
{
	if [ -z "${!1}" ]; then eval $1='$2'; fi
}

#
# Requires the specified tool, quits with an error otherwise
#
#   requireTool "g++"
#
function requireTool() 
{
	if ! type "$1" >/dev/null 2>&1
	then
		>&2 echo "error: missing required tool '$1'"
		exit 1
	fi
}

#
# Prints a list entry
#
#   list "example.txt"
#
function list() 
{
	headline " - '$1'"	
}

#
# Cleans the given directory
#
#   cleanDirectory "bin"
#
function cleanDirectory() 
{
	if [ -d "$1" ]
	then
		rm -rf $1
	fi
	mkdir "$1"
	list $1
}

#
# Removes the given file
#
#	remove "file.txt"
#
function remove() 
{
	if [ -e "$1" ]
	then
		rm $1 2&> /dev/null
	fi
	list $1
}

#
# Fails if the previous command was erroneous
#
#   $CC -c ...
#   failOnError
#
function failOnError() 
{
	if [[ $? != 0 ]]; then
		echo "Build failed"
		exit 1
	fi	
}

#
# Converts a source path to an object
#
#   sourceToObject "src/myclass.cpp"
#
function sourceToObject() 
{
	echo $1.o | sed "s/\//_/g"
}


# Global variables
with TOOLCHAIN_BASE		"/opt/MeetiXOSProject"

with SYSROOT $TOOLCHAIN_BASE"/MXlibraries"
SYSROOT_BIN=$SYSROOT/bin
SYSROOT_INCLUDE=$SYSROOT/include
SYSROOT_LIB=$SYSROOT/lib
