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

#include <memory/KernelHeap.hpp>
#include <logger/logger.hpp>
#include <EvangelionNG.hpp>
#include <memory/AddressSpace.hpp>
#include <memory/paging.hpp>
#include <memory/physical/PPallocator.hpp>
#include <memory/constants.hpp>
#include <memory/allocators/ChunkAllocator.hpp>

/**
 * new implementation
 */
void *operator new(long unsigned int size) { return KernelHeap::allocate(size); }

/**
 * new[] implementation
 */
void *operator new[](long unsigned int size) { return KernelHeap::allocate(size); }

/**
 * delete implementation
 */
void operator delete(void *m) { KernelHeap::free(m); }

/**
 * delete[] implementation
 */
void operator delete[](void *m) { KernelHeap::free(m); }

static ChunkAllocator allocator;

static VirtualAddress heapStart = 0;
static VirtualAddress heapEnd = 0;

static uint32_t usedMemoryAmount = 0;
static bool kernelHeapInitialized = false;

/**
 *
 */
uint32_t KernelHeap::getUsedMemoryAmount()
{
	return usedMemoryAmount;
}

/**
 *
 */
void KernelHeap::initialize(VirtualAddress start, VirtualAddress end)
{
	allocator.initialize(start, end);
	heapStart = start;
	heapEnd = end;

	logDebug("%! initialized with area: %h - %h", "kernheap", start, end);
	kernelHeapInitialized = true;
}

/**
 *
 */
bool KernelHeap::expandHeap()
{
	if (heapEnd > CONST_KERNEL_HEAP_MAXIMUM_END)
	{
		logDebug("%! maximum reached during expansion", "kernheap");
		return false;
	}

	// Expand virtual space
	for (VirtualAddress v = heapEnd; v < heapEnd + KERNEL_HEAP_EXPAND_STEP; v = v + PAGE_SIZE)
	{
		PhysicalAddress p = PPallocator::allocate();

		if (p == 0)
		{
			logWarn("%! no pages left for expanding", "kernheap");
			return false;
		}

		AddressSpace::map(v, p, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS);
	}

	// Create header
	allocator.expand(KERNEL_HEAP_EXPAND_STEP);
	heapEnd = heapEnd + KERNEL_HEAP_EXPAND_STEP;

	logDebug("%! expanded to end %h (%ikb in use)", "kernheap", heapEnd, usedMemoryAmount / 1024);
	return true;
}

/**
 *
 */
void *KernelHeap::allocate(uint32_t size)
{
	if (!kernelHeapInitialized) EvaKernel::panic("%! tried to use uninitialized kernel heap", "kernheap");

	void *allocated = allocator.allocate(size);
	if (allocated)
	{
		usedMemoryAmount = usedMemoryAmount + size;
		return allocated;
	}

	else if (expandHeap()) return allocate(size);

	EvaKernel::panic("%! could not expand kernel heap", "kernheap");
	return 0;
}

/**
 *
 */
void KernelHeap::free(void *mem)
{
	if (!kernelHeapInitialized) EvaKernel::panic("%! tried to use uninitialized kernel heap", "kernheap");

	usedMemoryAmount = usedMemoryAmount - allocator.free(mem);
}
