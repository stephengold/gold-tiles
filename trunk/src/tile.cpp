// File:    tile.cpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream> // cout
#include "tile.hpp"
#include "tiles.hpp"

// attribute utilities

string attributeToString(AIndex ind, AValue value) {
    ASSERT(ind < Tile::getNumAttributes());
    ASSERT(value <= Tile::getMaxAttribute(ind));
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

AValue charToAttribute(AIndex ind, char ch) {
    ASSERT(ind < Tile::getNumAttributes());
    AValue result;

    switch (ind) {
        case 0:
            result = (AValue)(ch - 'A');
            break;
        case 1:
            result = (AValue)(ch - 'M');
            break;
        case 2:
            result = (AValue)(ch - 'a');
            break;
        case 3:
            result = (AValue)(ch - 'm');
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

AValue *Tile::maxAttribute = NULL;
unsigned Tile::nextId = 0;
ACount Tile::numAttributes = 0;

// static methods

void Tile::displayEmpty(/* User & */) {
    cout << " ." << toStringEmpty() << ".";
}

AValue Tile::getMaxAttribute(AIndex attr) {
    ASSERT(attr < numAttributes);
    AValue result = maxAttribute[attr];
    
    return result;
}

ACount Tile::getNumAttributes(void) {
    ASSERT(numAttributes >= 2);
    ACount result = numAttributes;

    return result;
}

void Tile::setStatic(ACount na, AValue const maxAttr[]) {
    ASSERT(na >= 2);
    numAttributes = na;
    
    maxAttribute = new AValue[na];
    for (AIndex i = 0; i < na; i++) {
        ASSERT(maxAttr[i] >= 3);
        maxAttribute[i] = maxAttr[i];
    }
}

string Tile::toStringEmpty(void) {
    string result;
    for (AIndex i = 0; i < numAttributes; i++) {
        result += '.';
    }

    return result;
}

// constructors and destructor

Tile::Tile(void) {
    ASSERT(numAttributes >= 2);
    arr = new AValue[numAttributes];

    for (AIndex i = 0; i < numAttributes; i++) {
        arr[i] = 0;
    }
    id = nextId++;
}

// forge a new tile based on a string
Tile::Tile(string const &str) {
    ASSERT(numAttributes >= 2);
    arr = new AValue[numAttributes];

    AIndex ind = 0;

    string::const_iterator c;
    for (c = str.begin() ; c != str.end(); c++) {
        if (ind < numAttributes) {
            char ch = *c;
            arr[ind] = charToAttribute(ind, ch);
            ind++;
        }
    }

    while (ind < numAttributes) {
        arr[ind] = 0;
        ind++;
    }
    
    id = nextId++;

    ASSERT(isValid());
}

// construct a copy (with the same id)
Tile::Tile(Tile const &base) {
    ASSERT(numAttributes >= 2);
    arr = new AValue[numAttributes];
    for (AIndex i = 0; i < numAttributes; i++) {
        arr[i] = base.arr[i];
    }
    id = base.id;
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

unsigned Tile::commonAttribute(Tile const &other) const {
    ASSERT(numMatchingAttributes(other) == 1);
    
    AIndex result;
    for (AIndex ind = 0; ind < numAttributes; ind++) {
        if (arr[ind] == other.arr[ind]) {
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
    ASSERT(ind < numAttributes);
    AValue result = arr[ind];
    
    return result;
}

void Tile::getUserChoice(Tiles const &availableTiles) {
    while (true) {
        cout << "Enter a tile name:" << endl;

        string input;
        cin >> input;
        *this = Tile(input);
        ASSERT(isValid());
        if (input != this->toString()) {
            cout << "'" << input << "' is invalid." << endl;
        } else if (!isCloneAny(availableTiles)) {
            cout << input << " is unavailable." << endl;
        } else {
            availableTiles.unClone(*this);
            break;
        }
    }
}

bool Tile::hasAttribute(AIndex ind, AValue value) const {
    ASSERT(ind < numAttributes);
    AValue attrib = arr[ind];
    bool result = (attrib == value);
    
    return result;
}

bool Tile::isClone(Tile const &other) const {
    bool result = false;
    
    if (id != other.id) {
        result = true;
        for (AIndex i = 0; i < numAttributes; i++) {
            if (arr[i] != other.arr[i]) {
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
    
    if (id < nextId) {
        result = true;
    
        for (AIndex ind = 0; ind < numAttributes; ind++) {
            if (arr[ind] > maxAttribute[ind]) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

ACount Tile::numMatchingAttributes(Tile const &other) const {
    unsigned result = 0;
    for (AIndex ind = 0; ind < numAttributes; ind++) {
        if (hasAttribute(ind, other.arr[ind])) {
             ++result;
        }
    }
    
    return result;
}

bool Tile::operator<(Tile const &other) const {
     bool result = (id < other.id);
     
     return result;
}

Tile &Tile::operator=(Tile const &other) {
    arr = new AValue[numAttributes];
    for (AIndex ind = 0; ind < numAttributes; ind++) {
        AValue value = other.arr[ind];
        ASSERT(value <= maxAttribute[ind]);
        arr[ind] = value;
    }
    id = other.id;
     
    return *this;
}

bool Tile::operator==(Tile const &other) const {
    bool result = (id == other.id);
    if (result) {
        for (AIndex i = 0; i < numAttributes; i++) {
            ASSERT(arr[i] == other.arr[i]);
        }
    }
    
    return result;
}

void Tile::setAttribute(AIndex ind, AValue value) {
    ASSERT(ind < numAttributes);
    ASSERT(value <= maxAttribute[ind]);
    arr[ind] = value;
}

string Tile::toString(void) const {
    string result;

    for (AIndex ind = 0; ind < numAttributes; ind++) {
        AValue value = arr[ind];
        ASSERT(value <= maxAttribute[ind]);

        result += attributeToString(ind, value);
    }
    
    return result;
}
