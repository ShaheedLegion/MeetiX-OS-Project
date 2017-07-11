#!/bin/bash

# colors
RED=`tput setaf 1`
GREEN=`tput setaf 2`
CYAN=`tput setaf 6`
RESET=`tput sgr0`

# Make sure only root can run our script
if [ "$(id -u)" != "0" ]; then
   echo "${RED}This script can only run as root${RESET}"
   exit 1
fi

# get from system cpu core number
if [ -f /proc/cpuinfo ]; then
	cpus=`cat /proc/cpuinfo | grep '^processor[[:space:]]*:' | wc -l`
else
	cpus=1
fi

# show cpu for build
echo "${RED}Building Toolchains with "$cpus" cpus${RESET}"
MULTITHREAD="-j"$cpus

# function to check install of provided tool
function requireTool()
{
	if ! type "$1" >/dev/null 2>&1
	then
		>&2 echo "${RED}error: missing required tool '$1'${RESET}"
		exit 1
	fi
}

#exit from script if previous command fail
function failOnError()
{
	if [[ $? != 0 ]]; then
		echo "${RED}Build failed${RESET}"
		exit 1
	fi
}

# override a variable
function with()
{
    if [ -z "${!1}" ];
    then
        eval $1='$2';
    fi
}

# ovverride function for pushd and popd
function pushd()
{
    command pushd "$@" > /dev/null
}
function popd()
{
    command popd "$@" > /dev/null
}

# creating dirs
TOOLCHAIN_BASE="/opt/MeetiXOSProject"
SYSROOT=$TOOLCHAIN_BASE"/MXlibraries"
SYSROOT_BIN=$SYSROOT/bin
SYSROOT_INCLUDE=$SYSROOT/include
SYSROOT_LIB=$SYSROOT/lib

# define archives names
GCC_ARCHIVE="https://ghostkernel.org/repository/gcc/gcc-4.9.1.tar.gz"
GCC_PATCH="i686/gcc-4.9.1.patch"
GCC_UNPACKED="gcc-4.9.1"

# define archives names
BINUTILS_ARCHIVE="https://ghostkernel.org/repository/binutils/binutils-2.24.tar.gz"
BINUTILS_PATCH="i686/binutils-2.24.patch"
BINUTILS_UNPACKED="binutils-2.24"

# define archives names
AUTOMAKE_ARCHIVE="https://ftp.gnu.org/gnu/automake/automake-1.11.1.tar.bz2"
AUTOMAKE_UNPACKED="automake-1.11.1"
AUTOMAKE=""

# define archives names
AUTOCONF_ARCHIVE="https://ftp.gnu.org/gnu/autoconf/autoconf-2.64.tar.bz2"
AUTOCONF_UNPACKED="autoconf-2.64"
AUTOCONF=""

# system target
TARGET="i686-mx"

# step of build
STEP_CLEAN=1
STEP_DOWNLOAD=1
STEP_UNPACK=1
STEP_PATCH=1

# builds steps
STEP_BUILD_AUTOMAKE=1
STEP_BUILD_AUTOCONF=1
STEP_BUILD_BINUTILS=1
STEP_BUILD_GCC=1
STEP_BUILD_PORTS=1

# Parse parameters
for var in "$@"; do
	if [ $var == "--skip-download" ]; then STEP_DOWNLOAD=0
	elif [ $var == "--skip-unpack" ]; then STEP_UNPACK=0
	elif [ $var == "--skip-patch" ]; then STEP_PATCH=0
	elif [ $var == "--skip-build-automake"]; then STEP_BUILD_AUTOMAKE=0
	elif [ $var == "--skip-build-autoconf"]; then STEP_BUILD_AUTOCONF=0
	elif [ $var == "--skip-build-binutils" ]; then STEP_BUILD_BINUTILS=0
	elif [ $var == "--skip-build-gcc" ]; then STEP_BUILD_GCC=0
	elif [ $var == "--clean" ]; then STEP_CLEAN=1
	else echo "${RED}unknown parameter: $var${RESET}"
		exit 1
	fi
