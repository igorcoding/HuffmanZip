#ifndef PQ_MIN_H
#define PQ_MIN_H

#include <exception>

template <typename T, typename ComparatorLess = std::less<T>>
class pq_min
{
private:
	pq_min(ComparatorLess less);
public:
	pq_min(size_t capacity, ComparatorLess less = ComparatorLess());
	~pq_min();
	void insert(const T& elem);
	T min() const;
	void del_min();
	size_t size() const;
	size_t capacity() const;


private:
	void resize(size_t new_capacity);
	void sink(size_t k);
	void swim(size_t k);

private:
	T* _arr;
	size_t _capacity;
	size_t _size;
	ComparatorLess _less;
};


template <typename T, typename ComparatorLess>
pq_min<T, ComparatorLess>::pq_min(ComparatorLess less)
	: _capacity(1),
		_size(0),
		_less(less)
{
	_arr = new T[1];
}

template <typename T, typename ComparatorLess>
pq_min<T, ComparatorLess>::pq_min(size_t capacity, ComparatorLess less)
	: _capacity(capacity + 1),
		_size(0),
		_less(less)
{
	_arr = new T[capacity + 1];
	_arr[0] = T();
}

template <typename T, typename ComparatorLess>
pq_min<T, ComparatorLess>::~pq_min()
{
	delete[] _arr;
	_arr = nullptr;
	_capacity = 0;
	_size = 0;
}


template <typename T, typename ComparatorLess>
size_t pq_min<T, ComparatorLess>::size() const
{
	return _size;
}

template <typename T, typename ComparatorLess>
void pq_min<T, ComparatorLess>::resize(size_t new_capacity)
{
	T* new_arr = new T[new_capacity];
	for (size_t i = 0; i < new_capacity; i++)
		new_arr[i] = T();

	for (size_t i = 0; i < _size; i++)
	{
		new_arr[i] = _arr[i];
	}
	delete[] _arr;
	_arr = new_arr;
	_capacity = new_capacity;
		
}

template <typename T, typename ComparatorLess>
void pq_min<T, ComparatorLess>::insert(const T& elem)
{/*
	if (_size == _capacity)
		resize(_capacity << 1); //double the array size
	*/	
	_arr[++_size] = elem;
	swim(_size);
}

template <typename T, typename ComparatorLess>
void pq_min<T, ComparatorLess>::swim(size_t k)
{
	while (k > 1 && _less(_arr[k], _arr[k/2]))
	{
		std::swap(_arr[k/2], _arr[k]);
		k = k/2;
	}
}

template <typename T, typename ComparatorLess>
void pq_min<T, ComparatorLess>::sink(size_t k)
{
	while (2*k <= _size)
	{
		auto j = 2*k;
		if (j < _size && _less(_arr[j+1], _arr[j])) j++;
		if (!_less(_arr[j], _arr[k])) break;
		std::swap(_arr[j], _arr[k]);
		k = j;
	}
}

template <typename T, typename ComparatorLess>
T pq_min<T, ComparatorLess>::min() const
{
	return _arr[1];
}

template <typename T, typename ComparatorLess>
void pq_min<T, ComparatorLess>::del_min()
{
	std::swap(_arr[1], _arr[_size--]);
	sink(1);
}

#endif //PQ_MIN_H