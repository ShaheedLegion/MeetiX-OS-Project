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

#ifndef EVA_KERNEL_SYSTEM_CALLS
#define EVA_KERNEL_SYSTEM_CALLS

#include <stdarg.h>
#include <stddef.h>
#include "eva/common.h"
#include "eva/kernel.h"
#include "eva/system.h"
#include "eva/ramdisk.h"
#include "eva/ipc.h"
#include "eva/types.h"
#include "eva/fs.h"
#include "eva/calls/calls.h"

__BEGIN_C

/**
 * Performs an atomic wait. If the atom is true, the executing task must
 * wait until the task that owns the atom has finished its work and sets
 * it to false. Once the atom is false, it is set to true and the function
 * returns.
 *
 * @param atom
 * 		the atom to use
 *
 * @security-level APPLICATION
 */
void AtomicLock(uint8_t *atom);
void AtomicLockDual(uint8_t *atom1, uint8_t *atom2);

/**
 * Trys to perform atomic wait. If the lock is already locked, the function
 * returns 0. Otherwise, the lock is set as in {AtomicLock} and the
 * function returns 1.
 *
 * @param atom
 * 		the atom to use
 *
 * @security-level APPLICATION
 */
uint8_t AtomicTryLock(uint8_t *atom);
uint8_t AtomicTryLockDual(uint8_t *atom1, uint8_t *atom2);

/**
 * Performs an atomic block. If the atom is true, the executing task must
 * wait until the task that owns the atom has finished its work and sets
 * it to false. Different from the {AtomicLock}, the atom is not changed.
 *
 * @param atom
 * 		the atom to use
 *
 * @security-level APPLICATION
 */
void AtomicBlock(uint8_t *atom);
void AtomicBlockDual(uint8_t *atom1, uint8_t *atom2);

/**
 * Spawns a program binary.
 *
 * @param path
 * 		absolute path of the executable
 * @param args
 * 		unparsed arguments
 * @param workdir
 * 		working directory for the execution
 * @param securityLevel
 * 		security level to spawn the process to
 * @param outProcessId
 * 		is filled with the process id
 * @param out_stdio
 * 		is filled with stdio file descriptors, 0 is write end of stdin,
 * 		1 is read end of stdout, 2 is read end of stderr
 * @param in_stdio
 * 		if supplied, the given descriptors which are valid for the executing process
 * 		are used as the stdin/out/err for the spawned process; an entry might be -1
 * 		to be ignored and default behaviour being applied
 *
 * @return one of the {SpawnStatus} codes
 *
 * @security-level APPLICATION
 */
SpawnStatus Spawn(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel);
SpawnStatus SpawnP(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel, Pid *pid);
SpawnStatus SpawnPO(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel, Pid *pid, Fd outStdio[3]);
SpawnStatus SpawnPOI(const char *path, const char *args, const char *workdir, SecurityLevel securityLevel, Pid *pid, Fd outStdio[3], Fd inStdio[3]);

/**
 * Kills a process.
 *
 * @param pid
 * 		the process id
 *
 * @security-level APPLICATION
 */
KillStatus Kill(Pid pid);

/**
 * Prints a message to the log.
 *
 * @param message
 * 		the message to log
 *
 * @security-level APPLICATION
 */
void Log(const char *message);

/**
 * Performs the software interrupt necessary for the system call passing the
 * given data (usually a pointer to a call struct).
 *
 * @param call
 * 		the call to execute
 * @param data
 * 		the data to pass
 *
 * Clobber flags (more: https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html):
 * - The "cc" clobber indicates that the assembler code modifies the FLAGS register
 * - The "memory" clobber tells the compiler that the assembly code performs memory reads
 *   or writes to items other than those listed in the input and output operands. Causes
 *   GCC to assume that anything in memory could have changed (the system call could do so).
 *
 * @security-level APPLICATION
 */
void syscall(uint32_t call, uint32_t data);

/**
 * Opens a file.
 *
 * @param path
 * 		the path to the file
 * @param-opt flags
 * 		the flags for open mode
 * @param-opt outStatus
 * 		filled with one of the {FsOpenStatus} codes
 *
 * @return the file descriptor for the opened file
 *
 * @security-level APPLICATION
 */
