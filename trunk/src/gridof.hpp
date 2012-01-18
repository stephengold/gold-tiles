#if 0
#define GRIDOF_HPP

// File:    gridof.hpp
// Purpose: GridOf template for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridref.hpp"
#include <string>
#include <iostream>
#include <set>

template <class T> class GridOf {
	T ***arr;
	unsigned maxE, maxN, maxS, maxW;

	void grow(int n, int e);
	void grow(const GridRef &);

	public:
		GridOf<T>(void);
		GridOf<T>(GridOf<T> const &);
		~GridOf<T>(void);

        bool areAllEmpty(void) const;
        void clear(int n, int e);
        void clear(GridRef const &ref);
		void display(void) const;
		T *get(int n, int e) const;
		T *get(GridRef const &) const;
		T *get(GridRef const &, unsigned dir) const;
		T *get(GridRef const &, unsigned dir, int count) const;
		set<T> get(set<GridRef> const &) const;
		bool isEmpty(int n, int e) const;
		bool isEmpty(GridRef const &) const;
		GridOf<T> &operator=(GridOf<T> const &);
		void set(int n, int e, T const &);
		void set(GridRef const &, T const &);
		void validate(char const *) const;
};

// private member functions

template <class T> void GridOf<T>::grow(int n, int e) {
#if 0
    cout << "GridOf::grow(" << n << ", " << e << ")" << endl;
#endif
    unsigned oldMaxN = maxN;
    unsigned oldMaxS = maxS;
    unsigned oldMaxE = maxE;
    unsigned oldMaxW = maxW;

	bool copy = false;
    if (n > maxN) {
	    maxN = n;
		copy = true;
    }
	if (n < -maxS) {
		maxS = -n;
		copy = true;
    }
    if (e > maxE) {
	    maxE = e;
		copy = true;
    }
    if (e < -maxW) {
	    maxW = -e;
		copy = true;
    }

    if (copy) {
#if 0
        cout << "copying in GridOf::grow() maxN=" << maxN << " maxS=" << maxS 
            << " maxE=" << maxE << " maxW=" << maxW << endl;
#endif  
	    T ***oldArr = arr;
        arr = new T** [maxN + maxS + 1];
        for (int i = 0; i <= maxN + maxS; i++) {
	        int oldI = i - maxS + oldMaxS;
        	arr[i] = new T* [maxE + maxW + 1];
            for (int j = 0; j <= maxE + maxW; j++) {
                int oldJ = j - maxW + oldMaxW;
		        if (oldI >= 0 &&
                    oldI <= oldMaxN + oldMaxS &&
                    oldJ >= 0 &&
                    oldJ <= oldMaxE + oldMaxW)
                {
                    ASSERT(isValid(oldArr[oldI][oldJ]));
                    arr[i][j] = oldArr[oldI][oldJ];
                } else {
		            arr[i][j] = NULL;
                }
                ASSERT(isValid(arr[i][j]));
            } // j loop
		} // i loop
		
        for (int oldI = 0; oldI <= oldMaxN + oldMaxS; oldI++) {
            delete[] oldArr[oldI];
        }
        delete[] oldArr;
	} // if copy
}

template <class T> void GridOf<T>::grow(const GridRef &ref) {
	int n = ref.getN();
	int e = ref.getE();
	
	grow(n, e);
}

// constructors and destructor

template <class T> GridOf<T>::GridOf<T>(void) {
	maxN = 0;
	maxS = 0;
	maxE = 0;
	maxW = 0;
	arr = new T **;
    arr[0] = new T *;
	arr[0][0] = NULL;
    ASSERT(isValid(arr[0][0]));
}