done

# clean if necessary
if [ $STEP_CLEAN == 1 ]; then
	echo "[${GREEN}Cleaning build directory${RESET}]"
    rm -rf build
fi

# download if necessary
if [ $STEP_DOWNLOAD == 1 ];
then
    echo "[${CYAN}Downloading $AUTOMAKE_UNPACKED${RESET}]"
    curl $AUTOMAKE_ARCHIVE -o i686/automake-1.11.1.tar.bz2 -k

    echo "[${CYAN}Downloading $AUTOCONF_UNPACKED${RESET}]"
    curl $AUTOCONF_ARCHIVE -o i686/autoconf-2.64.tar.bz2 -k

	echo "[${CYAN}Downloading $GCC_UNPACKED${RESET}]"
	curl $GCC_ARCHIVE -o i686/gcc.tar.gz -k

	echo "[${CYAN}Downloading $BINUTILS_UNPACKED${RESET}]"
	curl $BINUTILS_ARCHIVE -o i686/binutils.tar.gz -k

else
	echo "[${CYAN}Skipping file download${RESET}]"
fi

echo "[${GREEN}Checking Tools${RESET}]"
with HOST_CXX	g++
requireTool patch
requireTool curl

echo "[${GREEN}Creating build directory${RESET}]"
mkdir -p build

# compile a custom version of automake without installing on system
if [ $STEP_BUILD_AUTOMAKE == 1 ];
then
    echo "[${GREEN}Building automake 1.11.1${RESET}]"
    AUTOMAKE_PREFIX=`pwd`"/build/atmk"

    echo "    ${CYAN}Unpacking${RESET}"
    tar -xf i686/automake-1.11.1.tar.bz2 -C build

    echo "    ${CYAN}Configuring${RESET}"
    pushd build/$AUTOMAKE_UNPACKED
    ./configure --prefix=$AUTOMAKE_PREFIX >>MeetiXBuild.log 2>&1
    failOnError

    echo "    ${RED}Compiling${RESET}"
    make $MULTITHREAD >>MeetiXBuild.log 2>&1
    failOnError

    echo "    ${RED}installing${RESET}"
    make install >>MeetiXBuild.log 2>&1
    failOnError

    popd
    export AUTOMAKE=`pwd`"/build/atmk/bin/automake-1.11"
fi

# compile a custom version of autoconf without installing on system
if [ $STEP_BUILD_AUTOCONF == 1 ];
then
    echo "[${GREEN}Building autoconf 2.64${RESET}]"
    AUTOCONF_PREFIX=`pwd`"/build/atcf"

    echo "    ${CYAN}Unpacking${RESET}"
    tar -xf i686/autoconf-2.64.tar.bz2 -C build

    echo "    ${CYAN}Configuring${RESET}"
    pushd build/$AUTOCONF_UNPACKED
    ./configure --prefix=$AUTOCONF_PREFIX >>MeetiXBuild.log 2>&1
    failOnError

    echo "    ${RED}Compiling${RESET}"
    make $MULTITHREAD >>MeetiXBuild.log 2>&1
    failOnError

    echo "    ${RED}installing${RESET}"
    make install >>MeetiXBuild.log 2>&1
    failOnError

    popd
    export AUTOCONF=`pwd`"/build/atcf/bin/autoconf"
fi

# Print some information and create devel directory
echo "[${GREEN}Toolchain will be installed to: $TOOLCHAIN_BASE${RESET}]"
mkdir -p $TOOLCHAIN_BASE
mkdir -p $SYSROOT
mkdir -p $SYSROOT_BIN
mkdir -p $SYSROOT_INCLUDE
mkdir -p $SYSROOT_LIB

# Clean
echo "[${GREEN}Cleaning automake${RESET}]"
rm -rf build/automake-1.11.1
failOnError

