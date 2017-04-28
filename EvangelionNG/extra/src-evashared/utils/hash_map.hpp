/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani  &D. Morandi]
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

#ifndef EVA_SHARED_UTILS_HASHMAP
#define EVA_SHARED_UTILS_HASHMAP

#include <utils/hashable.hpp>

/**
 *
 */
template<typename KeyType, typename ValueType> class HashMap 
{
public:

	/**
	 *	entry for couple of key value
	 */
	class HashMapEntry 
	{
	public:
		HashMapEntry(const KeyType &key, const ValueType &value) : key(key), value(value), next(0) {}

		KeyType key;
		ValueType value;
		HashMapEntry *next;
	};

	/**
	 *	iterator of map
	 */
	class HashMapIterator 
	{
	public:

		/**
		 *
		 */
		HashMapIterator() : _map(0), _bucketIndex(0), _current(0) {}

		/**
		 *
		 */
		HashMapIterator(HashMap *map) : _map(map), _bucketIndex(0), _current(0) 
		{

			for (uint32_t i = 0; i < _map->_bucketCount; ++i) 
			{
				if (_map->_buckets[i] != 0) 
				{
					_bucketIndex = i;
					_current = _map->_buckets[i];
					break;
				}
			}
		}

		/**
		 *
		 */
		HashMapIterator &operator ++ () 
		{

			if (_current->next == 0) 
			{
				_current = 0;

				while (++_bucketIndex < _map->_bucketCount) 
				{
					if (_map->_buckets[_bucketIndex] != 0) 
					{
						_current = _map->_buckets[_bucketIndex];
						break;
					}
				}
			} 

			else _current = _current->next;

			return *this;
		}

		/**
		 *
		 */
		HashMapEntry *operator -> () { return _current; }

		/**
		 *
		 */
		friend bool operator == (const HashMapIterator &lhs, const HashMapIterator &rhs) { return lhs._current == rhs._current; }

		/**
		 *
		 */
		friend bool operator != (const HashMapIterator &lhs, const HashMapIterator &rhs) { return !(lhs == rhs); }

	private:

		HashMap *_map;
		uint32_t _bucketIndex;
		HashMapEntry *_current;
	};

	/**
	 *
	 */
	HashMap() : _size(0), _bucketCount(64), _buckets(new HashMapEntry*[_bucketCount]()) {}

	/**
	 *
	 */
	HashMap(const HashMap &rhs) = delete;

	/**
	 *
	 */
	~HashMap() 
	{
		for (uint32_t i = 0; i < _bucketCount; ++i) 
		{
			HashMapEntry *current = _buckets[i];

			while (current != 0) 
			{
				HashMapEntry *next = current->next;
				delete current;
				current = next;
			}
		}

		delete[] _buckets;
	}

	/**
	 *
	 */
	HashMap &operator=(const HashMap &rhs) = delete;

	/**
	 *
	 */
	HashMapIterator begin() { return HashMapIterator(this); }

	/**
	 *
	 */
	HashMapIterator end() { return HashMapIterator(); }

	/**
	 *
	 */
	HashMapEntry *get(const KeyType &key) const 
	{
		uint32_t bucketIndex = HashAble::hashcode(key) % _bucketCount;
		HashMapEntry *entry = _buckets[bucketIndex];

		while (entry != 0) 
		{
			if (entry->key == key) return entry;

			entry = entry->next;
		}

		return 0;
	}

	/**
	 *
	 */
	void add(const KeyType &key, const ValueType &value) 
	{
		uint32_t bucketIndex = HashAble::hashcode(key) % _bucketCount;
		HashMapEntry *entry = _buckets[bucketIndex];

		if (!entry) 
		{
			_buckets[bucketIndex] = new HashMapEntry(key, value);
			++_size;
		} 

		else 
		{
			while (true)
			{
				if (entry->key == key) 
				{
					entry->value = value;
					++_size;
					break;
				} 

				else if (entry->next == 0) 
				{
					entry->next = new HashMapEntry(key, value);
					++_size;
					break;
				}

				entry = entry->next;
			}
		}
	}

	/**
	 *
	 */
	void erase(const KeyType &key) 
	{
		uint32_t bucketIndex = HashAble::hashcode(key) % _bucketCount;

		HashMapEntry *current = _buckets[bucketIndex];
		HashMapEntry *previous = 0;

		while (current != 0) 
		{
			if (current->key == key) 
			{
				if (!previous) _buckets[bucketIndex] = current->next;
				else previous->next = current->next;

				delete current;
				--_size;
				
				break;
			}

			previous = current;
			current = current->next;
		}
	}

	/**
	 *
	 */
	uint32_t size() const { return _size; }

private:
	uint32_t _size;
	uint32_t _bucketCount;
	HashMapEntry **_buckets;
};

#endif
