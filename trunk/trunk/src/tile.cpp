// File:    tile.cpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream> // cout
#include "tile.hpp"
#include "tiles.hpp"
#ifdef _GUI
#include "gui/topwindow.hpp"
#endif

// attribute utility functions

string attributeToString(AIndex ind, AValue value) {
    ASSERT(ind < Tile::getNumAttributes());
    ASSERT(value <= Tile::getMaxAttribute(ind));
    string result;

    switch (ind) {
        case 0:
            result = (char)('A' + value);
            break;
        case 1:
            result = (char)('Q' + value);
            break;
        case 2:
            result = (char)('a' + value);
            break;
        case 3:
            result = (char)('q' + value);
            break;
        default:
            result = (char)('1' + value);
            break;
    }

    return result;
}

AValue charToAttribute(AIndex ind, char ch) {
    ASSERT(ind < Tile::getNumAttributes());
    AValue result;

    switch (ind) {
        case 0:
            result = (AValue)(ch - 'A');
            break;
        case 1:
            result = (AValue)(ch - 'Q');
            break;
        case 2:
            result = (AValue)(ch - 'a');
            break;
        case 3:
            result = (AValue)(ch - 'q');
            break;
        default:
            result = (AValue)(ch - '1');
            break;
    }

    if (result > Tile::getMaxAttribute(ind)) {
        result = 0;
    }
    
    return result;
}

// static private data

AValue *Tile::_maxAttribute = NULL;
unsigned Tile::_nextId = 0;
ACount Tile::_numAttributes = 0;

// static methods

// display an empty cell
void Tile::displayEmpty(void) {
#ifdef _CONSOLE
    cout << " ." << toStringEmpty() << ".";
#endif
}

AValue Tile::getMaxAttribute(AIndex attr) {
    ASSERT(attr < _numAttributes);
    AValue result = _maxAttribute[attr];
    
    return result;
}

ACount Tile::getNumAttributes(void) {
    ASSERT(_numAttributes >= 2);
    ACount result = _numAttributes;

    return result;
}

void Tile::setStatic(ACount na, AValue const maxAttr[]) {
    ASSERT(na >= 2);
#ifdef _GUI
    ASSERT(na <= 4);
#endif
    _numAttributes = na;
    
    _maxAttribute = new AValue[na];
    for (AIndex i = 0; i < na; i++) {
        ASSERT(maxAttr[i] >= 3);
        ASSERT(maxAttr[i] <= 10);
        _maxAttribute[i] = maxAttr[i];
    }
}

string Tile::toStringEmpty(void) {
    string result(_numAttributes, '.');

    return result;
}

// constructors and destructor

Tile::Tile(void) {
    ASSERT(_numAttributes >= 2);
    _arr = new AValue[_numAttributes];

    for (AIndex i = 0; i < _numAttributes; i++) {
        _arr[i] = 0;
    }
    _id = _nextId++;
}

// Mint a new tile based on a string.
Tile::Tile(string const &str) {
    ASSERT(_numAttributes >= 2);
    _arr = new AValue[_numAttributes];

    AIndex ind = 0;

    string::const_iterator c;
    for (c = str.begin() ; c != str.end(); c++) {
        if (ind < _numAttributes) {
            char ch = *c;
            _arr[ind] = charToAttribute(ind, ch);
            ind++;
        }
    }

    while (ind < _numAttributes) {
        _arr[ind] = 0;
        ind++;
    }
    
    _id = _nextId++;

    ASSERT(isValid());
}

// construct a copy (with the same id)
Tile::Tile(Tile const &base) {
    ASSERT(_numAttributes >= 2);
    _arr = new AValue[_numAttributes];
    for (AIndex i = 0; i < _numAttributes; i++) {
        _arr[i] = base._arr[i];
    }
    _id = base._id;
}

// destroy a tile
Tile::~Tile(void) {
    //TODO delete[] arr;
}

// public member functions

// create a clone (with a different id)
Tile Tile::clone(void) const {
    Tile result = Tile(*this);
    result._id = _nextId++;
    
    return result;
}