echo "[${GREEN}Cleaning autoconf${RESET}]"
rm -rf build/autoconf-2.64
failOnError


# Unpack archives
if [ $STEP_UNPACK == 1 ];
then
	echo "[${CYAN}Unpacking $GCC_UNPACKED${RESET}]"
	tar -xf i686/gcc.tar.gz -C build
	failOnError

	echo "[${CYAN}Unpacking $BINUTILS_UNPACKED${RESET}]"
	tar -xf i686/binutils.tar.gz -C build
	failOnError

    echo "[${CYAN}Unpacking libraries${RESET}]"
    tar -xf i686/libs.tar.gz -C $TOOLCHAIN_BASE
    failOnError

else
	echo "[${CYAN}Skipping unpacking${RESET}]"
fi


# Apply patches
if [ $STEP_PATCH == 1 ];
then
	echo "[${GREEN}Patching $GCC_UNPACKED${RESET}]"
	patch -d build/$GCC_UNPACKED -p 1 < $GCC_PATCH	>>/dev/null 2>&1
	pushd build/$GCC_UNPACKED/libstdc++-v3
	echo "[${GREEN}Updating autoconf in libstdc++-v3${RESET}]"
	$AUTOCONF >>MeetiXBuild.log 2>&1
	popd

	echo "[${GREEN}Patching $BINUTILS_UNPACKED${RESET}]"
	patch -d build/$BINUTILS_UNPACKED -p 1 < $BINUTILS_PATCH		>>/dev/null 2>&1
	pushd build/$BINUTILS_UNPACKED/ld
	echo "[${GREEN}Updating automake for ld${RESET}]"
	$AUTOMAKE >>MeetiXBuild.log 2>&1
	popd

else
    echo "[${CYAN}Skipping patching${RESET}]"
fi

# Build tools
echo "[${RED}Building 'changes' tool${RESET}]"
pushd $TOOLCHAIN_BASE/tools/changes
CC=$HOST_CXX bash build.sh all		>>MeetiXBuild.log 2>&1
ln -sf $TOOLCHAIN_BASE/bin/changes /bin/
popd

echo "[${RED}Building ramdisk-writer' tool${RESET}]"
pushd $TOOLCHAIN_BASE/tools/ramdisk-writer
CC=$HOST_CXX bash build.sh all		>>MeetiXBuild.log 2>&1
ln -sf $TOOLCHAIN_BASE/bin/ramdisk-writer /bin/
popd

echo "[${RED}Installing 'pkg-config' wrapper${RESET}]"
pushd $TOOLCHAIN_BASE/tools/pkg-config
bash build.sh						>>MeetiXBuild.log 2>&1
ln -sf $TOOLCHAIN_BASE/bin/mx-pkg-config.sh /bin/
popd


# Install headers
echo "[${RED}Installing libc and libapi headers${RESET}]"
pushd $TOOLCHAIN_BASE/libc
bash build.sh install-headers	>>MeetiXBuild.log 2>&1
popd

KERNEL_INC=`pwd`"/../source/Evangelion/inc"
pushd $TOOLCHAIN_BASE/libapi
bash build.sh install-headers $KERNEL_INC	>>MeetiXBuild.log 2>&1
popd

# Build binutils
if [ $STEP_BUILD_BINUTILS == 1 ];
then
	echo "[${GREEN}Building Binutils${RESET}]"
	mkdir -p build/build-binutils
	pushd build/build-binutils

	echo "     ${RED}Configuring${RESET}"
	../$BINUTILS_UNPACKED/configure --target=$TARGET --prefix=$TOOLCHAIN_BASE --disable-nls  --disable-werror --with-sysroot=$SYSROOT >>MeetiXBuild.log 2>&1
	failOnError

	echo "     ${RED}Building${RESET}"
	make all $MULTITHREAD			   >>MeetiXBuild.log 2>&1
	failOnError

	echo "     ${RED}Installing${RESET}"
	make install						>>MeetiXBuild.log 2>&1
	failOnError

	popd

