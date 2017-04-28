 /*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (char *argumentat your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHout ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "spawner.hpp"

#include <stdio.h>
#include <string.h>
#include <sstream>

#include <eva/bytewise.h>
#include <mxuser/utils/logger.hpp>
#include <mxuser/io/files/file_utils.hpp>

#include "elf32_loader.hpp"

static Pid pid;

/**
 *
 */
int main(int argc, char** argv) 
{
	receiveRequests();
}

/**
 *
 */
void receiveRequests() 
{
	// register this task as the system spawner
	if (!TaskRegisterID(SPAWNER_IDENTIFIER)) 
	{
		klog("failed to initialize spawner service: could not register with identifier %i", SPAWNER_IDENTIFIER);
		return;
	}

	// service ready
	klog("spawner server ready");

	// obtain task id
	Tid tid = GetTid();
	pid = GetPid();

	size_t requestLenMax = sizeof(MessageHeader) + sizeof(SpawnCommandSpawnRequest) + 1024;
	uint8_t *requestBuffer = new uint8_t[requestLenMax];
	while (true)
	{
		// receive incoming request
		MessageReceiveStatus stat = ReceiveMessage(requestBuffer, requestLenMax);

		if (stat != MESSAGE_RECEIVE_STATUS_SUCCESSFUL) 
		{
			protocolError("receiving command failed with code %i", stat);
			continue;
		}

		MessageHeader *header = (MessageHeader*) requestBuffer;
		SpawnCommandHeader *commandHeader = (SpawnCommandHeader*) MESSAGE_CONTENT(header);
	
		if (commandHeader->command == SPAWN_COMMAND_SPAWN_REQUEST) processSpawnRequest((SpawnCommandSpawnRequest*) commandHeader, header->sender, header->transaction);

		else protocolError("received unknown command: code %i, task %i", commandHeader->command, header->sender);
	}
	
	delete requestBuffer;
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
void writeCliArgs(ProcessCreationIdentifier targetProc, std::string args) 
{
	// prepare well-sized buffer
	const char *argsCstr = args.c_str();
	char *argsBuf = new char[CLIARGS_BUFFER_LENGTH];
	size_t argsLen = strlen(argsCstr);

	// copy arguments to buffer
	memcpy(argsBuf, argsCstr, argsLen);
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

/**
 *
 */
SpawnStatus spawn(const char *path, const char *args, const char *workdir, SecurityLevel secLvl, Pid requesterPid, Pid *outPid, Fd *outStdin, Fd *outStdout, Fd *outStderr, Fd inStdin, Fd inStdout, Fd inStderr)
{
	// open input file
	Fd file = Open(path);
	if (file == -1) 
	{
		klog("unable to open the file: %s", path);
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

