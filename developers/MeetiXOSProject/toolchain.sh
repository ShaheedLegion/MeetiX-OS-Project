#!/bin/bash
. MXbase.sh

# Define globals
GCC_ARCHIVE=https://ghostkernel.org/repository/gcc/gcc-4.9.1.tar.gz
GCC_PATCH=patches/toolchain/gcc-4.9.1-meetix-1.4.patch
GCC_UNPACKED=gcc-4.9.1

BINUTILS_ARCHIVE=https://ghostkernel.org/repository/binutils/binutils-2.24.tar.gz
BINUTILS_PATCH=patches/toolchain/binutils-2.24-meetix-1.2.patch
BINUTILS_UNPACKED=binutils-2.24

REQUIRED_AUTOMAKE="automake (GNU automake) 1.11"
REQUIRED_AUTOCONF="autoconf (GNU Autoconf) 2.64"

TARGET=i686-mx

STEP_DOWNLOAD=1
STEP_CLEAN=1
STEP_UNPACK=1
STEP_PATCH=1

STEP_BUILD_BINUTILS=1
STEP_BUILD_GCC=1


# Set defaults for variables
with AUTOMAKE	automake
with AUTOCONF	autoconf
with HOST_CXX	g++

#estract developer tar
tar -zxvf MeetiXOSProject.tar.gz

# Parse parameters
for var in "$@"; do
	if [ $var == "--skip-download" ]; then
		STEP_DOWNLOAD=0
	elif [ $var == "--skip-unpack" ]; then
		STEP_UNPACK=0
	elif [ $var == "--skip-patch" ]; then
		STEP_PATCH=0
	elif [ $var == "--skip-build-binutils" ]; then
		STEP_BUILD_BINUTILS=0
	elif [ $var == "--skip-build-gcc" ]; then
		STEP_BUILD_GCC=0
	elif [ $var == "--clean" ]; then
		STEP_CLEAN=1
	else
		headline "unknown parameter: $var"
		exit
	fi
done

function pushd() 
{
    command pushd "$@" > /dev/null
}

function popd() 
{
    command popd "$@" > /dev/null
}


headline "Checking tools"
requireTool patch
requireTool curl
requireTool $AUTOMAKE
requireTool $AUTOCONF


# Additional build config
BUILD_GCC_ADDITIONAL_FLAGS=""
case "$(uname -s)" in CYGWIN*|MINGW32*|MSYS*)
		headline "Preparing for build on Cygwin"
		BUILD_GCC_ADDITIONAL_FLAGS="--with-gmp=/usr/local --with-mpc=/usr/local --with-mpfr=/usr/local"
		;;
esac



# Check version of automake/autoconf
echo "    $REQUIRED_AUTOMAKE"
$AUTOMAKE --version | grep -q "$REQUIRED_AUTOMAKE"
if [[ $? != 0 ]]; then
	echo "    -> wrong automake version:" 
	echo "       $($AUTOMAKE --version | sed -n 1p)"
	exit
fi

echo "    $REQUIRED_AUTOCONF"
$AUTOCONF --version | grep -q "$REQUIRED_AUTOCONF"
if [[ $? != 0 ]]; then
	echo "    -> wrong autoconf version:"
	echo "       $($AUTOCONF --version | sed -n 1p)"
	exit
fi


# Print some information
headline "Toolchain will be installed to: $TOOLCHAIN_BASE"

# Clean if necessary
if [ $STEP_CLEAN == 1 ]; then
	headline "Cleaning "$GCC_UNPACKED
	sudo rm -rf $GCC_UNPACKED

	headline "Cleaning "$BINUTILS_UNPACKED
	sudo rm -rf $BINUTILS_UNPACKED
fi


# Create temp
headline "Creating temporary work directory"
mkdir -p temp


# Download archives if necessary
if [ $STEP_DOWNLOAD == 1 ]; then

	headline "Downloading archive files"
	echo "    gcc"
	curl $GCC_ARCHIVE -o temp/gcc.tar.gz -k
	echo "    binutils"
	curl $BINUTILS_ARCHIVE -o temp/binutils.tar.gz -k
	
else
	headline "Skipping file download"
fi


# Unpack archives
if [ $STEP_UNPACK == 1 ]; then

	headline "Unpacking archives"
	tar -xf temp/gcc.tar.gz -C temp
	failOnError
	tar -xf temp/binutils.tar.gz -C temp
	failOnError
	
