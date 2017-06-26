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

#include "javascript.hpp"
#include "duktape.h"
#include <iostream>

using namespace std;

/**
 *
 */
inline bool endsWith(string const &value, string const &ending)
{
	if (ending.size() > value.size()) return false;
	else return equal(ending.rbegin(), ending.rend(), value.rbegin());
}

/**
 *
 */
int main(int argc, char *argv[])
{
	duk_context *ctx = duk_create_heap_default();
	cout << "Java Script interpreter (Duktape v" << (DUK_VERSION / 10000) << "." << ((DUK_VERSION % 10000) / 100) << "." << (DUK_VERSION % 100) << ")" << endl;

	while (true)
	{
		cout << ">";
		flush(cout);

		string exec;
		getline(cin, exec);

		if (!endsWith(exec, ";"))
		{
			while (true)
			{
				cout << "~";
				string content;
				getline(cin, content);
				exec = exec + content;

				if (content == "") break;
			}
		}

		if (duk_peval_string(ctx, exec.c_str()))
		{
			cout << (char) 27 << "[31m";
			cout << duk_safe_to_string(ctx, -1) << endl;
			cout << (char) 27 << "[0m";

		}

		else if (duk_get_type(ctx, -1) > DUK_TYPE_UNDEFINED) cout <<  duk_json_encode(ctx, -1) << endl;
	}
	duk_destroy_heap(ctx);

	return 0;
}
