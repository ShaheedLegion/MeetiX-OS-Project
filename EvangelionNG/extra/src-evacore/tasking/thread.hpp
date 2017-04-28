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

#ifndef EVA_MULTITASKING_THREAD
#define EVA_MULTITASKING_THREAD

#include <system/processor_state.hpp>

#include "eva/kernel.h"
#include "eva/calls/calls.h"
#include "eva/signal.h"
#include "memory/paging.hpp"
#include "memory/collections/address_range_pool.hpp"

// forward declarations
class Process;
class Waiter;
class Scheduler;

/**
 * Data used by virtual 8086 processes
 */
struct ThreadInformationVm86 
{
public:
	ThreadInformationVm86() : cpuIf(false), out(0), interruptRecursionLevel(0) {}

	bool cpuIf;
	Vm86Registers *out;
	uint32_t interruptRecursionLevel;
};

/**
 *
 */
enum class ThreadInterruptionInfoType : uint8_t 
{
	NONE, 
	IRQ, 
	SIGNAL
};

/**
 *
 */
class ThreadInterruptionInfo 
{
public:
	ProcessorState cpuState;
	ProcessorState *cpuStateAddress;
	Waiter *waitManager;

	ThreadInterruptionInfoType type = ThreadInterruptionInfoType::NONE;
	uint8_t handledIrq;
	int handledSignal;
};

/**
 *
 */
class Thread 
{
private:
	ThreadInformationVm86 *vm86Information;
	char *identifier;

public:
	Thread(ThreadType type);
	~Thread();

	Tid id;
	bool alive;
	ThreadType type;
	ThreadPriority priority;
	Process *process;
	Scheduler *scheduler;

	Waiter *waitManager;
	uint32_t waitCount;

	uint64_t rounds;

	void *userData;
	void *threadEntry;

	ProcessorState *cpuState;
	VirtualAddress kernelStackPageVirt;
	VirtualAddress kernelStackEsp0;
	VirtualAddress userStackAreaStart;
	uint8_t userStackPages;

	VirtualAddress userThreadAddr;
	VirtualAddress tlsCopyVirt;

	ThreadInterruptionInfo *interruptionInfo;

	ThreadInformationVm86 *getVm86Information();
	const char *getIdentifier();
	void setIdentifier(const char *newIdentifier);

	void wait(Waiter *waitManager);
	void unwait();
	bool checkWaiting();

	void raiseSignal(int signal);
	void enterIrqHandler(uintptr_t address, uint8_t irq, uintptr_t callback);
	void enterSignalHandler(uintptr_t address, int signal, uintptr_t callback);
	bool startPrepareInterruption();
	void finishPrepareInterruption(uintptr_t address, uintptr_t callback);

	void storeForInterruption();
	void restoreInterruptedState();
};

#endif
