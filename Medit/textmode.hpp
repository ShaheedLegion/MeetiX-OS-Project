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

#ifndef _MediT_TextMode_H_
#define _MediT_TextMode_H_

#include <unistd.h>
#include <string>

class TextMode_t
{
public:
	/*constructor*/
	TextMode_t()
	{

	}

	/*main of textmode*/
	void TextMode(int argc, char *argv[]);

	/*register task identifier*/
	void prepare();

	/*clear the screen and change color*/
	void format_space();

	/*write Medit logo, write version*/
	void create_intro();


	/*Medit help*/
	void help();


	/*read and write on monitor the file content*/
	void read_file_content(FILE *fp);

	/*write modify or content of file created/opened*/
	void type_file(FILE *fp);

	/*save the content to file*/
	void save_file(std::string text, FILE *fp);

	/*close the file*/
	void close_file(FILE *fp);
};

#endif