Fd Open(const char *path);
Fd OpenF(const char *path, int32_t flags);
Fd OpenFS(const char *path, int32_t flags, FsOpenStatus *outStatus);
Fd OpenFMS(const char *path, int32_t flags, int32_t mode, FsOpenStatus *outStatus);

/**
 * Closes a file.
 *
 * @param fd
 * 		the file descriptor to close
 *
 * @return one of the {FsCloseStatus} codes
 *
 * @security-level APPLICATION
 */
FsCloseStatus Close(Fd fd);

/**
 * Retrieves the length of a file in bytes.
 *
 * @param fd
 * 		the file descriptor
 *
 * @param outStatus
 * 		is filled with the status code
 *
 * @return the length in bytes
 *
 * @security-level APPLICATION
 */
int64_t Length(Fd fd);
int64_t LengthS(Fd fd, FsLengthStatus *outStatus);

/**
 * Opens a directory.
 *
 * @param path
 * 		path of the directory
 *
 * @return pointer to a directory iterator, or 0 if not successful
 */
FsDirectoryIterator *OpenDirectory(const char *path);
FsDirectoryIterator *OpenDirectoryS(const char *path, FsOpenDirectoryStatus *outStatus);

/**
 * Reads the next entry of the directory.
 *
 * @param iterator
 * 		the directory iterator
 *
 * @param outStatus
 * 		is filled with the status code
 *
 * @return a directory entry structure, or 0 if not successful
 */
FsDirectoryEntry *ReadDirectory(FsDirectoryIterator *iterator);
FsDirectoryEntry *ReadDirectoryS(FsDirectoryIterator *iterator, FsReadDirectoryStatus *outStatus);

/**
 * Closes a directory.
 *
 * @param iterator
 * 		the directory iterator
 */
void CloseDirectory(FsDirectoryIterator *iterator);

/**
 * Retrieves the length of a file in bytes.
 *
 * @param path
 * 		path of the file
 *
 * @param follow_symlinks
 * 		whether to follow symbolic links or not
 *
 * @param-opt outStatus
 * 		is filled with the status code
 *
 * @return the length in bytes
 *
 * @security-level APPLICATION
 */
int64_t Flength(const char *path);
int64_t FlengthS(const char *path, uint8_t followSymlinks);
int64_t FlengthSS(const char *path, uint8_t followSymlinks, FsLengthStatus *outStatus);

/**
 * Repositions the offset within a file.
 *
 * @param fd
 * 		the file descriptor
 * @param off
 * 		the offset
 * @param-opt outStatus
 * 		is filled with the status
 * @param whence
 * 		one of the {FsSeekMode} constants
 *
 * @return if successful returns the new absolute offset, otherwise -1
 *
 * @security-level APPLICATION
 */
int64_t Seek(Fd fd, int64_t off, FsSeekMode mode);
int64_t SeekS(Fd fd, int64_t off, FsSeekMode mode, FsSeekStatus *outStatus);

/**
 * Retrieves the current offset within a file.
 *
 * @param fd
 * 		the file descriptor
 * @param-opt outStatus
 * 		is filled with the status
 *
 * @return if successful returns the current absolute offset, otherwise -1
 *
 * @security-level APPLICATION
 */
int64_t Tell(Fd fd);
int64_t TellS(Fd fd, FsTellStatus *outStatus);

/**
 * Sets the working directory for the current process.
 *
 * @param path
 * 		buffer of at least {PATH_MAX} bytes size
 * 		containing the new working directory
 *
 * @param process
 * 		process to set working directory for during spawning
 *
 * @return one of the {SetWorkingDirectoryStatus} codes
 *
 * @security-level APPLICATION if no process given, otherwise KERNEL
 */
SetWorkingDirectoryStatus SetWorkingDirectory(const char *path);
SetWorkingDirectoryStatus SetWorkingDirectoryP(const char *path, ProcessCreationIdentifier process);

/**
 * Retrieves the working directory for the current process.
 *
 * @param path
 * 		buffer of at least <maxlen> or {PATH_MAX} bytes size
 *
 * @param maxlen
 * 		length of the buffer in bytes
 *
 * @return whether the action was successful
 *
 * @security-level APPLICATION
 */
GetWorkingDirectoryStatus GetWorkingDirectory(char *buffer);
GetWorkingDirectoryStatus GetWorkingDirectoryL(char *buffer, size_t maxlen);

