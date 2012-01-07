// File:    tile.cpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream> // std::cout
#include "tile.hpp"

unsigned Tile::blankAttribute = 0;
unsigned Tile::nextId = 0;
unsigned Tile::numAttributes = 0;

Tile::Tile(void) {
    this->arr = new unsigned[Tile::numAttributes];
    for (unsigned i = 0; i < Tile::numAttributes; i++) {
        this->arr[i] = Tile::blankAttribute;
    }
    this->id = Tile::nextId;
    ++Tile::nextId;
}

Tile::~Tile(void) {
    delete[] arr;
}

bool Tile::compatibleWith(const Tile *tile) const {
    if (tile == NULL) {
        return true;
    }
    
    unsigned mismatches = 0; 
    for (unsigned i = 0; i < Tile::numAttributes; i++) {
        if (this->arr[i] != Tile::blankAttribute &&
            this->arr[i] != tile->arr[i] &&
            tile->arr[i] != Tile::blankAttribute) {
                ++mismatches;
        }
    }
    
    return (mismatches == 1);
}

void Tile::display(void) const {
    std::cout << " [";
    for (unsigned i = 0; i < Tile::numAttributes; i++) {
        std::cout << 'A' + this->arr[i];
    }
    std::cout << ']';
}

void Tile::displayEmpty(void) {
    std::cout << " .";
    for (unsigned i = 0; i < Tile::numAttributes; i++) {
        std::cout << '.';
    }
    std::cout << '.';
}

bool Tile::operator<(const Tile & t) const {
     return (this->id < t.id);
}

void Tile::setAttribute(unsigned ind, unsigned value) {
    this->arr[ind] = value;
}

