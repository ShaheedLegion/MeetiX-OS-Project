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

. /opt/MeetiXOSProject/MXbase.sh

TARGET=$1
with TARGET "all"

# includes and source dir
INC=inc
BIN=bin
SRC_EVALD=src-evaloader
SRC_EVANG=src-evacore
SRC_SHARED=src-evashared
INC_LIBGCC=/opt/MeetiXOSProject/lib/gcc/i686-mx/4.9.1/include

# object dir
OBJ_SHARED=$BIN/obj-evashared
OBJ_EVALD=$BIN/obj-evaloader
OBJ_EVANG=$BIN/obj-evacore

# iso dir
ISO_SRC=../iso
ISO_TGT=../MeetiXOS.iso

# kernel install dir
ARTIFACT_EVALD=$ISO_SRC/boot/Evangelion_K/Eva_loader/EvaLoader
ARTIFACT_EVANG=$ISO_SRC/boot/Evangelion_K/Eva_core/EvangelionNG

# script dir
LINKSCRIPT_LOADER=extra/link-loader.ld
LINKSCRIPT_KERNEL=extra/link-kernel.ld

# ramdisk install dir
RAMDISK=$ISO_SRC/boot/MXfs/MXfs.img

# advanged package module
AP_STARTUP_SRC=src-ap/smpStart.asm
AP_STARTUP_OBJ=$BIN/smpstart.o
AP_STARTUP_TGT=../MXfs/sys/lib/Eva_expn/smpstart.eko

# compile flags
LDFLAGS="-nostdlib -nostartfiles"
CFLAGS="-std=c++11 -D_ARCH_X86_=1 -Wall -Wunused-variable -Wno-unused-but-set-variable -ffreestanding -fno-exceptions -fno-rtti"

# qemu and ramdisk writer
QEMU=qemu-system-x86_64
VBOX=virtualbox
RAMDISK_WRITER=ramdisk-writer

# ramdisk source
MXfs=../MXfs

##
# Cleans binary contents, 
##
function targetClean() 
{	
	headline "cleaning"
	remove $ARTIFACT_EVALD
	remove $ARTIFACT_EVANG
	remove $RAMDISK
	remove $ISO_TGT
	cleanDirectory $BIN
	cleanDirectory $OBJ_SHARED
	cleanDirectory $OBJ_EVALD
	cleanDirectory $OBJ_EVANG
	changes --clear
}

##
# Assembles the application processor startup object.
##
function targetCompileAPStartup() 
{
	headline "building Advanced Package module"

	nasm -f bin -o "$AP_STARTUP_OBJ" -s "$AP_STARTUP_SRC"
	failOnError
	mv "$AP_STARTUP_OBJ" "$AP_STARTUP_TGT"
	list $AP_STARTUP_OBJ
}

##
#	compile all file that it find
##
function targetCompile() 
{
	srcdir=$1
	objdir=$2
	includes=$3
	headline "compiling $srcdir"
	
	# check if headers have changed
	headersHaveChanged=1
	for file in $(find "$INC" -iname "*.hpp" -o -iname "*.h"); 
	do	
		changes -c $file
		if [ $? -eq 1 ]; then
			headersHaveChanged=1
		fi
		changes -s $file
	done
	
	# compile sources
	for file in $(find "$srcdir" -iname "*.cpp" -o -iname "*.c"); 
	do	
		changes -c $file
		changed=$?
		if ( [ $headersHaveChanged -eq 1 ] || [ $changed -eq 1 ] ); then
			out=`sourceToObject $file`
			list $out
			i686-mx-g++ -c $file -o "$objdir/$out" $includes $CFLAGS
			failOnError
			changes -s $file
		fi
	done

	# assemble sources
	for file in $(find "$srcdir" -iname "*.asm"); 
	do	
		changes -c $file
		changed=$?
		if ( [ $headersHaveChanged -eq 1 ] || [ $changed -eq 1 ] ); then
			out=`sourceToObject $file`
			list $out
			nasm -f elf -s $file -o "$objdir/$out"
			failOnError
			changes -s $file
		fi
	done
}

