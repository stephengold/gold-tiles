#ifndef TILE_HPP
#define TILE_HPP

// File:    tile.hpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"
#include <set>
#include <string>

// attribute types and utilities
typedef unsigned ACount, AIndex;
typedef unsigned AValue;

string attributeToString(AIndex, AValue);
AValue charToAttribute(AIndex, char ch);

class Tile {
    // static private data
	static AValue *maxAttribute;  // number of values for each tile attribute
	static unsigned nextId;
	static AIndex numAttributes;  // number of attributes for each tile

    // private data
	AValue *arr;
	unsigned id;

	public:
        // static methods
        static void displayEmpty(/* User & */);
		static AValue getMaxAttribute(AIndex);
        static ACount getNumAttributes(void);
		static void setStatic(ACount, AValue const maxA[]);
        static string toStringEmpty(void);

        // constructors
		Tile(void);          // construct a blank tile
		Tile(string const &);
		Tile(Tile const &);  // copy a tile
		~Tile(void);
		
		Tile clone(void) const;
		unsigned commonAttribute(Tile const &) const;
		void display(/* User & */) const;
		AValue getAttribute(AIndex) const;
		void getUserChoice(Tiles const &);
		bool hasAttribute(AIndex, AValue) const;
		bool isClone(Tile const &) const;
		bool isCloneAny(Tiles const &) const;
		bool isCompatibleWith(Tile const *) const;
		bool isValid(void) const;
		ACount numMatchingAttributes(Tile const &) const;
		bool operator<(Tile const &) const;
		Tile &operator=(Tile const &);
	    bool operator==(Tile const &) const;
		void setAttribute(AIndex, AValue);
		string toString(void) const;
};

// Tile* utilities
bool isValid(const Tile *);

#endif
