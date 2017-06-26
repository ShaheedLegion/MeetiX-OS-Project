/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
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


#ifndef _LINKED_LIST_
#define _LINKED_LIST_

#include <stdint.h>

/*
 * template list with internal linked list
 */
template<class Type> class llist
{
public:
	/*
	 * node descriptor
	 */
	struct ListNode
	{
		// constructor
		ListNode(Type value) : value(value), next(0), prev(0) {}

		// value
		Type value;

		// pointers
		ListNode *next;
		ListNode *prev;
	} __attribute__((packed));

	/*
	 * internal iterator of class
	 */
	class iterator
	{
	public:
		/*
		 *	empty constructor
		 */
		iterator() : list(0), _current(0), _index(0) {}

		/*
		 *	initialized constructor
		 */
		iterator(llist *list) : list(list), _current(0), _index(0)
		{
			if (list->_firstNode) _current = list->_firstNode;
		}

		/*
		 *	initialized constructor
		 */
		iterator(const iterator &it) : list(it.list), _current(it._current), _index(it._index) {}

		/*
		 *	increment operator
		 */
		iterator &operator++()
		{
			// if there is the next set it
			if (_current->next && _index < list->_size)
			{
				_current = _current->next;
				_index++;
			}

			// reset current entry
			else _current = 0;

			// update object
			return *this;
		}

		/*
		 * increment current entry of provided size
		 */
		iterator &operator+(int size)
		{
			return increment(size);
		}

		/*
		 * increment and assign current entry of provided size
		 */
		iterator &operator+=(int size)
		{
			return (*this = increment(size));
		}

		/*
		 *	access operator
		 */
		ListNode *operator->() { return _current; }
		Type operator*() { return (_current->value); }

		/**
		 *	operator overload
		 */
		friend bool operator==(const iterator &lhs, const iterator &rhs) { return lhs._current == rhs._current; }
		friend bool operator!=(const iterator &lhs, const iterator &rhs) { return !(lhs == rhs); }

	private:
		llist *list;
		ListNode *_current;
		uint32_t _index;

		/*
		 * increment of size position the current node
		 */
		iterator &increment(int size)
		{
			// instantiate a new iterator
			iterator *it = new iterator(*this);

			// increment of size
			int i = 0;
			while (it->_current && it->_index < list->_size)
			{
				it->_current = it->_current->next;
				i++;

				if (i == size) break;
			}

			// return it's reference
			return *it;
		}

		friend class llist;
	} __attribute__((packed));


	/*
	 * constructor
	 */
	llist() : _size(0), _firstNode(0), _lastNode(0) {}

	/*
	 * destructor delete all nodes
	 */
	~llist() { clear(); }

	/*
	 *	iterator initializer
	 */
	iterator begin() { return iterator(this); }
	iterator end() { return iterator(); }

	/*
	 *	insert a value to back of llist
	 */
	void add(Type value)
	{
		// try to add to back of list
		if (!_firstNode)
		{
			_firstNode = new ListNode(value);
			_lastNode = _firstNode;
			_size++;
		}

		// find the first free bucket
		else
		{
			// start from first
			ListNode *current = _lastNode;
			while (current)
			{
				// if we find a next field free add new node
				if (!current->next)
				{
					current->next = new ListNode(value);  // create new node
					current->next->prev = current; 	// link previous of new node previous

					_lastNode = current->next;	// save as last node the new node
					_size++;  // increment list size
					break;
				}

				// reiterate to next field
				current = current->next;
			}
		}
	}

	/*
	 *	get a value from llist
	 */
	Type operator[](uint32_t position)
	{
		// if is requested first index and list have almost one node return value
		if (!position && _firstNode) return _firstNode->value;

		// else find node requested
		else if (position <= _size)
		{
			ListNode *current = _firstNode != 0 ? _firstNode : 0;
			uint32_t _index = 1;
			while (_index <= position)
			{
				if (current) current = current->next;
				_index++;
			}

			// return value if node is valid
			return current->value;
		}

		// return empty constructor of type elsewhere
		return Type();
	}

	/*
	 *	remove a node from iterator
	 */
	void erase(const iterator &it)
	{
		// get node from iterator
		ListNode *toErase = it._current;

		// remove it
		remove(toErase);
	}

	/*
	 *	remove node from position
	 */
	void erase(uint32_t position)
	{
		// find node
		ListNode *toErase = _firstNode;

		// if position is major than 0 find node, else the node to erase is the first
		if (position > 0)
		{
			uint32_t i = 0;
			while (position < _size && toErase)
			{
				toErase = toErase->next;
				i++;

				if (i == position) break;
			}
		}

		// remove node
		remove(toErase);
	}

	/*
	 *	delete all nodes
	 */
	void clear()
	{
		ListNode *current = _firstNode;
		ListNode *next = 0;

		while (current)
		{
			next = 0;
			if (current->next) next = current->next;

			delete current;
			current = next;
		}
		_size = 0;
	}

	/*
	 * sort the nodes with comparison algoritm provided
	 */
	void sort(bool (*comparison)(const Type &first, const Type &second))
	{
		selectionSort(comparison);
	}

	/*
	 * return llist size
	 */
	inline uint32_t count() const { return _size; }

private:
	// number of nodes into this llist
	uint32_t _size;

	ListNode *_firstNode; // first added node
	ListNode *_lastNode;  // last added node

	/*
	 *	remove provided node
	 */
	void remove(ListNode *node)
	{
		// get next and previous if exists
		ListNode *next = node->next != 0 ? node->next : 0;
		ListNode *prev = node->prev != 0 ? node->prev : 0;

		// check if node to erase is the first or the last node
		if (node == _firstNode) _firstNode = next;
		else if (node == _lastNode) _lastNode = prev;
		else // else link next and prev
		{
			prev->next = next;
			next->prev = prev;
		}

		delete node; // erase node

		_size--; // decrement size
	}

	/*
	 * quick sort algorithm for list sort
	 */
	void quickSort(int arr[], int left, int right, bool (*comparison)(Type first, Type second))
	{
		int i = left, j = right;
		int tmp;
		int pivot = arr[(left + right) / 2];

		/* partition */
		while (i <= j)
		{
		    while (arr[i] < pivot)
		        i++;

		    while (arr[j] > pivot)
		        j--;

		    if (i <= j)
		    {
		        tmp = arr[i];
		        arr[i] = arr[j];
		        arr[j] = tmp;
		        i++;
		        j--;
		    }
		};

		/* recursion */
		if (left < j) quickSort(arr, left, j);
		if (i < right) quickSort(arr, i, right);
	}

	/*
	 * selection sort for list sort
	 */
	void selectionSort(bool (*comparison)(const Type &first, const Type &second))
	{
		Type tmp;

		for (iterator it = this->begin(); it != this->end(); ++it)
		{
			for (iterator it2 = it + 1; it2 != this->end(); ++it2)
			{
				if (comparison(it->value, it2->value))
				{
					tmp = it->value;
					it->value = it2->value;
					it2->value = tmp;
				}
			}
		}
	}

} __attribute__((packed));

#endif