/**
 * Retrieves the directory of the executable when available, otherwise an empty
 * string is written to the buffer.
 *
 * @param path
 * 		buffer of at least {PATH_MAX} bytes size
 *
 * @security-level APPLICATION
 */
void GetExecutablePath(char *buffer);

/**
 * Reads bytes from the file to the buffer.
 *
 * @param fd
 * 		the file descriptor
 * @param buffer
 * 		the target buffer
 * @param length
 * 		the length in bytes
 * @param-opt outStatus
 * 		filled with one of the {FsReadStatus} codes
 *
 * @return if the read was successful the length of bytes or
 * 		zero if EOF, otherwise -1
 *
 * @security-level APPLICATION
 */
int32_t Read(Fd fd, void *buffer, uint64_t length);
int32_t ReadS(Fd fd, void *buffer, uint64_t length, FsReadStatus *outStatus);

/**
 * Writes bytes from the buffer to the file.
 *
 * @param fd
 * 		the file descriptor
 * @param buffer
 * 		the source buffer
 * @param length
 * 		the length in bytes
 * @param-opt outStatus
 * 		filled with one of the {FsWriteStatus} codes
 *
 * @return if successful the number of bytes that were written, otherwise -1
 *
 * @security-level APPLICATION
 */
int32_t Write(Fd fd, const void *buffer, uint64_t length);
int32_t WriteS(Fd fd, const void *buffer, uint64_t length, FsWriteStatus *outStatus);

/**
 * Returns the next transaction id that can be used for messaging.
 * When sending a message, a transaction can be added so that one can wait
 * for an answer with the same transaction. This method always returns a
 * new transaction id each time it is called and is thread-safe.
 *
 * @return a new transaction id
 *
 * @security-level APPLICATION
 */
MessageTransaction GetMessageTxId();

/**
 * Allocates a memory region with the size of at least the given
 * size in bytes. This region can for example be used as shared memory.
 *
 * Allocating memory using this call makes the requesting process the physical owner of the
 * pages in its virtual space (important for unmapping).
 *
 * @param size
 * 		the size in bytes
 *
 * @return a pointer to the allocated memory region, or 0 if failed
 *
 * @security-level APPLICATION
 */
void *AllocMem(int32_t size);

/**
 * Shares a memory area with another process.
 *
 * @param memory
 * 		a pointer to the memory area to share
 * @param size
 * 		the size of the memory area
 * @param pid
 * 		the id of the target process
 *
 * @return a pointer to the shared memory location within the target address space
 *
 * @security-level APPLICATION
 */
void *ShareMem(void *memory, int32_t size, Pid pid);

/**
 * Yields, causing a switch to the next process.
 *
 * @security-level APPLICATION
 */
void Yield();

/**
 * Sleeps for the given amount of milliseconds.
 *
 * @param ms
 * 		the milliseconds to sleep
 *
 * @security-level APPLICATION
 */
void Sleep(uint64_t ms);

/**
 * Retrieves the current process id.
 *
 * @return the id of the executing process
 *
 * @security-level APPLICATION
 */
uint32_t GetPid();

/**
 * Retrieves the current thread id. If this thread is the main
 * thread of the process, the id is the same as the process id.
 *
 * @return the id of the executing thread
 *
 * @security-level APPLICATION
 */
uint32_t GetTid();

/**
 * Retrieves the process id for a thread id.
 *
 * @param tid the thread id
 *
 * @return the process id of the thread, or -1
 *
 * @security-level APPLICATION
 */
uint32_t GetPidForTid(uint32_t tid);

/**
 * Quits the process with the given status code.
 *
 * @param status
 * 		the status code
 *
 * @security-level APPLICATION
 */
void Exit(int status);

/**
 * Creates a thread.
 *
 * @param function
 * 		the entry function
 * @param-opt userData
 * 		a pointer to user data that should be passed
 * 		to the entry function
 *
 * @security-level APPLICATION
 */
Tid CreateThread(void *function);
Tid CreateThreadD(void *function, void *userData);
Tid CreateThreadDS(void *function, void *userData, CreateThreadStatus *outStatus);

