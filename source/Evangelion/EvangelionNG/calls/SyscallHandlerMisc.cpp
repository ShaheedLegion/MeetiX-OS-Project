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

#include <calls/SyscallHandler.hpp>

#include <logger/logger.hpp>
#include <tasking/ThreadManager.hpp>
#include <tasking/tasking.hpp>
#include <utils/string.hpp>
#include <memory/physical/PPallocator.hpp>
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
		if (data->message[i] == '%') data->message[i] = '!';

	const char *taskIdent = currentThread->getIdentifier();
	if (taskIdent == 0) taskIdent = currentThread->process->main->getIdentifier();

	// If the task has an identifier, do log with name:
	const char *prefix = "*";
	if (taskIdent != 0)
	{
		logInfo("%! (%i:%i) %s", taskIdent, process->main->id, currentThread->id, data->message);
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
		EvaQueryPciCountData *buffer = (EvaQueryPciCountData*) data->buffer;
		buffer->count = Pci::getDeviceCount();
	}

	else if (queryCommand == EVAQUERY_PCI_GET)
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;

		EvaQueryPciGetData *buf = (EvaQueryPciGetData*) data->buffer;
		PciHeader *pciHeader = Pci::getDeviceAt(buf->position);

		if (pciHeader == nullptr) buf->found = false;
		else
		{
			buf->found = true;

			buf->bus = pciHeader->bus;
			buf->slot = pciHeader->slot;
			buf->function = pciHeader->function;

			buf->vendorId = pciHeader->vendorId;
			buf->deviceId = pciHeader->deviceId;

			buf->classCode = pciHeader->classCode;
			buf->subclassCode = pciHeader->subclassCode;
			buf->progIf = pciHeader->progIf;
		}
	}

	else if (queryCommand == EVAQUERY_TASK_COUNT)
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;
		EvaQueryTaskCountData *buf = (EvaQueryTaskCountData*) data->buffer;
		buf->count = Tasking::count();
	}

	else if (queryCommand == EVAQUERY_TASK_LIST)
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;
		EvaQueryTaskListData *buf = (EvaQueryTaskListData*) data->buffer;
		buf->filledIds = Tasking::getTaskIds(buf->idBuffer, buf->idBufferSize);
	}

	else if (queryCommand == EVAQUERY_TASK_GET_BY_ID)
	{
		data->status = EVAQUERY_STATUS_SUCCESSFUL;

		EvaQueryTaskGetData *buf = (EvaQueryTaskGetData*) data->buffer;
		Thread *thread = Tasking::getTaskById(buf->id);

		if (thread)
		{
			buf->id = thread->id;
			if (thread->process->main == nullptr) buf->parent = -1;
			else buf->parent = thread->process->main->id;
			buf->type = thread->type;
			buf->memoryUsed = ThreadManager::getMemoryUsage(thread);

			// copy identifier
			const char *threadIdent = thread->getIdentifier();
			if (threadIdent == nullptr) buf->identifier[0] = 0;
			else
			{
				// do safe copy
				int max = 512;
				for (int i = 0; i < max; i++)
				{
					buf->identifier[i] = threadIdent[i];
					if (buf->identifier[i] == 0) break;
					if (i == max - 1)
					{
						buf->identifier[i] = 0;
						break;
					}
				}
			}

			// copy process source
			char *sourcePath = thread->process->sourcePath;
			if (sourcePath == nullptr) buf->sourcePath[0] = 0;
			else String::copy(buf->sourcePath, sourcePath);
		}
		else data->status = EVAQUERY_STATUS_UNKNOWN_ID;

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

	// copy loader name & version
	String::copy(data->loaderName, LOADER_NAME);
	data->LversionMJ = L_VERSION_MAJOR;
	data->LversionMN = L_VERSION_MINOR;
	data->LversionPT = L_VERSION_PATCH;

	// copy kernel name & version
	String::copy(data->kernelName, KERNEL_NAME);
	data->versionMJ = VERSION_MAJOR;
	data->versionMN = VERSION_MINOR;
	data->versionSB = VERSION_SUB;
	data->versionPT = VERSION_PATCH;

	return currentThread;
}

/**
 * return system specifications
 */
SYSCALL_HANDLER(sysinfo)
{
	SyscallSysinfo *data = (SyscallSysinfo*) SYSCALL_DATA(currentThread->cpuState);

	// get ram size in KB
	data->totRam = (PPallocator::getInitialAmount() * PAGE_SIZE / 1024);
	data->freeRam = (PPallocator::getFreePageCount() * PAGE_SIZE / 1024);

	// get number of cpu cores
	data->numberOfCore = System::getNumberOfProcessors();

	// get the cpu vendor name
	char vendor[13];
	Processor::getVendor(vendor);
	vendor[12] = 0;
	String::copy(data->cpuVendor, vendor);

	return currentThread;
}
