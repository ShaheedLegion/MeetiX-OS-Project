/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
*         DERIVED FROM THE GHOST OPERATING SYSTEM                                     *
*         This software is derived from the Ghost operating system project,           *
*         written by Max Schlüssel <lokoxe@gmail.com>. Copyright 2012-2017            *
*         https://ghostkernel.org/                                                    *
*         https://github.com/maxdev1/ghost                                            *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#include "spawner.hpp"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <eva/bytewise.h>
#include <mxuser/utils/logger.hpp>
#include <mxuser/utils/environment.hpp>
#include <mxuser/io/files/futils.hpp>

#include "Elf32Loader.hpp"
#include "power.hpp"

/**
 *
 */
int main(int argc, char *argv[])
{
	// register this task as the system spawner
	if (TaskGetID(SPAWNER_IDENTIFIER) == -1)
	{
		klog("failed to initialize spawner service: could not register with identifier %s", SPAWNER_IDENTIFIER);
		return -1;
	}

	// initialize the system userspace
	init();

	// main loop of spawner
	receiveRequests();
}

/**
 *	create environment file
 */
void createOsEnvironmentFile(const char *dir, const char *fileName)
{
	//open directory etc
	SetWorkingDirectory(dir);

	// define first variables
	const char envvar[] = {
		"HOSTNAME=MeetiX_OS\nVERSION=0.7\nSYSTEM_LEVEL=basic\nPATH=/cmd/:/app/:/sys/eva/server/\nTHEME=multi\n"
	};
	int length = strlen(envvar);

	// create the environment file
	Fd env = OpenF(fileName, O_CREAT | O_READ | O_WRITE);

	// write variables on file and close it
	Write(env, envvar, length);
	Close(env);
}

/**
 *	create log file
 */
void createOsLogFile(const char *dir, const char *fileName)
{
	//open directory etc
	SetWorkingDirectory(dir);

	const char loginput[] = {
		"[0] log file created\n"
	};
	int length = strlen(loginput);

	// create the log file
	//Fd logfile = OpenF(fileName, O_CREAT | O_READ | O_WRITE);

	// write variables on file and close it
	//Write(logfile, loginput, length);
	//Close(logfile);
}

/**
 *
 */
void init()
{
	// let the spawner load shell
	Pid mxPid;
	SpawnStatus stat = spawn("/cmd/mx", "-s inittab.sh", "/cfg/init/bsp", SECURITY_LEVEL_KERNEL, 0, &mxPid, 0, 0, 0, 0, 0, 0);

	if (stat == SPAWN_STATUS_SUCCESSFUL)
	{
		klog("mx shell executed in process %d", mxPid);
		klog("Creating environment variables file");
		createOsEnvironmentFile("/cfg/environ/", "vars");

		klog("Creating log file");
		createOsLogFile("/sys/log/", "log");
	}

	else klog("failed to load mx shell from '/bin/mx' with code %d", stat);
}

/**
 *
 */
void receiveRequests()
{
	// service ready
	klog("spawner server ready");

	// defining size for messages
	const size_t requestLenMax = sizeof(MessageHeader) + sizeof(SpawnCommandSpawnRequest) + 1024;

	while (true)
	{
		// creating buffer for message
		uint8_t requestBuffer[requestLenMax];

		// receive incoming request
		MessageReceiveStatus stat = ReceiveMessage(requestBuffer, requestLenMax);

		if (stat != MESSAGE_RECEIVE_STATUS_SUCCESSFUL) protocolError("receiving command failed with code %i", stat);

		MessageHeader *header = (MessageHeader*) requestBuffer;
		SpawnCommandHeader *commandHeader = (SpawnCommandHeader*) MESSAGE_CONTENT(header);

		if (commandHeader->command == SPAWN_COMMAND_SPAWN_REQUEST) processSpawnRequest((SpawnCommandSpawnRequest*) commandHeader, header->sender, header->transaction);
		else if (commandHeader->command == SPAWN_COMMAND_SHUTDOWN_MACHINE || commandHeader->command == SPAWN_COMMAND_REBOOT_MACHINE) processHealtMachine(commandHeader->command);
		else protocolError("received unknown command: code %i, task %i", commandHeader->command, header->sender);
	}
}

