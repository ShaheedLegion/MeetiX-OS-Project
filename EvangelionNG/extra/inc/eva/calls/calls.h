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

#ifndef EVA_API_CALLS_CALLS
#define EVA_API_CALLS_CALLS

#include "eva/common.h"
#include "eva/calls/calls_memory.hpp"
#include "eva/calls/calls_misc.hpp"
#include "eva/calls/calls_messaging.hpp"
#include "eva/calls/calls_ramdisk.hpp"
#include "eva/calls/calls_spawning.hpp"
#include "eva/calls/calls_tasking.hpp"
#include "eva/calls/calls_vm86.hpp"
#include "eva/calls/calls_filesystem.hpp"
#include "eva/calls/calls_speaker.hpp"

__BEGIN_C

#define SYSCALL_EXIT								0x101
#define SYSCALL_YIELD								0x102
#define SYSCALL_GET_PROCESS_ID						0x103
#define SYSCALL_GET_TASK_ID							0x104
#define SYSCALL_GET_PROCESS_ID_FOR_TASK_ID			0x105
#define SYSCALL_FORK								0x106
#define SYSCALL_JOIN								0x107
#define SYSCALL_SLEEP								0x108
#define SYSCALL_ATOMIC_LOCK							0x109
#define SYSCALL_WAIT_FOR_IRQ						0x10A
#define SYSCALL_LOG									0x10B
#define SYSCALL_SET_VIDEO_LOG						0x10C
#define SYSCALL_TEST								0x10D
#define SYSCALL_STORE_CLI_ARGUMENTS					0x10E
#define SYSCALL_RELEASE_CLI_ARGUMENTS				0x10F
#define SYSCALL_GET_WORKING_DIRECTORY				0x110
#define SYSCALL_SET_WORKING_DIRECTORY				0x111
#define SYSCALL_KILL								0x112
#define SYSCALL_ATOMIC_BLOCK						0x113
#define SYSCALL_REGISTER_IRQ_HANDLER				0x114
#define SYSCALL_RESTORE_INTERRUPTED_STATE			0x115
#define SYSCALL_REGISTER_SIGNAL_HANDLER				0x116
#define SYSCALL_RAISE_SIGNAL						0x117
#define SYSCALL_ATOMIC_TRY_LOCK						0x118
#define SYSCALL_EVAQUERY							0x119
#define SYSCALL_GET_EXECUTABLE_PATH					0x11A
#define SYSCALL_EXIT_THREAD							0x11B
#define SYSCALL_GET_PARENT_PROCESS_ID				0x11C		

#define SYSCALL_CALL_VM86							0x201
#define SYSCALL_LOWER_MEMORY_ALLOCATE				0x202
#define SYSCALL_LOWER_MEMORY_FREE					0x203
#define SYSCALL_ALLOCATE_MEMORY						0x204
#define SYSCALL_UNMAP								0x205
#define SYSCALL_SHARE_MEMORY						0x206
#define SYSCALL_MAP_MMIO_AREA						0x207
#define SYSCALL_SBRK								0x208

#define SYSCALL_RAMDISK_SPAWN						0x301
#define SYSCALL_CREATE_THREAD						0x302
#define SYSCALL_GET_THREAD_ENTRY					0x303
#define SYSCALL_CREATE_EMPTY_PROCESS				0x304
#define SYSCALL_CREATE_PAGES_IN_SPACE				0x305
#define SYSCALL_ATTACH_CREATED_PROCESS				0x306
#define SYSCALL_CANCEL_PROCESS_CREATION				0x307
#define SYSCALL_GET_CREATED_PROCESS_ID				0x308
#define SYSCALL_WRITE_TLS_MASTER_FOR_PROCESS		0x309
#define SYSCALL_CONFIGURE_PROCESS					0x30A

#define SYSCALL_REGISTER_TASK_IDENTIFIER			0x401
#define SYSCALL_GET_TASK_FOR_IDENTIFIER				0x402
#define SYSCALL_MESSAGE_SEND						0x403
#define SYSCALL_MESSAGE_RECEIVE						0x404
#define SYSCALL_MESSAGE_RECEIVE_TRANSACTION			0x405

#define SYSCALL_RAMDISK_FIND						0x501
#define SYSCALL_RAMDISK_FIND_CHILD					0x502
#define SYSCALL_RAMDISK_INFO						0x503
#define SYSCALL_RAMDISK_READ						0x504
#define SYSCALL_RAMDISK_CHILD_COUNT					0x505
#define SYSCALL_RAMDISK_CHILD_AT					0x506
#define SYSCALL_GET_MILLISECONDS					0x507

#define SYSCALL_FS_OPEN								0x601
#define SYSCALL_FS_READ								0x602
#define SYSCALL_FS_WRITE							0x603
#define SYSCALL_FS_CLOSE							0x604
#define SYSCALL_FS_STAT								0x605
#define SYSCALL_FS_FSTAT							0x606
#define SYSCALL_FS_CLONEFD							0x607
#define SYSCALL_FS_PIPE								0x608
#define SYSCALL_FS_LENGTH							0x609
#define SYSCALL_FS_SEEK								0x60A
#define SYSCALL_FS_TELL								0x60B
#define SYSCALL_FS_REGISTER_AS_DELEGATE				0x60C
#define SYSCALL_FS_SET_TRANSACTION_STATUS			0x60D
#define SYSCALL_FS_CREATE_NODE						0x60E
#define SYSCALL_FS_OPEN_DIRECTORY					0x60F
#define SYSCALL_FS_READ_DIRECTORY					0x610
#define SYSCALL_FS_CLOSE_DIRECTORY					0x611

#define SYSCALL_UNAME								0x701
#define SYSCALL_SYSINFO								0x702
	
#define SYSCALL_REBOOT								0x801
#define SYSCALL_SHUTDOWN							0X802

#define SYSCALL_SOUND                         		0x901
#define SYSCALL_SOUNDMUTE							0x902

__END_C

#endif