/**
 * Sends a message to the given task. This means that <len> bytes from the
 * buffer <buf> are copied to a message that is then sent to the <target> task.
 * The message may be no longer than {MESSAGE_MAXIMUM_LENGTH}.
 *
 * The mode specifies how the function shall block:
 * - {MESSAGE_SEND_MODE_BLOCKING} the executing task will bock if the target tasks
 * 		message queue is full
 * - {MESSAGE_SEND_MODE_NON_BLOCKING} the function will return {MESSAGE_SEND_STATUS_QUEUE_FULL}
 * 		if the target tasks message queue is full
 *
 * @param target
 * 		id of the target task
 * @param buf
 * 		message content buffer
 * @param len
 * 		number of bytes to copy from the buffer
 * @param-opt mode
 * 		determines how the function blocks when given, default is {MESSAGE_SEND_MODE_BLOCKING}
 * @param-opt tx
 * 		transaction id
 *
 * @return one of the <MessageSendStatus> codes
 *
 * @security-level APPLICATION
 */
MessageSendStatus SendMessage(Tid target, void *buf, size_t len);
MessageSendStatus SendMessageM(Tid target, void *buf, size_t len, MessageSendMode mode);
MessageSendStatus SendMessageT(Tid tid, void *buf, size_t len, MessageTransaction tx);
MessageSendStatus SendMessageTM(Tid tid, void *buf, size_t len, MessageTransaction tx, MessageSendMode mode);

/**
 * Receives a message. At maximum <max> bytes will be attempted to be copied to
 * the buffer <buf>. Note that when receiving a message, a buffer with a size of
 * at least the size of {MessageHeader} plus the size of the sent message
 * must be used.
 *
 * After successful completion, the buffer will contain the message header followed
 * by the content of the message.
 * - to access the header, use the buffer pointer: ((MessageHeader * buf)
 * - to access the content, use the helper macro:  MESSAGE_CONTENT(buf)
 *
 * The mode specifies how the function shall block:
 * - {MESSAGE_RECEIVE_MODE_BLOCKING} the executing task will block if no messages
 * 		are available
 * - {MESSAGE_RECEIVE_MODE_NON_BLOCKING} the function will return {MESSAGE_RECEIVE_STATUS_QUEUE_EMPTY}
 * 		if the executing tasks message queue is empty
 *
 * When a transaction ID is given, only messages that were sent with the same
 * transaction ID will be received.
 *
 * @param buf
 * 		output buffer
 * @param max
 * 		maximum number of bytes to copy to the buffer
 * @param-opt mode
 * 		determines how the function blocks when given, default is {MESSAGE_RECEIVE_MODE_BLOCKING}
 * @param-opt tx
 * 		transaction id
 * @param break_condition
 * 		can be used to break the waiting process by setting its value to 1
 *
 * @security-level APPLICATION
 */
MessageReceiveStatus ReceiveMessage(void *buf, size_t max);
MessageReceiveStatus ReceiveMessageM(void *buf, size_t max, MessageReceiveMode mode);
MessageReceiveStatus ReceiveMessageT(void *buf, size_t max, MessageTransaction tx);
MessageReceiveStatus ReceiveMessageTM(void *buf, size_t max, MessageTransaction tx, MessageReceiveMode mode);
MessageReceiveStatus ReceiveMessageTMB(void *buf, size_t max, MessageTransaction tx, MessageReceiveMode mode, uint8_t *breakCondition);

/**
 * Registers the executing task for the given identifier.
 *
 * @param identifier
 * 		the identifier to set
 *
 * @return if it was successful true, if the identifier is taken false
 *
 * @security-level APPLICATION
 */
uint8_t TaskRegisterID(const char *identifier);

/**
 * Returns the id of the task that is register for the given identifier.
 *
 * @param identifier
 * 		the identifier
 *
 * @return the id of the task, or -1 if no task has this identifier
 *
 * @security-level APPLICATION
 */
Tid TaskGetID(const char *identifier);

/**
 * The executing thread waits for the interrupt request with the given number.
 *
 * @param irq
 * 		the irq number
 *
 * @security-level DRIVER
 */
void WaitForIrq(uint8_t irq);

/**
 * Maps the given physical address to the executing processes address space so
 * it can access it directly.
 *
 * @param addr
 * 		the physical memory address that should be mapped
 * @param size
 * 		the size that should be mapped
 *
 * @return a pointer to the mapped area within the executing processes address space
 *
 * @security-level DRIVER
 */
void *MapMmio(void *addr, uint32_t size);

/**
 * Unmaps the given memory area.
 *
 * @param area
 * 		a pointer to the area
 *
 * @security-level DRIVER
 */
