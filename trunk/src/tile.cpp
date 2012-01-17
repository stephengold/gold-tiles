// File:    tile.cpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream> // cout
#include <sstream>
#include "tile.hpp"
#include "tiles.hpp"

// static members

unsigned Tile::nextId = 0;
unsigned Tile::numAttributes;
unsigned *Tile::maxAttribute;
unsigned Tile::tileMagic = 40779;

unsigned Tile::getNumAttributes(void) {
    return numAttributes;
}

unsigned Tile::getMaxAttribute(unsigned attr) {
    return maxAttribute[attr];
}

void Tile::setStatic(unsigned na, unsigned const maxAttr[]) {
    ASSERT(na >= 2);
    numAttributes = na;
    
    maxAttribute = new unsigned[na];
    for (int i = 0; i < na; i++) {
        ASSERT(maxAttr[i] >= 3);
        maxAttribute[i] = maxAttr[i];
    }
}

// constructors and destructor

Tile::Tile(void) {
    arr = new unsigned[numAttributes];
    for (unsigned i = 0; i < numAttributes; i++) {
        arr[i] = 0;
    }
    id = nextId++;
    magic = tileMagic;
}

// forge a tile based on a string
Tile::Tile(string const &str) {
    string::const_iterator c;
    for (c = str.begin(); c != str.end(); c++) {
        //TODO
    }
}

// construct a copy (with the same id)
Tile::Tile(Tile const &old) {
    arr = new unsigned[numAttributes];
    for (unsigned i = 0; i < numAttributes; i++) {
        arr[i] = old.arr[i];
    }
    id = old.id;
    magic = tileMagic;
}

// destroy a tile
Tile::~Tile(void) {
    //delete[] arr;
}

// public member functions

// allocate a clone with a different id
Tile Tile::clone(void) const {
    Tile result = Tile(*this);
    result.id = nextId++;
    
    return result;
}

void Tile::display(void) const {
    cout << " " << this->toString();
}

void Tile::displayEmpty(void) {
    cout << " .";
    for (unsigned i = 0; i < numAttributes; i++) {
        cout << '.';
    }
    cout << '.';
}

unsigned Tile::getAttribute(unsigned ind) const {
    ASSERT(ind < numAttributes);
    unsigned result = arr[ind];
    
    return result;
}

bool Tile::hasAttribute(unsigned ind, unsigned value) const {
    ASSERT(ind < numAttributes);
    unsigned attrib = arr[ind];
    bool result = (attrib == value);
    
    return result;
}

bool Tile::isClone(Tile const &other) const {
    bool result = false;
    
    if (id != other.id) {
        result = true;
        for (unsigned i = 0; i < numAttributes; i++) {
            if (arr[i] != other.arr[i]) {
                result = false;
                break;
            }
        }
    }

#if 0
    cout << "Tile::isClone(" << toString() << ", " << other.toString();
    cout << ") returns " << result << endl;
#endif    
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
        unsigned mismatches = 0; 
        for (unsigned i = 0; i < numAttributes; i++) {
            if (arr[i] != other->arr[i]) {
                 ++mismatches;
            }
        }
        result = (mismatches == numAttributes - 1);
    }
    
    return result;
}

bool Tile::operator<(Tile const &other) const {
     bool result = (id < other.id);
     
     return result;
}

Tile &Tile::operator=(Tile const &other) {
    arr = new unsigned[numAttributes];
    for (unsigned i = 0; i < numAttributes; i++) {
        arr[i] = other.arr[i];
    }
    id = other.id;
     
    return *this;
}

bool Tile::operator==(Tile const &other) const {
    bool result = (id == other.id);
    if (result) {
        for (unsigned i = 0; i < numAttributes; i++) {
            ASSERT(arr[i] == other.arr[i]);
        }
    }
    
    return result;
}

void Tile::setAttribute(unsigned ind, unsigned value) {
    ASSERT(ind < numAttributes);
    ASSERT(value <= maxAttribute[ind]);
    arr[ind] = value;
}

string Tile::toString(void) const {
    ostringstream sout;

    ASSERT(numAttributes > 1);
        
    sout << "[";
    for (unsigned ind = 0; ind < numAttributes; ind++) {
        unsigned value = arr[ind];
        sout << attributeToString(ind, value);
    }
    sout << "]";
#if 0
    sout << id;
#endif    
    string result = sout.str();
    
    return result;
}

// attribute utilities

string attributeToString(unsigned ind, unsigned value) {
    string result;
    
    switch (ind) {
        case 0:
            result = (char)('A' + value);
            break;
        case 1:
            result = (char)('M' + value);
            break;
        case 2:
            result = (char)('a' + value);
            break;
        case 3:
            result = (char)('m' + value);
            break;
        default:
            result = (char)('1' + value);
            break;
    }
    
    return result;
}

// Tile* utilities

bool isValid(Tile const *ptr) {
    bool result = true;
    
    if (ptr != NULL) {
        result = (ptr->magic == Tile::tileMagic);
    }
    return result;
}