/**
 *
 */
void protocolError(std::string msg, ...)
{
	va_list va;
	va_start(va, msg);
	kvlog(("protocol error: " + msg).c_str(), va);
	va_end(va);
}

/**
 *
 */
void processSpawnRequest(SpawnCommandSpawnRequest *request, Tid requester, MessageTransaction tx)
{
	SecurityLevel secLvl = request->securityLevel;

	const char *pos = (const char*) request;
	pos = pos + sizeof(SpawnCommandSpawnRequest);

	const char *path = pos;
	pos = pos + request->pathBytes;

	const char *args = pos;
	pos = pos + request->argsBytes;

	const char *workdir = pos;

	// parameters ready, perform spawn
	Pid oPid;
	Fd oFdInw;
	Fd oFdOutr;
	Fd oFdErr;
	Pid requesterPid = GetPidForTid(requester);
	SpawnStatus spawnStatus = spawn(path, args, workdir, secLvl, requesterPid, &oPid, &oFdInw, &oFdOutr, &oFdErr, request->stdin, request->stdout, request->stderr);

	// send response
	SpawnCommandSpawnResponse response;
	response.spawnedProcessID = oPid;
	response.status = spawnStatus;
	response.stdinWrite = oFdInw;
	response.stdoutRead = oFdOutr;
	response.stderrRead = oFdErr;
	SendMessageT(requester, &response, sizeof(SpawnCommandSpawnResponse), tx);
}

/**
 *
 */
bool createPipe(Pid thisPid, Pid requesterPid, Pid targetPid, Fd source, Fd *out, Fd target)
{
	Fd created = FD_NONE;
	if (source == FD_NONE)
	{
		// create pipe
		Fd pipe[2];
		FsPipeStatus pipeStat;
		PipeS(&pipe[0], &pipe[1], &pipeStat);

		if (pipeStat == FS_PIPE_SUCCESSFUL)
		{
			// map into target & requester
			created = CloneFdT(pipe[1], thisPid, target, targetPid);
			*out = CloneFd(pipe[0], thisPid, requesterPid);

			// close pipe here
			Close(pipe[0]);
			Close(pipe[1]);
		}
	}

	else
	{
		// map into target
		created = CloneFdT(source, requesterPid, target, targetPid);
		*out = source;
	}

	return created != FD_NONE;
}

/**
 *
 */
bool setupStdio(Pid createdPid, Pid requesterPid, Fd *outStdin, Fd *outStdout, Fd *outStderr, Fd inStdin, Fd inStdout,Fd inStderr)
{
	uint32_t thisPid = GetPid();
	if (!createPipe(thisPid, requesterPid, createdPid, inStdin, outStdin, STDIN_FILENO)) return false;
	if (!createPipe(thisPid, requesterPid, createdPid, inStdout, outStdout, STDOUT_FILENO)) return false;
	if (!createPipe(thisPid, requesterPid, createdPid, inStderr, outStderr, STDERR_FILENO)) return false;

	return true;
}

/**
 *
 */
void writeCliArgs(ProcessCreationIdentifier targetProc, const char *args)
{
	// prepare well-sized buffer
	char *argsBuf = new char[CLIARGS_BUFFER_LENGTH];
	size_t argsLen = strlen(args);

	// copy arguments to buffer
	memcpy(argsBuf, args, argsLen);
	argsBuf[argsLen] = 0;

	// store arguments
	CliArgsStore(targetProc, argsBuf);

	// free buffer
	delete argsBuf;
}

/**
 *
 */
BinaryFormat detectFormat(Fd file)
{
	// try to detect the format
	BinaryFormat form = BF_UNKNOWN;
	if (Elf32Loader::checkForElfBinaryAndReset(file) == LS_SUCCESSFUL) form = BF_ELF32;

	return form;
}