void Unmap(void *area);

/**
 * Adjusts the program heap break.
 *
 * @param amount
 * 		the value to adjust the break by
 * @param out_brk	is filled with the result
 *
 * @return whether adjusting was successful
 *
 * @security-level APPLICATION
 */
uint8_t Sbrk(int amount, void **outBrk);

/**
 * Performs a Virtual 8086 BIOS interrupt call.
 *
 * @param interrupt
 * 		number of the interrupt to fire
 * @param in
 * 		input register values
 * @param out
 * 		output register values
 *
 * @return one of the VM86_CALL_STATUS_* status codes
 *
 * @security-level DRIVER
 */
Vm86CallStatus CallVm86(uint32_t interrupt, Vm86Registers *in, Vm86Registers *out);

/**
 * Frees a memory area allocated with {lower_malloc}.
 *
 * @param memory
 * 		a pointer to the memory area
 *
 * @security-level DRIVER
 */
void LowerFree(void *memory);

/**
 * Allocates a memory area in the lower 1MiB. This can be used
 * for example for the virtual 8086 mode.
 *
 * @param size
 * 		the size to allocate
 *
 * @security-level DRIVER
 */
void *LowerMalloc(uint32_t size);

/**
 * Enables or disables logging to the video output.
 *
 * @param enabled
 * 		whether to enable/disable video log
 *
 * @security-level APPLICATION
 */
void SetVideoLog(uint8_t enabled);

/**
 * Spawns a binary from the ramdisk.
 *
 * @param path
 * 		ramdisk-relative path to the binary
 * @param securityLevel
 * 		the security level to set for the spawned process
 *
 * @param security-level KERNEL
 */
RamdiskSpawnStatus RamdiskSpawn(const char *path, SecurityLevel securityLevel);

/**
 * Finds a ramdisk entry by its absolute path.
 *
 * @param path
 * 		absolute path
 *
 * @return if found the ramdisk node id, otherwise -1
 *
 * @security-level APPLICATION
 */
int RamdiskFind(const char *path);

/**
 * Finds a ramdisk entry's child by its relative path.
 *
 * @param path
 * 		relative path
 *
 * @return if found the ramdisk node id, otherwise -1
 *
 * @security-level APPLICATION
 */
int RamdiskFindChild(int nodeId, const char *childName);

/**
 * Retrieves ramdisk entry info for a specific node.
 *
 * @param nodeId
 * 		ramdisk node id
 * @param out
 * 		is filled with the info
 *
 * @security-level APPLICATION
 */
void RamdiskInfo(int nodeId, RamdiskEntryInfo *out);

/**
 * Reads bytes from a file on the ramdisk.
 *
 * @param nodeID
 * 		ramdisk node id
 * @param offset
 * 		offset within the file
 * @param buffer
 * 		target buffer
 * @param length
 * 		number of bytes to read
 *
 * @return if successful the number of bytes read, otherwise -1
 *
 * @security-level APPLICATION
 */
int RamdiskRead(int nodeId, uint32_t offset, char *buffer, uint32_t length);

/**
 * Returns the number of children of a ramdisk entry.
 *
 * @param nodeId
 * 		ramdisk node id
 *
 * @return the number of children
 *
 * @security-level APPLICATION
 */
int RamdiskChildCount(int nodeId);

/**
 * Returns the id of a child of the given node at the given index.
 *
 * @param nodeId
 * 		ramdisk node id
 * @param index
 * 		child index
 *
 * @return if the child exists the child node id, otherwise -1
 *
 * @security-level APPLICATION
 */
int RamdiskChildAt(int nodeId, int index);

/**
 * TODO: currently returns the number of milliseconds that one
 * of the schedulers is running.
 *
 * @return the number of milliseconds
 *
 * @security-level APPLICATION
 */
uint64_t Millis();

/**
 * Test-call for kernel debugging.
 *
 * @security-level VARIOUS
 */
uint32_t Test(uint32_t test);

/**
 * Forks the current process.
 * TODO:
 * 	- only works from the main thread
 *
 * @return within the executing process the forked processes id is returned,
 * 		within the forked process 0 is returned
 *
 * @security-level APPLICATION
 */
Tid Fork();

