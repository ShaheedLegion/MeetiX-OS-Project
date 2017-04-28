#!/bin/bash

################################################################################## 
# MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]					 #
# This program is free software; you can redistribute it and/or  				 #
# modify it under the terms of the GNU General Public License                    #
# as published by the Free Software Foundation; either version 2 				 #
# of the License, or (at your option) any later version.						 #
# 																			     #
# This program is distributed in the hope that it will be useful,                #
# but WITHOUT ANY WARRANTY; without even the implied warranty of 				 #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 				 #
# GNU General Public License for more details.									 #
# 																				 #
# You should have received a copy of the GNU General Public License				 #
# along with this program; if not, write to the Free Software 					 #
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA #
##################################################################################

TARGET=$1
ARTIFACT_LOCAL=$OBJ/$ARTIFACT_NAME

echo "target: $TARGET"
requireTool changes

##
#	remove the object from compile dir
##
function targetClean() 
{
	echo "cleaning:"
	remove $ARTIFACT_LOCAL
	cleanDirectory $OBJ
	changes --clear
}

##
#	compile all file that it find
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
#	link object created from compilation
##
function targetLink() 
{
	echo "linking:"
	i686-mx-g++ -o $ARTIFACT_LOCAL $OBJ/*.o -lmxuser $LDFLAGS
	list $ARTIFACT_LOCAL
}

##
#	clean previous executable in install dir
##	
function targetCleanTarget() 
{
	echo "cleaning target:"
	rm $ARTIFACT_TARGET 2&> /dev/null
	list $ARTIFACT_TARGET
}

function targetInstall() 
{
	targetCleanTarget
	
	echo "installing artifact"
	cp $ARTIFACT_LOCAL $ARTIFACT_TARGET
}


# execute targets
if [[ $TARGET == "all" ]]; then
	targetCompile
	targetLink
	targetInstall
	
elif [[ $TARGET == "clean" ]]; then
	targetClean
	
else
	echo "unknown target: '$TARGET'"
	exit 1
fi

exit 0