template <class T> GridOf<T>::GridOf<T>(GridOf<T> const &old) {
	maxN = old.maxN;
	maxS = old.maxS;
	maxE = old.maxE;
	maxW = old.maxW;
	
    arr = new T** [maxN + maxS + 1];
    for (unsigned i = 0; i <= maxN + maxS; i++) {
       	arr[i] = new T* [maxE + maxW + 1];
       	for (unsigned j = 0; j <= maxE + maxW; j++) {
            T *ptr = old.arr[i][j];
            ASSERT(isValid(ptr));
            if (ptr != NULL) {
                ptr = new T(*ptr);
            }
            arr[i][j] = ptr;
            ASSERT(isValid(arr[i][j]));
        }
    }
}

template <class T> GridOf<T>::~GridOf<T>(void) {
    for (unsigned i = 0; i <= maxN + maxS; i++) {
       	for (unsigned j = 0; j <= maxE + maxW; j++) {
            delete arr[i][j];
        }
       	delete[] arr[i];
    }
    delete[] arr;
}

// public members

template <class T> bool GridOf<T>::areAllEmpty(void) const {
    for (unsigned i = 0; i <= maxN + maxS; i++) {
       	for (unsigned j = 0; j <= maxE + maxW; j++) {
            ASSERT(isValid(arr[i][j]));
	        if (arr[i][j] != NULL) {
                return false;
            }
        }
    }
    
    return true;
}

template <class T> void GridOf<T>::clear(int n, int e) {
	grow(n, e);

	unsigned i = n + maxS;
	unsigned j = e + maxW;
	ASSERT(isValid(arr[i][j]));
	arr[i][j] = NULL;
	ASSERT(isValid(arr[i][j]));
}

template <class T> void GridOf<T>::clear(const GridRef &ref) {
	int n = ref.getN();
	int e = ref.getE();
	clear(n, e);
}

template <class T> void GridOf<T>::display(void) const {
}

template <class T> T *GridOf<T>::get(int n, int e) const {
    T *result = NULL;

    if (n <= maxN && n >= -maxS && e <= maxE && e >= -maxW) {
  	    unsigned i = n + maxS;
	    unsigned j = e + maxW;
	    result = arr[i][j];
    }  
   	ASSERT(isValid(result));
#if 0
    cout << "GridOf::get(" << n << ", " << e << ") returns " << (unsigned)result << endl;
#endif   
	return result;
}

template <class T> T *GridOf<T>::get(GridRef const &ref) const {
	int n = ref.getN();
	int e = ref.getE();
	T *result = get(n, e);

	return result;
}

template <class T> T *GridOf<T>::get(GridRef const &ref, unsigned dir) const {
    GridRef neighbor(ref, dir);
	T *result = get(neighbor);

	return result;
}

template <class T> T *GridOf<T>::get(
    const GridRef &ref,
    unsigned dir,
    int count) const
{
    GridRef neighbor(ref, dir, count);
	T *result = get(neighbor);

	return result;
}

template <class T> bool GridOf<T>::isEmpty(int n, int e) const {
	T *ptr = get(n, e);
	bool result = (ptr == NULL);

	return result; 
}

template <class T> bool GridOf<T>::isEmpty(GridRef const &ref) const {
	T *ptr = get(ref);
	bool result = (ptr == NULL);

	return result; 
}

template <class T> GridOf<T> &GridOf<T>::operator=(GridOf<T> const &ref) {
    // TODO
	ASSERT(false); 
}

template <class T> void GridOf<T>::set(int n, int e, T const &t) {
	grow(n, e);

	unsigned i = n + maxS;
	unsigned j = e + maxW;
	arr[i][j] = new T(t);
	ASSERT(isValid(arr[i][j]));
}

template <class T> void GridOf<T>::set(GridRef const &ref, T const &t) {
	int n = ref.getN();
	int e = ref.getE();

	set(n, e, t);
}

template <class T> void GridOf<T>::validate(char const *where) const {
    for (unsigned i = 0; i <= maxN + maxS; i++) {
       	for (unsigned j = 0; j <= maxE + maxW; j++) {
            if (!isValid(arr[i][j])) {
                cout << "Grid validation failed: where=" << where 
                    << ", i=" << i << ", j=" << j << endl;
                ASSERT(false);
            }
        }
    }
}

#endif