/**
 * Joins the task with the given id, making the executing task
 * wait until this task has died.
 *
 * @param tid
 * 		id of the task to join
 *
 * @security-level APPLICATION
 */
void Join(uint32_t tid);

/**
 * Clones a file descriptor in a process to another processes. Creates a new file
 * descriptor in the target process.
 *
 * @param source_fd
 * 		source file descriptor
 * @param source_pid
 * 		id of the source process
 * @param target_pid
 * 		id of the target process
 * @param-opt outStatus
 * 		is filled with the status code
 *
 * @return the resulting file descriptor
 *
 * @security-level APPLICATION
 */
Fd CloneFd(Fd sourceFd, Pid sourceProcess, Pid targetProcess);
Fd CloneFdS(Fd sourceFd, Pid sourceProcess, Pid targetProcess, FsClonefdStatus *outStatus);

/**
 * Clones a file descriptor in a process to another processes file descriptor value.
 *
 * @param source_fd
 * 		source file descriptor
 * @param source_pid
 * 		id of the source process
 * @param target_fd
 * 		target file descriptor
 * @param target_pid
 * 		id of the target process
 * @param-opt outStatus
 * 		is filled with the status code
 *
 * @return the target file descriptor
 *
 * @security-level APPLICATION
 */
Fd CloneFdT(Fd sourceFd, Pid sourceProcess, Fd targetFd, Pid targetProcess);
Fd CloneFdTS(Fd sourceFd, Pid sourceProcess, Fd targetFd, Pid targetProcess, FsClonefdStatus *outStatus);

/**
 * Opens a pipe.
 *
 * @param out_write
 * 		is filled with the pipes write end
 * @param out_read
 * 		is filled with the pipes read end
 * @param outStatus
 * 		is filled with the status code
 *
 * @security-level APPLICATION
 */
void Pipe(Fd *outWrite, Fd *outRead);
void PipeS(Fd *outWrite, Fd *outRead, FsPipeStatus *outStatus);

/**
 * Stores command line arguments for a created process.
 *
 * @param process
 * 		the process creation identifier
 * @param arguments
 * 		arguments to store
 *
 * @security-level KERNEL
 */
void CliArgsStore(ProcessCreationIdentifier process, char *arguments);

/**
 * Returns and releases the command line arguments for the executing process.
 * This buffer must have a length of at least {PROCESS_COMMAND_LINE_ARGUMENTS_BUFFER_LENGTH} bytes.
 * If no arguments were supplied for the executing process, the buffer is null-terminated only.
 *
 * @param buffer
 * 		target buffer to store the arguments to
 *
 * @security-level KERNEL
 */
void CliArgsRelease(char *buffer);

/**
 * Creates an empty process.
 *
 * @param securityLevel
 * 		the security level to use
 *
 * @return the process creation identifier for the created process
 *
 * @security-level KERNEL
 */
ProcessCreationIdentifier CreateEmptyProcess(SecurityLevel securityLevel);

/**
 * Configures the created empty process during spawning.
 *
 * @param process
 * 		process creation identifier for the target process
 * @param configuration
 * 		the configuration structure
 *
 * @security-level KERNEL
 */
void ConfigureProcess(ProcessCreationIdentifier process, ProcessConfiguration configuration);

/**
 * Creates pages in a process during process creation and maps the created pages in the
 * executing processes address space.
 *
 * @param process
 * 		the process creation identifier
 * @param virtualAddress
 * 		the address to map to
 * @param pages
 * 		number of pages to map
 *
 * @return the virtual address of the pages mapped to the executing processes address space
 *
 * @security-level KERNEL
 */
void *CreatePagesInSpaces(ProcessCreationIdentifier process, uint32_t virtualAddress, int pages);

/**
 * Creates a thread-local-storage area for a process and copies/zeroes the given amount of bytes
 * from the content.
 *
 * @param process
 * 		the process creation identifier
 * @param content
 * 		master copy content
 * @param copysize
 * 		number of bytes to copy from content
 * @param zerosize
 * 		total size (including bytes to zero)
 * @param alignment
 * 		tls alignment
 *
 * @return whether writing was successful
 *
 * @security-level KERNEL
 */
uint8_t WriteTlsMasterForProcess(ProcessCreationIdentifier process, uint8_t *content, uint32_t copysize, uint32_t totalsize, uint32_t alignment);

