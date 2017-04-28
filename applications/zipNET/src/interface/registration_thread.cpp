/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "registration_thread.hpp"
#include <mxuser/ui/interface_specification.hpp>
#include <interface/command_message_receiver_thread.hpp>
#include <interface/application_exit_cleanup_thread.hpp>
#include <stdio.h>

/**
 *
 */
void RegistrationThread_t::run() 
{
	TaskRegisterID("zipNET/RegistrationThread");

	Tid tid = GetTid();

	// register this thread
	if (!TaskRegisterID(UI_REGISTRATION_THREAD_IDENTIFIER)) 
	{
		klog("failed to register task identifier for registration thread");
		return;
	}

	// wait for initialization requests
	size_t bufferLength = sizeof(MessageHeader) + sizeof(UiInitializeRequest);
	uint8_t *buffer = new uint8_t[bufferLength];

	while (true) 
	{
		MessageReceiveStatus stat = ReceiveMessage(buffer, bufferLength);

		if (stat == MESSAGE_RECEIVE_STATUS_SUCCESSFUL) 
		{
			MessageHeader *requestMessage = (MessageHeader*) buffer;
			UiInitializeRequest *request = (UiInitializeRequest*) MESSAGE_CONTENT(buffer);

			// create handler thread
			CommandMessageReceiverThread_t *communicator = new CommandMessageReceiverThread_t();
			Tid communicatorTid = communicator->start();

			// create a thread that cleans up the ui when the client thread exits
			ApplicationExitCleanupThread_t *cleanup = new ApplicationExitCleanupThread_t(GetPidForTid(requestMessage->sender), communicator);
			cleanup->start();

			// send response
			UiInitializeResponse response;
			response.header.id = UI_PROTOCOL_INITIALIZATION;
			response.status = UI_PROTOCOL_SUCCESS;
			response.windowServerDelegateThread = communicatorTid;
			SendMessageT(requestMessage->sender, &response, sizeof(UiInitializeResponse), requestMessage->transaction);
		}
	}
}
