// File:    tile.cpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream> // std::cout
#include "tile.hpp"

unsigned Tile::blankAttribute = 0;
unsigned Tile::nextId = 0;
unsigned Tile::numAttributes = 0;

void Tile::setNumAttributes(unsigned na) {
    numAttributes = na;
}

// construct a blank tile
Tile::Tile(void) {
    arr = new unsigned[numAttributes];
    for (unsigned i = 0; i < numAttributes; i++) {
        arr[i] = blankAttribute;
    }
    id = nextId++;
}

// construct a copy
Tile::Tile(const Tile &t) {
    this->arr = new unsigned[numAttributes];
    for (unsigned i = 0; i < numAttributes; i++) {
        arr[i] = t.arr[i];
    }
    id = nextId++;
}

// destroy a tile
Tile::~Tile(void) {
    delete[] arr;
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