else echo "${CYAN}Skipping build of Binutils${RESET}"
fi


# Build GCC
if [ $STEP_BUILD_GCC == 1 ];
then
	echo "[${GREEN}Building GCC${RESET}]"
	mkdir -p build/build-gcc
	pushd build/build-gcc

	echo "     ${RED}Configuring${RESET}"
	../$GCC_UNPACKED/configure --target=$TARGET --prefix=$TOOLCHAIN_BASE --enable-languages=c,c++ --with-sysroot=$SYSROOT >>MeetiXBuild.log 2>&1
	failOnError

	echo "     ${RED}Building core${RESET}"
	make all-gcc $MULTITHREAD			>>MeetiXBuild.log 2>&1
	failOnError

	echo "     ${RED}Installing core${RESET}"
	make install-gcc					>>MeetiXBuild.log 2>&1
	failOnError

    echo "     ${RED}Building target libgcc${RESET}"
	make all-target-libgcc $MULTITHREAD	>>MeetiXBuild.log 2>&1
	failOnError

    echo "     ${RED}Installing target libgcc${RESET}"
	make install-target-libgcc			>>MeetiXBuild.log 2>&1
	failOnError
	popd

else echo "${CYAN}Skipping build of GCC${RESET}"
fi

#creating links
echo "[${GREEN}Creating Link to /bin/ directory${RESET}]"
for file in $TOOLCHAIN_BASE/bin/*;
do
    ln -sf $file /bin/
done

# Build libc
echo "[${RED}Building libc${RESET}]"
pushd $TOOLCHAIN_BASE/libc
bash build.sh all						>>MeetiXBuild.log 2>&1
failOnError
popd

# Build libapi
echo "[${RED}Building libapi${RESET}]"
pushd $TOOLCHAIN_BASE/libapi
bash build.sh all KERNEL_INC			>>MeetiXBuild.log 2>&1
failOnError
popd

# Build libstdc++-v3
echo "[${RED}Building libstdc++-v3${RESET}]"
pushd build/build-gcc
make all-target-libstdc++-v3 $MULTITHREAD	>>MeetiXBuild.log 2>&1
failOnError

echo "[${RED}Installing libstdc++-v3${RESET}]"
make install-target-libstdc++-v3	>>MeetiXBuild.log 2>&1
failOnError
popd

# build cross port libs
if [ $STEP_BUILD_PORTS == 1 ];
then
    echo "[${GREEN}Building ports${RESET}]"
    pushd $TOOLCHAIN_BASE/ports

    echo "    [${RED}Building zlib${RESET}]"
    bash port.sh zlib/ >>MeetiXBuild.log 2>&1
    failOnError

    echo "    [${RED}Building pixman${RESET}]"
    bash port.sh pixman/ >>MeetiXBuild.log 2>&1
    failOnError

    echo "    [${RED}Building libpng${RESET}]"
    bash port.sh libpng/ >>MeetiXBuild.log 2>&1
    failOnError

    echo "    [${RED}Building freetype${RESET}]"
    bash port.sh freetype/ >>MeetiXBuild.log 2>&1
    failOnError

    echo "    [${RED}Building cairo${RESET}]"
    bash port.sh cairo/ >>MeetiXBuild.log 2>&1
    failOnError

    popd

else echo "${CYAN}Skipping ports build${RESET}"
fi

# build mxuser
echo "[${RED}Building mxuser${RESET}]"
pushd $TOOLCHAIN_BASE/libuser
bash build.sh all              >>MeetiXBuild.log 2>&1
failOnError
popd

# clean
echo "[${RED}Cleaning${RESET}]"
rm -rf build
rm -rf $TOOLCHAIN_BASE/ports/build

# Finished
echo "${GREEN}Toolchain successfully built${RESET}"
