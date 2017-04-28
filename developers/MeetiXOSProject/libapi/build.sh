#!/bin/sh
. ../MXbase.sh

TARGET=$1

with TARGET					"all"
with SRC					"src"
with OBJ					"obj"
with INC					"inc"
with INC_KERNEL				"/your/path/to/MeetiX-OS/EvangelionNG/inc"

with ARTIFACT_NAME			"libmxapi.a"
ARTIFACT_LOCAL="$ARTIFACT_NAME"
ARTIFACT_TARGET="$SYSROOT_LIB/$ARTIFACT_NAME"

with CFLAGS					"-std=c++11 -I$INC -I$INC_KERNEL"

# check tools
requireTool changes

# always create output folder
mkdir -p $OBJ

##
#	clean object
##
function targetClean() 
{
	echo "cleaning:"
	rm $ARTIFACT_LOCAL
	cleanDirectory $OBJ
	changes --clear
}

##
#	compile all sources that it find
##
function targetCompile() 
{
	echo "compiling:"
	
	# check if headers have changed
	headersHaveChanged=1
	for file in $(find "$INC" -iname "*.h" -o -iname "*.hpp"); do
		changes -c $file
		if [ $? -eq 1 ]; then
			headersHaveChanged=1
		fi
		changes -s $file
	done
	
	# compile sources
	for file in $(find "$SRC" -iname "*.c" -o -iname "*.cpp"); do 
		changes -c $file
		changed=$?
		if ( [ $headersHaveChanged -eq 1 ] || [ $changed -eq 1 ] ); then
			out=`sourceToObject $file`
			list $out
			i686-mx-g++ -c $file -o "$OBJ/$out" $CFLAGS
			failOnError
			changes -s $file
		fi
	done
}

##
#	create static lib archive
##
function targetArchive() 
{
	echo "archiving:"
	i686-mx-ar -r $ARTIFACT_LOCAL $OBJ/*.o
}

##
#	clean install directory
##
function targetCleanTarget() 
{
	echo "removing $ARTIFACT_TARGET"
	rm $ARTIFACT_TARGET 2&> /dev/null
}

##
#	install lib headers
##
function targetInstallHeaders() 
{
	echo "installing api headers"
	cp -r $INC/* $SYSROOT_INCLUDE/

	echo "installing kernel headers"
	cp -r $INC_KERNEL/* $SYSROOT_INCLUDE/
}

##
#	install all
##
function targetInstall() 
{
	targetCleanTarget
	targetInstallHeaders
	
	echo "installing artifact"
	cp $ARTIFACT_LOCAL $ARTIFACT_TARGET
	
	# c'mon
	chmod -R 777 $SYSROOT
}

# show chosed target
targetHeadline "target: $TARGET"

# execute targets
if [[ $TARGET == "install-headers" ]]; then
	targetInstallHeaders

elif [[ $TARGET == "all" ]]; then
	targetCompile
	targetArchive
	targetInstall
	
elif [[ $TARGET == "clean" ]]; then
	targetClean
	
else
	echo "unknown target: '$TARGET'"
	exit 1
fi

exit 0