else
	headline "Skipping unpacking"
fi


# Apply patches
if [ $STEP_PATCH == 1 ]; then

	headline "Patching GCC"
	patch -d temp/$GCC_UNPACKED -p 1 < $GCC_PATCH				>>temp/patching.log 2>&1
	pushd temp/$GCC_UNPACKED/libstdc++-v3
	headline "Updating autoconf in libstdc++-v3"
	$AUTOCONF
	popd
	
	headline "Patching binutils"
	patch -d temp/$BINUTILS_UNPACKED -p 1 < $BINUTILS_PATCH		>>temp/patching.log 2>&1
	pushd temp/$BINUTILS_UNPACKED/ld
	headline "Updating automake for ld"
	$AUTOMAKE
	popd
	
else
	headline "Skipping patching"
fi


# Build tools
headline "Building 'changes' tool"
pushd tools/changes
CC=$HOST_CXX bash build.sh all		>>MeetiXBuild.log 2>&1
popd

headline "Building 'ramdisk-writer' tool"
pushd tools/ramdisk-writer
CC=$HOST_CXX bash build.sh all		>>MeetiXBuild.log 2>&1
popd

headline "Installing 'pkg-config' wrapper"
pushd tools/pkg-config
bash build.sh						>>MeetiXBuild.log 2>&1
popd


# Install headers
headline "Installing libc and libapi headers"
pushd libc
bash build.sh install-headers	>>MeetiXBuild.log 2>&1
popd

pushd libapi
bash build.sh install-headers	>>MeetiXBuild.log 2>&1
popd


# Add bin folder to PATH
PATH=$PATH:$TOOLCHAIN_BASE/bin


# Build binutils
if [ $STEP_BUILD_BINUTILS == 1 ]; then

	headline "Building binutils"
	mkdir -p temp/build-binutils
	pushd temp/build-binutils

	echo "    Configuring"
	../$BINUTILS_UNPACKED/configure --target=$TARGET --prefix=$TOOLCHAIN_BASE --disable-werror --with-sysroot=$SYSROOT >>MeetiXBuild.log 2>&1
	failOnError

	echo "    Building"
	make all -j8						>>MeetiXBuild.log 2>&1
	failOnError

	echo "    Installing"
	make install						>>MeetiXBuild.log 2>&1
	failOnError

	popd

else headline "Skipping build of binutils"
fi


# Build GCC
if [ $STEP_BUILD_GCC == 1 ]; then

	headline "Building GCC"
	mkdir -p temp/build-gcc
	pushd temp/build-gcc
	
	echo "    Configuration"
	../$GCC_UNPACKED/configure --target=$TARGET --prefix=$TOOLCHAIN_BASE --enable-languages=c,c++ --with-sysroot=$SYSROOT $BUILD_GCC_ADDITIONAL_FLAGS >>MeetiXBuild.log 2>&1
	failOnError

	echo "    Building core"
	sudo make all-gcc -j8					>>MeetiXBuild.log 2>&1
	failOnError

	echo "    Installing core"
	sudo make install-gcc					>>MeetiXBuild.log 2>&1
	failOnError

	echo "    Building target libgcc"
	sudo make all-target-libgcc -j8			>>MeetiXBuild.log 2>&1
	failOnError

	echo "    Installing target libgcc"
	sudo make install-target-libgcc			>>MeetiXBuild.log 2>&1
	failOnError
	popd

else headline "Skipping build of GCC"
fi

headline "Creating Link to /bin/ directory"
. install.sh

# Build libc
headline "Building libc"
pushd libc
bash build.sh all						>>MeetiXBuild.log 2>&1
failOnError
popd

# Build libapi
headline "Building libapi"
pushd libapi
bash build.sh all						>>MeetiXBuild.log 2>&1
failOnError
popd


# Build libstdc++-v3
headline "Building libstdc++-v3"
pushd temp/build-gcc

echo "    Building libstdc++-v3"
make all-target-libstdc++-v3 -j8	>>MeetiXBuild.log 2>&1
failOnError

echo "    Installing libstdc++-v3"
make install-target-libstdc++-v3	>>MeetiXBuild.log 2>&1
failOnError
popd


# Finished
headline "Toolchain successfully built"
