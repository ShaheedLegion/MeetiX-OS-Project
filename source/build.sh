#/bin/bash

##################################################################################
# MeetiX OS By MeetiX OS Project [Marco Cicognani]           					 #
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

# copy first argument to local variable
target=$1

##
#	call compile of application directory
##
function userspace()
{
	cd "userspace"
	bash compile.sh $1
	cd ..
}

##
#	call builder of Evangelion directory
##
function kernel()
{
	cd "Evangelion"
	bash build.sh $1
	cd ..
}

##
# return language for new Project
##
function getLanguage()
{
	# copy the argument
	language=$1

	# return only if the provided language is supported
	if [[ $language == "c" || $language == "c++" || $language == "js" ]]; then echo "$language"
	else echo "${RED}Provided an unknow language name${RESET}"
		exit 1
	fi
}

##
# return type for new project
##
function getType()
{
	if [[ $type == "app" ]]; then echo "application"
	elif [[ $type == "cmd" ]]; then echo "command"
	elif [[ $type == "srv" ]]; then echo "server"
	fi
}

##
# create dirs
##
function create()
{
	# create the fundamentals dir for project
	mkdir $1
	mkdir $1"/obj"
	mkdir $1"/src"

	echo "$1"
}

##
# create application directory for new Project
##
function createApp()
{
	# create path
	local dirName="userspace/applications/"$1

	# create the dirs on the ramdisk directory
	mkdir "MXfs/app/"$1
	mkdir "MXfs/app/"$1"/bin"
	mkdir "MXfs/app/"$1"/deps"
	mkdir "MXfs/app/"$1"/deps/icon"

	# return the created path
	echo $(create $dirName)
}

##
# create command directory for new Project
##
function createCommand()
{
	# create path
	local dirName="userspace/commands/"$1

	# return the created path
	echo $(create $dirName)
}

##
# create server directory for new Project
##
function createServer()
{
	# create path
	local dirName="userspace/servers/"$1

	# return the created path
	echo $(create $dirName)
}

##
# create the project with provided type
##
function switchProjectType()
{
	# copy arguments
	type=$1
	name=$2

	if [[ $type == "app" ]]; then echo $(createApp $name)
	elif [[ $type == "cmd" ]]; then echo $(createCommand $name)
	elif [[ $type == "srv" ]]; then echo $(createServer $name)
	else echo ""
	fi
}

##
# create the build.sh for the new project
##
function createScript()
{
	# copy arguments
	dir=$1
	name=$2
	type=$(getType $3)
	language=$(getLanguage $4)
	builddir=$(pwd)

	# go to new project directory
	cd $dir
	cat > "build.sh" <<EOF
#!/bin/sh
. /opt/MeetiXOSProject/MXbase.sh

# Define build setup
SRC="src"
INC="src"
OBJ="obj"
ARTIFACT_NAME="$name"
ARTIFACT_TYPE="$type"
COMPILER="i686-mx-$language"
CFLAGS="-std=${language}11 -I$SRC"
LDFLAGS=""

# Include build tasks
. ../../builder.sh
EOF

	return 0
}

##
#  create new developer Project
##
function createNew()
{
	echo "${GREEN}MeetiX Project Creator script${RESET}"

	# test if there are e arguments Provided
	if [ $# -eq 3 ];
	then
		# copy to local variables the arguments
		local type=$1
		local name=$2
		local language=$3

		# save the created dir
		local createdDir=$(switchProjectType $type $name)

		# call type switcher
		if [[ ! -z $createdDir ]]
		then
			# create the script and the main files for the project
			if [[ $(createScript $createdDir $name $type $language) -eq 0 ]];
			then
				echo "${GREEN}Succesful created new project of type $type, named $name with language $language${RESET}"

			# show error if script creation fails
			else
				echo "${RED}Provided an unknow type of language${RESET}"
				exit 1
			fi

		# show error if switch type fails
		else
			echo "${RED}Provided an unknow type of project${RESET}"
			exit 1
		fi

	# show help
	else
		echo "Usage:"
		echo "${RED}bash build.sh new [app/cmd/srv] [name of project] [c/c++/js]${RESET}"
		echo ""

	fi
}

# switch arguments
if [[ $target == "user" ]]; then
	userspace "all"

elif [[ $target == "new" ]]; then
	createNew $2 $3 $4

elif [[ $target == "cleanuser" ]]; then
	userspace "clean"

else
	kernel $target
fi