/*
 *
 */
bool findFile(const char *path, Fd *file)
{
	// try open with provided path
	if ((*file = OpenF(path, O_RDONLY)) != -1) return true;

	// if file don't exist add path variables
	std::string vars = Environment::getEnvVar("PATH");

	// find the first and last divisor for variables
	uint32_t index = vars.find(':');
	uint32_t last = vars.find(':');

	// separate strings and add path
	std::string cmd = vars.substr(0, vars.find(':')) + path;
	std::string app = vars.substr(vars.find(':') + 1, vars.find(':')) + path + "/bin/" + path;;
	std::string svr = vars.substr(vars.find_last_of(':') + 1) + path;

	// open all files and return true at first finded
	if ((*file = OpenF(cmd.c_str(), O_RDONLY)) != -1) return true;
	if ((*file = OpenF(app.c_str(), O_RDONLY)) != -1) return true;
	if ((*file = OpenF(svr.c_str(), O_RDONLY)) != -1) return true;

	return false;
}

/**
 *
 */
SpawnStatus spawn(const char *path, const char *args, const char *workdir, SecurityLevel secLvl, Pid requesterPid, Pid *outPid, Fd *outStdin, Fd *outStdout, Fd *outStderr, Fd inStdin, Fd inStdout, Fd inStderr)
{
	// file of executable
	Fd file;

	// open input file
	if (!findFile(path, &file))
	{
		klog("unable to open the file: %s, it doesn't exist on PATH directory", path);
		return SPAWN_STATUS_IO_ERROR;
	}

	// detect format
	BinaryFormat format = detectFormat(file);
	if (format == BF_UNKNOWN)
	{
		klog("binary has an unknown format: %s", path);
		return SPAWN_STATUS_FORMAT_ERROR;
	}

	// create empty target process
	auto targetProc = CreateEmptyProcess(secLvl);
	Pid targetPid = GetCreatedProcessID(targetProc);

	// apply configuration
	ProcessConfiguration configuration;
	configuration.sourcePath = (char*) path;
	ConfigureProcess(targetProc, configuration);

	// create a loader
	Loader *loader;
	if (format == BF_ELF32) loader = new Elf32Loader(targetProc, file);
	//else if (format == BF_ELF64) loader = new Elf64Loader(targetProc, file);
	else
	{
		klog("no loader implementation for binary format %i", format);
		return SPAWN_STATUS_FORMAT_ERROR;
	}

	// setup standard I/O
	if (!setupStdio(targetPid, requesterPid, outStdin, outStdout, outStderr, inStdin, inStdout, inStderr)) klog("unable to setup stdio for process %i", targetPid);

	// perform loading
	uintptr_t entryAddress;
	LoaderStatus ldrStat = loader->load(&entryAddress);
	SpawnStatus spawnStat = SPAWN_STATUS_UNKNOWN;

	if (ldrStat == LS_SUCCESSFUL)
	{
		// push command line arguments
		writeCliArgs(targetProc, args);

		// set working directory
		SetWorkingDirectoryP(workdir, targetProc);

		// attached loaded process
		AttachCreatedProcess(targetProc, entryAddress);

		// out-set process id
		*outPid = targetPid;
		spawnStat = SPAWN_STATUS_SUCCESSFUL;

	}

	else
	{
		// cancel creation & let kernel clean up
		CancelProcessCreation(targetProc);

		if (ldrStat == LS_IO_ERROR) spawnStat = SPAWN_STATUS_IO_ERROR;
		else if (ldrStat == LS_FORMAT_ERROR) spawnStat = SPAWN_STATUS_FORMAT_ERROR;
		else if (ldrStat == LS_MEMORY_ERROR) spawnStat = SPAWN_STATUS_MEMORY_ERROR;
		else
		{
			klog("loader return unknown status on failure %i",  ldrStat);
			spawnStat = SPAWN_STATUS_UNKNOWN;
		}
	}

	// Close binary file
	Close(file);

	return spawnStat;
}
