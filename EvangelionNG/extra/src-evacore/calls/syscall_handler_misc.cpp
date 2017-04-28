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

#include <calls/syscall_handler.hpp>

#include <logger/logger.hpp>
#include <tasking/thread_manager.hpp>
#include <tasking/tasking.hpp>
#include <utils/string.hpp>
#include <memory/physical/pp_allocator.hpp>
#include <system/system.hpp>
#include <eva/evaquery.h>
#include <system/pci/pci.hpp>

/**
 * Writes a message to the system log.
 */
SYSCALL_HANDLER(log) 
{
	Process *process = currentThread->process;
	SyscallLog *data = (SyscallLog*) SYSCALL_DATA(currentThread->cpuState);

	// % signs are not permitted, because the internal logger would get confused.
	uint32_t len = String::length(data->message);
	for (uint32_t i = 0; i < len; i++) 
	{
		if (data->message[i] == '%') data->message[i] = '!';
	}

	const char *taskIdent = currentThread->getIdentifier();
	if (taskIdent == 0) taskIdent = currentThread->process->main->getIdentifier();

	// If the task has an identifier, do log with name:
	const char *prefix = "*";
	if (taskIdent != 0) 
	{
		int header_len = String::length(prefix);
		int ident_len = String::length(taskIdent);
		char loggie[header_len + ident_len + 1];
		String::concat(prefix, taskIdent, loggie);

		logInfo("%! (%i:%i) %s", loggie, process->main->id, currentThread->id, data->message);
	} 

	else 
	{
		logInfo("%! (%i:%i) %s", prefix, process->main->id, currentThread->id, data->message);
	}

	return currentThread;
}

/**
 * Sets the log output to the screen enabled or disabled.
 */
SYSCALL_HANDLER(setVideoLog) 
{
	SyscallSetVideoLog *data = (SyscallSetVideoLog*) SYSCALL_DATA(currentThread->cpuState);

	Logger::setVideo(data->enabled);

	return currentThread;
}

/**
 * Test call handler
 */
SYSCALL_HANDLER(test) 
{

	SyscallTest *data = (SyscallTest*) SYSCALL_DATA(currentThread->cpuState);

	if (data->test == 1) 
	{
		data->result = PPallocator::getFreePageCount();
		logInfo("free pages: %i", data->result);
	} 

	else data->result = 0;

	return currentThread;
}

/**
 * Evaquery handler
 */
SYSCALL_HANDLER(evaQuery) 
{
	SyscallEvaQuery *data = (SyscallEvaQuery*) SYSCALL_DATA(currentThread->cpuState);

	// get the short query command
	uint16_t queryCommand = data->command;

	if (queryCommand == EVAQUERY_PCI_COUNT) 
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;
		EvaQueryPciCountOut *out = (EvaQueryPciCountOut*) data->outbuffer;
		out->count = Pci::getDeviceCount();
	} 

	else if (queryCommand == EVAQUERY_PCI_GET) 
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;

		EvaQueryPciGetIn *in = (EvaQueryPciGetIn*) data->query;
		PciHeader *pciHeader = Pci::getDeviceAt(in->position);

		EvaQueryPciGetOut *out = (EvaQueryPciGetOut*) data->outbuffer;
		if (pciHeader == nullptr) out->found = false;

		else 
		{
			out->found = true;

			out->bus = pciHeader->bus;
			out->slot = pciHeader->slot;
			out->function = pciHeader->function;

			out->vendorId = pciHeader->vendorId;
			out->deviceId = pciHeader->deviceId;

			out->classCode = pciHeader->classCode;
			out->subclassCode = pciHeader->subclassCode;
			out->progIf = pciHeader->progIf;
		}

	} 

	else if (queryCommand == EVAQUERY_TASK_COUNT) 
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;
		((EvaQueryTaskCountOut*) data->outbuffer)->count = Tasking::count();
	} 

	else if (queryCommand == EVAQUERY_TASK_GET_BY_POS) 
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;

		EvaQueryTaskGetByPosIn *in = (EvaQueryTaskGetByPosIn*) data->query;
		EvaQueryTaskGetOut *out = (EvaQueryTaskGetOut*) data->outbuffer;

		Thread *thread = Tasking::getAtPosition(in->position);

		out->id = thread->id;
		out->parent = thread->process->main->id;
		out->type = thread->type;
		out->memoryUsed = ThreadManager::getMemoryUsage(thread);
		
		// get identifier
		const char *threadIdent = thread->getIdentifier();

		// if doesn't exit set null
		if (threadIdent == nullptr) out->identifier[0] = 0;

		// else copy it
		else String::copy(out->identifier, threadIdent);

		// copy process source
		char *sourcePath = thread->process->sourcePath;
		if (sourcePath == nullptr) out->sourcePath[0] = 0;

		else String::copy(out->sourcePath, sourcePath);
	} 

	else data->status = EVAQUERY_STATUS_UNKNOWN_ID;

	return currentThread;
}

/**
 * return kernel version
 */
SYSCALL_HANDLER(uname)
{
	SyscallUname *data = (SyscallUname*) SYSCALL_DATA(currentThread->cpuState);

	data->LversionMJ = L_VERSION_MAJOR;
	data->LversionMN = L_VERSION_MINOR;
	data->LversionPT = L_VERSION_PATCH;

	data->versionMJ = VERSION_MAJOR;
	data->versionMN = VERSION_MINOR;
	data->versionSB = VERSION_SUB;
	data->versionPT = VERSION_PATCH;

	return currentThread;
}

/**
 * return system spech
 */
SYSCALL_HANDLER(sysinfo)
{
	SyscallSysinfo *data = (SyscallSysinfo*) SYSCALL_DATA(currentThread->cpuState);

	data->numberOfCore = System::getNumberOfProcessors();
	//Processor::getVendor(data->cpuVendor);

	data->totRam = (PPallocator::getInitialAmount() * PAGE_SIZE / 1024);
	data->freeRam = (PPallocator::getFreePageCount() * PAGE_SIZE / 1024);

	return currentThread;
}