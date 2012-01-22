#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

// File:    tile.hpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*
 A Tile object represents a playing piece which may be played
 on a Board object or stored in a bag or hand (Tiles).  In the
 context of a particular game, every Tile has the same number 
 of attributes.  Distinct tiles with identical attributes 
 ("clones") are possible; a unique id is used to distinguish 
 between clones.
*/

#include "project.hpp"
#include <set>
#include <string>

// attribute types and utilities
typedef unsigned ACount, AIndex;
typedef unsigned AValue;

string attributeToString(AIndex, AValue);
AValue charToAttribute(AIndex, char ch);

// a set of strings
class Strings: public set<string> {};


class Tile {
    // static private data
	static AValue *_maxAttribute;  // number of values for each tile attribute
	static unsigned _nextId;
	static AIndex _numAttributes;  // number of attributes for each tile

    // private data
	AValue *_arr;
	unsigned _id;

	public:
        // static methods
        static void displayEmpty(void);
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
		void display(void) const;
		AValue getAttribute(AIndex) const;
		string getUserChoice(Tiles const &, Strings const &);
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
