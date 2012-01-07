#ifndef GRIDOF_HPP
#define GRIDOF_HPP

#include <cstring>
#include <iostream>

class GridRef {
	int easting, northing;

	public:
		GridRef(int n, int e);
		int getE(void) const;
		int getN(void) const;
		GridRef toDirection(unsigned) const;
};

template <class T> class GridOf {
	T ***arr;
	unsigned maxE, maxN, maxS, maxW;

	void grow(GridRef &);

	public:
		GridOf<T>(void);
		void display(void) const;
		T * get(GridRef &);
		bool isClear(GridRef &) const;
		void print(void) const;
		void set(GridRef &, T *);
};

template <class T> void GridOf<T>::grow(GridRef &ref) {
	int n = ref.getN();
	int e = ref.getE();

	int maxE = this->maxE;
	int maxN = this->maxN;
	int maxS = this->maxS;
	int maxW = this->maxW;

	bool copy = false;

	if (n < -maxS) {
		maxS = -n;
		copy = true;
    }
    if (n > maxN) {
	    maxN = n;
		copy = true;
    }
    if (e < -maxW) {
	    maxW = -e;
		copy = true;
    }
    if (e > maxE) {
	    maxE = e;
		copy = true;
    }

    if (copy) {
	    T ***old = this->arr;

        this->arr = new T** [maxN + maxS + 1];
        for (int i = 0; i <= maxN + maxS; i++) {
        	this->arr[i] = new T* [maxE + maxW + 1];
            for (int j = 0; j <= maxE + maxW; j++) {
		        int ii = i - maxS + this->maxS;
                int jj = j - maxW + this->maxW;
		        if (ii >= 0 ||
                    ii <= this->maxN + this->maxS ||
                    jj >= 0 ||
                    jj <= this->maxE + this->maxW)
                {
                    this->arr[i][j] = old[ii][jj];
                } else {
		            this->arr[i][j] = NULL;
                }
            } // j loop
		} // i loop

		this->maxE = maxE;
		this->maxN = maxN;
		this->maxS = maxS;
		this->maxW = maxW;
	} // if copy
}

template <class T> GridOf<T>::GridOf<T>(void) {
	this->maxE = 0;
	this->maxN = 0;
	this->maxS = 0;
	this->maxW = 0;
	this->arr = new (T **);
    this->arr[0] = new (T *);
	this->arr[0][0] = NULL;
}

template <class T> void GridOf<T>::display(void) const {
    for (int i = 0; i <= this->maxN + this->maxS; i++) {
	    for (int j = 0; j <= this->maxE + this->maxW; j++) {
			T * ptr = this->arr[i][j];
			if (ptr == NULL) {
				T::displayEmpty();
			} else {
				ptr->display();
			}
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

template <class T> T * GridOf<T>::get(GridRef &ref) {
	this->grow(ref);

	int i = ref.getN() + this->maxS;
	int j = ref.getE() + this->maxW;
	T *ptr = this->arr[i][j];

	return ptr;
}

template <class T> bool GridOf<T>::isClear(GridRef & ref) const {
	T *ptr = this->get(ref);

	return (ptr == NULL); 
}

template <class T> void GridOf<T>::set(GridRef &ref, T *ptr) {
	this->grow(ref);

	int i = ref.getN() + this->maxS;
	int j = ref.getE() + this->maxW;
	this->arr[i][j] = ptr;
}

#endif