/**
 * Attaches and therefore starts a process during process creation at the given execution point.
 *
 * @param process
 * 		the process creation identifier
 * @param eip
 * 		execution entry point
 *
 * @security-level KERNEL
 */
void AttachCreatedProcess(ProcessCreationIdentifier process, uint32_t eip);

/**
 * Cancels process creation.
 *
 * @param process
 * 		the process creation identifier
 *
 * @security-level KERNEL
 */
void CancelProcessCreation(ProcessCreationIdentifier process);

/**
 * Retrieves the pid for a created process. Only usable if the process was attached.
 *
 * @param process
 * 		the process creation identifier
 *
 * @return the pid of the created process.
 *
 * @security-level KERNEL
 */
Pid GetCreatedProcessID(ProcessCreationIdentifier process);

/**
 * Creates a mountpoint and registers the current thread as its file system delegate.
 *
 * @param name
 * 		the wanted name
 *
 * @param phys_mountpoint_id
 * 		the physical id to set for the mountpoint
 *
 * @param out_mountpoint_id
 * 		is filled with the node id of the mountpoint on success
 *
 * @param out_transaction_storage
 * 		is filled with the address of the transaction storage
 *
 * @return one of the {FsRegisterAsDelegateStatus} codes
 *
 * @security-level DRIVER
 */
FsRegisterAsDelegateStatus FsRegisterAsDelegate(const char *name, FsPhysID physMountpointID, FsVirtID *outMountpointID, Address *outTransactionStorage);

/**
 * Updates the status for a filesystem transaction.
 *
 * @param id
 * 		the transaction id
 *
 * @param status
 * 		the transaction status
 *
 * @security-level DRIVER
 */
void FsSetTransactionStatus(FsTransactionID id, FsTransactionStatus status);

/**
 * Creates a filesystem node.
 *
 * @param parent
 * 		id of the parent node
 *
 * @param name
 * 		the node name
 *
 * @param type
 * 		one of the FsNodeType types
 *
 * @param phys_fs_id
 * 		the filesystem id of the node
 *
 * @param out_created_id
 * 		id of the created node
 *
 * @return one of the {FsCreateNodeStatus} codes
 *
 * @security-level DRIVER
 */
FsCreateNodeStatus FsCreateNode(uint32_t parent, char *name, FsNodeType type, uint64_t fsID, uint32_t *outCreatedID);

/**
 * Registers the <handler> routine as the handler for the <irq>.
 *
 * @param irq
 * 		IRQ number to handle
 *
 * @param handler
 * 		handler routine to call
 *
 * @return one of the {RegisterIrqHandlerStatus} codes
 *
 * @security-level DRIVER
 */
RegisterIrqHandlerStatus RegisterIrqHandler(uint8_t irq, void (*handler)(uint8_t));

/**
 * Restores the interruption state (for example after signal/irq handling) of the current thread.
 */
void RestoreInterruptedState();

/**
 * Registers the <handler> routine as the handler for the <irq>.
 *
 * @param signal
 * 		signal to handle
 *
 * @param handler
 * 		handler routine to call
 *
 * @return
 * 		previously registered handler
 *
 * @security-level DRIVER
 */
void *RegisterSignalHandler(int signal, void *handler);

/**
 * Raises the <signal> in the <process>.
 *
 * @param process
 * 		target process
 *
 * @param signal
 * 		signal number
 *
 * @return one of the {RaiseSignalStatus} codes
 */
RaiseSignalStatus RaiseSignal(Pid process, int signal);

/**
 * Executes the given query for kernel.
 *
 * @param command
 * 		query command
 *
 * @param query
 * 		input query
 *
 * @param outbuffer
 * 		output buffer
 *
 * @return one of the {EvaQueryStatus} codes
 */
EvaQueryStatus EvaQuery(uint16_t command, const uint8_t *query, uint8_t *outbuffer);

/**
 * Return EvangelionNG version
 * 
 * 
 */
SyscallUname Uname();

/**
 * Return system information
 * 
 * 
 */
SyscallSysinfo Sysinfo();

/**
 * reboot system
 */
void RebootSystem();

/**
 * shutodown acpi system
 */
void ShutdownSystem();

/**
 * play sound frequency
 *
 * @param frequency
 *		frequency that you shoud play
 *
 */
void Sound(uint32_t frequency);

/**
 * mute sound
 */
void SoundMute();


__END_C

#endif