##
#	link object list
##
function targetLink() 
{
	artifact=$1
	script=$2
	objects=$3
	headline "linking $artifact"
	
	i686-mx-ld $LD_FLAGS -o $artifact -T $script $objects
	failOnError
}

##
#	create ramdisk swith ramdisk writer
##
function targetRamdisk() 
{
	headline "building ramdisk"
	$RAMDISK_WRITER "$MXfs" "$RAMDISK"
	failOnError
}

##
#	create iso with mkisofs
##
function targetMakeISO() 
{
	headline "making iso"
	mkisofs -R -b boot/grub/i386-pc/eltorito.img -c boot.catalog -no-emul-boot -boot-load-size 32 -boot-info-table -o $ISO_TGT $ISO_SRC 
	failOnError
}

##
#	run MeetiX OS on Qemu
##
function targetQemu() 
{
	$QEMU -enable-kvm -vga vmware -serial stdio -m 512M -cdrom $ISO_TGT
}

##
#	run MeetiX OS on Qemu with debug
##
function targetQemuDebugGdb() 
{
	$QEMU -cdrom $ISO_TGT -s -S -m 1024 -serial tcp::1234,server
}

##
#	exec all target
##
function targetAll() 
{
	targetClean
	targetCompileAPStartup
	targetCompile $SRC_SHARED $OBJ_SHARED "-I$INC -I$INC_LIBGCC -I$SRC_SHARED"
	targetCompile $SRC_EVALD $OBJ_EVALD "-I$INC -I$INC_LIBGCC -I$SRC_SHARED -I$SRC_EVALD"
	targetCompile $SRC_EVANG $OBJ_EVANG "-I$INC -I$INC_LIBGCC -I$SRC_SHARED -I$SRC_EVANG"
	targetLink $ARTIFACT_EVALD $LINKSCRIPT_LOADER "$OBJ_EVALD/* $OBJ_SHARED/*"
	targetLink $ARTIFACT_EVANG $LINKSCRIPT_KERNEL "$OBJ_EVANG/* $OBJ_SHARED/*"
	targetRamdisk
	targetMakeISO
}

##
#	compile and link Evangelion kernel
##
function targetEva()
{
	targetClean
	targetCompileAPStartup
	targetCompile $SRC_SHARED $OBJ_SHARED "-I$INC -I$INC_LIBGCC -I$SRC_SHARED"
	targetCompile $SRC_EVALD $OBJ_EVALD "-I$INC -I$INC_LIBGCC -I$SRC_SHARED -I$SRC_EVALD"
	targetCompile $SRC_EVANG $OBJ_EVANG "-I$INC -I$INC_LIBGCC -I$SRC_SHARED -I$SRC_EVANG"
	targetLink $ARTIFACT_EVALD $LINKSCRIPT_LOADER "$OBJ_EVALD/* $OBJ_SHARED/*"
	targetLink $ARTIFACT_EVANG $LINKSCRIPT_KERNEL "$OBJ_EVANG/* $OBJ_SHARED/*"
}

##
#	create ramdisk and iso
##
function targetRepack() 
{
	targetRamdisk
	targetMakeISO
}

##
#	run MeetiX OS on virtualBox
##
function targetVbox()
{
	$VBOX --startvm MeetiX-OS
}

# print selected option
targetHeadline $TARGET

# execute targets
if [[ $TARGET == "all" ]]; then
	targetAll
	targetVbox

elif [[ $TARGET == "repack" ]]; then
	targetRepack
	targetQemu
	clear

elif [[ $TARGET == "eva" ]]; then
	targetEva

elif [[ $TARGET == "ramdisk" ]]; then
	targetRamdisk
	clear
	
elif [[ $TARGET == "vbox" ]]; then
	targetVbox

elif [[ $TARGET == "qemu" ]]; then
	targetQemu
	
elif [[ $TARGET == "qemu-debug-gdb" ]]; then
	targetQemuDebugGdb
	
elif [[ $TARGET == "clean" ]]; then
	targetClean
	
else
	echo "unknown target: '$TARGET'"
	exit 1
fi

exit 0
 
