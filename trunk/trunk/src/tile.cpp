// File:    tile.cpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream> // cout
#include "tile.hpp"

// static members
unsigned Tile::blankAttribute = 0;
unsigned Tile::nextId = 0;
unsigned Tile::numAttributes;
unsigned *Tile::maxAttribute;

unsigned Tile::getNumAttributes(void) {
    return numAttributes;
}

unsigned Tile::getMaxAttribute(unsigned attr) {
    return maxAttribute[attr];
}

void Tile::setStatic(unsigned na, unsigned maxAttr[]) {
    numAttributes = na;
    maxAttribute = new unsigned[na];
    for (int i = 0; i < na; i++) {
        maxAttribute[i] = maxAttr[i];
    }
}

// construct a blank tile
Tile::Tile(void) {
    arr = new unsigned[numAttributes];
    for (unsigned i = 0; i < numAttributes; i++) {
        arr[i] = blankAttribute;
    }
    id = nextId++;
}

// construct a copy with the same id
Tile::Tile(const Tile &t) {
    arr = new unsigned[numAttributes];
    for (unsigned i = 0; i < numAttributes; i++) {
        arr[i] = t.arr[i];
    }
    id = t.id;
}

// destroy a tile
Tile::~Tile(void) {
    delete[] arr;
}

// construct a clone with a different id
Tile *Tile::clone(void) const {
    Tile *clo = new Tile(*this);
    clo->id = nextId++;
    return clo;
}

bool Tile::compatibleWith(const Tile *tile) const {
    if (tile == NULL) {
        return true;
    }
    
    unsigned mismatches = 0; 
    for (unsigned i = 0; i < numAttributes; i++) {
        if (arr[i] != blankAttribute &&
            arr[i] != tile->arr[i] &&
            tile->arr[i] != blankAttribute) {
                ++mismatches;
        }
    }
    
    return (mismatches == 1);
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
    return arr[ind];
}

bool Tile::hasAttribute(unsigned ind, unsigned value) const {
    unsigned attrib = arr[ind];
    return (attrib == value || attrib == blankAttribute);
}

bool Tile::operator<(const Tile &t) const {
     return (id < t.id);
}

void Tile::setAttribute(unsigned ind, unsigned value) {
    arr[ind] = value;
}

string Tile::toString(void) const {
    string result = string("[");
    for (unsigned i = 0; i < numAttributes; i++) {
        unsigned attrib = arr[i];
        if (attrib == blankAttribute) {
            result = result + "-";
        } else {
            result = result + (char)('@' + attrib);
        }
    }
    return *new string(result + ']');    
}
