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

#include <memory/memory.hpp>
#include <utils/string.hpp>

/**
 *
 */
void String::concat(const char *a, const char *b, char *out) 
{
	int lenA = String::length(a);
	int lenB = String::length(b);
	
	Memory::copy(out, a, lenA);
	Memory::copy(&out[lenA], b, lenB);
	
	out[lenA + lenB] = 0;
}

/**
 *
 */
void String::copy(char *target, const char *source) 
{
	int len = length(source);
	Memory::copy(target, source, len);
	target[len] = 0;
}

/**
 *
 */
int String::length(const char *str) 
{
	int size = 0;
	while (*str++) 
	{
		++size;
	}
	return size;
}

/**
 *
 */
int String::indexOf(const char *str, char c) 
{
	int pos = 0;
	while (*str) 
	{
		if (*str == c) return pos;

		++pos;
		++str;
	}
	return -1;
}

/**
 * Returns if the given cstring equals this string
 */
bool String::equals(const char *stra, const char *strb) 
{

	if (stra == strb)
		return true;

	int alen = length(stra);
	int blen = length(strb);

	if (alen != blen) return false;

	while (alen-- > 0) 
	{
		if (stra[alen] != strb[alen]) return false;
	}
	return true;
}

/**
 * Replaces all occurences of char a with char b in the given cstring
 */
void String::replace(char *str, char character, char replacement) 
{
	for (uint32_t i = 0;; i++) 
	{
		if (str[i] == 0) break;
		if (str[i] == character) str[i] = replacement;
	}
}