unsigned Tile::commonAttribute(Tile const &other) const {
    ASSERT(numMatchingAttributes(other) == 1);
    
    AIndex result;
    for (AIndex ind = 0; ind < _numAttributes; ind++) {
        if (_arr[ind] == other._arr[ind]) {
            result = ind;
            break;
        }
    }
    return result;
}

void Tile::display(void) const {
    cout << " [" << this->toString() << "]";

}

AValue Tile::getAttribute(AIndex ind) const {
    ASSERT(ind < _numAttributes);
    AValue result = _arr[ind];
    
    return result;
}

string Tile::getUserChoice(Tiles const &availableTiles, Strings const &alts) {
	string result;

    while (true) {
        cout << "Enter a tile name";
    	Strings::const_iterator alt;
		for (alt = alts.begin(); alt != alts.end(); alt++) {
			cout << " or '" << *alt << "'";
		}
		cout << ": ";

        cin >> result;
		if (alts.find(result) != alts.end()) {
			break;
		}

        *this = Tile(result);
        ASSERT(isValid());
        if (result != this->toString()) {
            cout << "'" << result << "' is invalid." << endl;
        } else if (!isCloneAny(availableTiles)) {
            cout << result << " is unavailable." << endl;
        } else {
            availableTiles.unClone(*this);
            break;
        }
    }

	return result;
}

bool Tile::hasAttribute(AIndex ind, AValue value) const {
    ASSERT(ind < _numAttributes);
    AValue attrib = _arr[ind];
    bool result = (attrib == value);
    
    return result;
}

bool Tile::isClone(Tile const &other) const {
    bool result = false;
    
    if (_id != other._id) {
        result = true;
        for (AIndex i = 0; i < _numAttributes; i++) {
            if (_arr[i] != other._arr[i]) {
                result = false;
                break;
            }
        }
    }

    D(cout << "Tile::isClone(" << toString() << ", " << other.toString()
        << ") returns " << result << endl);
    return result;
}

bool Tile::isCloneAny(Tiles const &s) const {
    bool result = false;
    
    Tiles::const_iterator tile;
    for (tile = s.begin(); tile != s.end(); tile++) {
        if (isClone(*tile)) {
            result = true;
            break;
        }
    }
    
    return result;
}

bool Tile::isCompatibleWith(Tile const *other) const {
    bool result = true;
    
    if (other != NULL) {
        ACount matches = numMatchingAttributes(*other);
        result = (matches == 1);
        D(cout << "Tile::isCompatibleWith(" << this->toString() << ", " << other->toString()
            << ") returns " << result << endl);
    }
    
    return result;
}

bool Tile::isValid(void) const {
    bool result = false;
    
    if (_id < _nextId) {
        result = true;
    
        for (AIndex ind = 0; ind < _numAttributes; ind++) {
            if (_arr[ind] > _maxAttribute[ind]) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

ACount Tile::numMatchingAttributes(Tile const &other) const {
    unsigned result = 0;
    for (AIndex ind = 0; ind < _numAttributes; ind++) {
        if (hasAttribute(ind, other._arr[ind])) {
             ++result;
        }
    }
    
    return result;
}

bool Tile::operator<(Tile const &other) const {
     bool result = (_id < other._id);
     
     return result;
}

Tile &Tile::operator=(Tile const &other) {
    _arr = new AValue[_numAttributes];
    for (AIndex ind = 0; ind < _numAttributes; ind++) {
        AValue value = other._arr[ind];
        ASSERT(value <= _maxAttribute[ind]);
        _arr[ind] = value;
    }
    _id = other._id;
     
    return *this;
}

bool Tile::operator==(Tile const &other) const {
    bool result = (_id == other._id);
    if (result) {
        for (AIndex i = 0; i < _numAttributes; i++) {
            ASSERT(_arr[i] == other._arr[i]);
        }
    }
    
    return result;
}

void Tile::setAttribute(AIndex ind, AValue value) {
    ASSERT(ind < _numAttributes);
    ASSERT(value <= _maxAttribute[ind]);
    _arr[ind] = value;
}

string Tile::toString(void) const {
    string result;

    for (AIndex ind = 0; ind < _numAttributes; ind++) {
        AValue value = _arr[ind];
        ASSERT(value <= _maxAttribute[ind]);

        result += attributeToString(ind, value);
    }
    
    return result;
}
