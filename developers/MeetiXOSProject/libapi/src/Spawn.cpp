/*********************************************************************************
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]                  *
* 																			     *
* This program is free software; you can redistribute it and/or                  *
* modify it under the terms of the GNU General Public License                    *
* as published by the Free Software Foundation; either version 2				 *
* of the License, or (char *argumentat your option) any later version.			 *
*																				 *
* This program is distributed in the hope that it will be useful,				 *
* but WITHout ANY WARRANTY; without even the implied warranty of                 *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 				 *
* GNU General Public License for more details.									 *
*																				 *
* You should have received a copy of the GNU General Public License				 *
* along with this program; if not, write to the Free Software                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
**********************************************************************************/

#include "eva.h"
#include "eva/bytewise.h"
#include <string.h>
#include <eva/utils/local.hpp>

// redirect
SpawnStatus Spawn(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel)
{
	return SpawnPOI(path, args, workdir, securityLevel, nullptr, nullptr, nullptr);
}

// redirect
SpawnStatus SpawnP(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel, Pid *pid)
{
	return SpawnPOI(path, args, workdir, securityLevel, pid, nullptr, nullptr);
}

// redirect
SpawnStatus SpawnPO(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel, Pid *pid, Fd outStdio[3])
{
	return SpawnPOI(path, args, workdir, securityLevel, pid, outStdio, nullptr);
}

/**
 *
 */
SpawnStatus SpawnPOI(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel, Pid *pid, Fd outStdio[3], Fd inStdio[3])
{

	SpawnStatus res = SPAWN_STATUS_UNKNOWN;
	uint32_t tid = GetTid();

	// get spawner task identifier
	Tid spawnerTid = TaskGetID(SPAWNER_IDENTIFIER);
	if (spawnerTid == -1) 
	{
		return res;
	}

	// create transaction
	MessageTransaction tx = GetMessageTxId();

	// create request
	size_t pathBytes = strlen(path) + 1;
	size_t argsBytes = strlen(args) + 1;
	size_t workdirBytes = strlen(workdir) + 1;
	size_t requestlen = sizeof(SpawnCommandSpawnRequest) + pathBytes + argsBytes + workdirBytes;
	Local<uint8_t> _request(new uint8_t[requestlen]);
	uint8_t *request = _request();

	// copy request contents
	SpawnCommandSpawnRequest *req = (SpawnCommandSpawnRequest*) request;
	req->header.command = SPAWN_COMMAND_SPAWN_REQUEST;
	req->securityLevel = securityLevel;
	req->pathBytes = pathBytes;
	req->argsBytes = argsBytes;
	req->workdirBytes = workdirBytes;

	if (inStdio != nullptr) 
	{
		req->stdin = inStdio[0];
		req->stdout = inStdio[1];
		req->stderr = inStdio[2];
	} 

	else 
	{
		req->stdin = FD_NONE;
		req->stdout = FD_NONE;
		req->stderr = FD_NONE;
	}

	uint8_t *insert = request;
	insert = insert + sizeof(SpawnCommandSpawnRequest);
	memcpy(insert, path, pathBytes);
	insert = insert + pathBytes;
	memcpy(insert, args, argsBytes);
	insert = insert + argsBytes;
	memcpy(insert, workdir, workdirBytes);

	// send request to spawner
	SendMessageT(spawnerTid, request, requestlen, tx);

	// receive response
	size_t respLen = sizeof(MessageHeader) + sizeof(SpawnCommandSpawnResponse);
	Local<uint8_t> respBuf(new uint8_t[respLen]);
	ReceiveMessageT(respBuf(), respLen, tx);

	SpawnCommandSpawnResponse *response = (SpawnCommandSpawnResponse*) MESSAGE_CONTENT(respBuf());

	// if successful, take response parameters
	if (response->status == SPAWN_STATUS_SUCCESSFUL) 
	{

		if (pid != nullptr) 
		{
			*pid = response->spawnedProcessID;
		}

		if (outStdio != nullptr) 
		{
			outStdio[0] = response->stdinWrite;
			outStdio[1] = response->stdoutRead;
			outStdio[2] = response->stderrRead;
		}
	}

	return response->status;
}
