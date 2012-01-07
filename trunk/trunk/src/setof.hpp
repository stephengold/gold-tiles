#ifndef SETOF_HPP
class SetIterator {
	unsigned current, max;

	public:
		SetIterator(unsigned);
		void bump(void);
		unsigned getIndex(void);
		bool isMore(void);
};

template <class T> class SetOf {
	T ** arr;
	unsigned count, space;

	void deleteInd(unsigned);
	int find(T &);
	void grow(unsigned);

	public:
		SetOf<T>(void);
		void add(T &);
		void del(T &);
		T & get(SetIterator &);
		unsigned getCount(void);
		bool isEmpty(void);
		SetIterator & newIter(void);
		T & pullRandom(void);
};
#define SETOF_HPP
#endif
