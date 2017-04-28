#!/bin/bash

option=$1

destDir=/opt/MeetiXOSProject/bin

if [[ $option == "install" ]];
then
	ln -sf $destDir/changes /bin/
	ln -sf $destDir/i686-mx-addr2line /bin/
	ln -sf $destDir/i686-mx-ar /bin/
	ln -sf $destDir/i686-mx-as /bin/
	ln -sf $destDir/i686-mx-c++ /bin/
	ln -sf $destDir/i686-mx-c++filt /bin/
	ln -sf $destDir/i686-mx-cpp /bin/
	ln -sf $destDir/i686-mx-elfedit /bin/
	ln -sf $destDir/i686-mx-g++ /bin/
	ln -sf $destDir/i686-mx-gcc /bin/
	ln -sf $destDir/i686-mx-gcc-4.9.1 /bin/
	ln -sf $destDir/i686-mx-gcc-ar /bin/
	ln -sf $destDir/i686-mx-gcc-nm /bin/
	ln -sf $destDir/i686-mx-gcc-ranlib /bin/
	ln -sf $destDir/i686-mx-gcov /bin/
	ln -sf $destDir/i686-mx-gprof /bin/
	ln -sf $destDir/i686-mx-ld /bin/
	ln -sf $destDir/i686-mx-ld.bfd /bin/
	ln -sf $destDir/i686-mx-nm /bin/
	ln -sf $destDir/i686-mx-objcopy /bin/
	ln -sf $destDir/i686-mx-objdump /bin/
	ln -sf $destDir/i686-mx-ranlib /bin/
	ln -sf $destDir/i686-mx-readelf /bin/
	ln -sf $destDir/i686-mx-size /bin/
	ln -sf $destDir/i686-mx-strings /bin/
	ln -sf $destDir/i686-mx-strip /bin/
	ln -sf $destDir/mx-pkg-config.sh /bin/
	ln -sf $destDir/ramdisk-writer /bin/

elif [[ $option == "uninstall" ]];
then
	rm /bin/changes
	rm /bin/i686-mx-addr2line
	rm /bin/i686-mx-ar
	rm /bin/i686-mx-as
	rm /bin/i686-mx-c++
	rm /bin/i686-mx-c++filt
	rm /bin/i686-mx-cpp
	rm /bin/i686-mx-elfedit
	rm /bin/i686-mx-g++
	rm /bin/i686-mx-gcc
	rm /bin/i686-mx-gcc-4.9.1
	rm /bin/i686-mx-gcc-ar
	rm /bin/i686-mx-gcc-nm
	rm /bin/i686-mx-gcc-ranlib
	rm /bin/i686-mx-gcov
	rm /bin/i686-mx-gprof
	rm /bin/i686-mx-ld
	rm /bin/i686-mx-ld.bfd
	rm /bin/i686-mx-nm
	rm /bin/i686-mx-objcopy
	rm /bin/i686-mx-objdump
	rm /bin/i686-mx-ranlib
	rm /bin/i686-mx-readelf
	rm /bin/i686-mx-size
	rm /bin/i686-mx-strings
	rm /bin/i686-mx-strip
	rm /bin/mx-pkg-config.sh
	rm /bin/ramdisk-writer
fi