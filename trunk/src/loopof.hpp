#include <cstring>

class LoopIterator {
	unsigned current, max;

	public:
		LoopIterator(unsigned);
		void bump(void);
		unsigned getIndex(void);
		bool isWrap(void);
};

template <class T> class LoopOf {
	T ** arr;
	unsigned count;

	void grow(unsigned);

	public:
		LoopOf<T>(void);
        void add(T &);
        T & get(LoopIterator &);
        LoopIterator & newIter(void);
};

template <class T> void LoopOf<T>::grow(unsigned ind) {
	if (ind < this->count) {
		return;
	}

	T ** old_arr = this->arr;
	unsigned old_count = this->count;

    this->count = ind + 1;
	this->arr = (T **) operator new (sizeof(T *) * this->count);
	for (unsigned i = 0; i < old_count; i++) {
		this->arr[i] = old_arr[i];
	}
	for (unsigned i = old_count; i <= ind; i++) {
		this->arr[i] = NULL;
	}
}

template <class T> LoopOf<T>::LoopOf<T>(void) {
	this->arr = NULL;
	this->count = 0;
}

template <class T> void LoopOf<T>::add(T & element) {
	unsigned ind = this->count;

	this->grow(ind);
	this->arr[ind] = element;
}

template <class T> T & LoopOf<T>::get(LoopIterator & iter) {
	unsigned ind = iter.getIndex();

	this->grow(ind);
	return this->arr[ind];
}

template <class T> LoopIterator & LoopOf<T>::newIter(void) {
	LoopIterator *iter = new LoopIterator(this->count);

	return *iter;
}
