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

# include base
. /opt/MeetiXOSProject/MXbase.sh

# copy first argument to local variable
target=$1

##
#	install developer packages
##
function installPackets()
{
	headline "Getting start directory"
	startDir=`pwd`

	headline "Move the MeetiXOSProject dir to /opt/"
	cd "developers"
	sudo mv -r "MeetiXOSProject" "/opt"

	headline "Installing toolchains"
	cd "/opt/MeetiXOSProject/"
	sudo bash toolchain.sh

	headline "Running installation script"
	sudo bash install.sh "install"
	
	headline "Remove Garbage"
	cd startDir
	rm -rf "developers"
}

##
#	uninstall developer packages
##
function uninstallPackets()
{
	headline "This option remove only the dir on "$TOOLCHAIN_BASE" and links on bin"
	
	headline "Removing links"
	sudo bash install.sh "uninstall"

	headline "Removing developer packages"
	sudo rm -rf "/opt/MeetiXOSProject"
}

##
#	call compile of application directory
##
function applications()
{
	cd applications
	. compile.sh $1
	cd ..
}

##
#	call compile of EvangelionNG directory
##
function kernel()
{
	cd EvangelionNG
	. build.sh $1
	cd ..
}

# switch arguments
if [[ $target == "install" ]]; then
	installPackets

elif [[ $target == "uninstall" ]]; then
	uninstallPackets

elif [[ $target == "all" ]]; then
	kernel $target
	applications $target

elif [[ $target == "eva" ]]; then
	kernel $target

elif [[ $target == "repack" ]]; then
	kernel $target

elif [[ $target == "app" ]]; then
	applications all

elif [[ $target == "qemu" ]]; then
	kernel $target

elif [[ $target == "vbox" ]]; then
	kernel $target

else
	headline "unknown target: '$target'"
	exit 1
fi