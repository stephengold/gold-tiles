// File:    board.cpp
// Purpose: Board class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream>
#include <sstream>
#include <string>
#include "board.hpp"
#include "locus.hpp"
#include "play.hpp"
#include "tiles.hpp"
#include "tilesquare.hpp"

class Rows: public set<int> {};
class Columns: public set<int> {};

// constructors, assignment, and destructor

// The compiler-generated default constructor is OK.
// The compiler-generated copy constructor is OK.
// The compiler-generated assignment method is OK.
// The compiler-generated destructor is OK.

// public methods

bool Board::anyConnectToOrigin(Locus const &squares) const {
    bool result = false;
    
    Locus::const_iterator s;
    for (s = squares.begin(); s != squares.end(); s++) {
        Locus done;
        if (connectsToOrigin(*s, done)) {
            result = true;
            break;
        }
    }

    return result;
}

bool Board::areAllCompatible(Locus const &squares) const {
    bool result = true;

    if (squares.size() > 1) {
        Tiles tiles = getAll(squares);
        if (!tiles.areAllCompatible()) {
            result = false;
        }
    }
    
    return result;
}

bool Board::areAllEmpty(Locus const &squares) const {
    bool result = true;
    
    Locus::const_iterator s;
    for (s = squares.begin(); s != squares.end(); s++) {
        if (!isEmptyCell(*s)) {
            result = false;
            break;
        }
    }
    
    return result;
}

bool Board::areAllRowsCompatible(Locus const &squares) const {
    bool result = true;
    
    Rows doneRows;

    Locus::const_iterator s;
    for (s = squares.begin(); s != squares.end(); s++) {
        int row = s->getRow();
        if (doneRows.find(row) == doneRows.end()) {
            if (!isRowCompatible(*s)) {
                result = false;
                break;
            }
            doneRows.insert(row);
        }
    }
    
    return result; 
}

bool Board::areAllColumnsCompatible(Locus const &squares) const {
    D(cout << "Board::areAllColumnsCompatible(" << squares.toString() << ")" << endl);
    bool result = true;
    
    Columns doneColumns;
        
    Locus::const_iterator s;
    for (s = squares.begin(); s != squares.end(); s++) {
        int column = s->getColumn();
        if (doneColumns.find(column) == doneColumns.end()) {
            if (!isColumnCompatible(*s)) {
                result = false;
                break;
            }
            doneColumns.insert(column);
        }
    }
    
    return result; 
}

bool Board::connectsToOrigin(GridRef const &ref, Locus &done) const { // recursive
    bool result = true;
    
    if (!ref.isOrigin()) {
        result = false;
        done.insert(ref);

        for (unsigned dir = FirstDir; dir <= LastDir; dir++) {
            GridRef look(ref, dir, 1);
            if (!isEmptyCell(look) && !done.contains(look) && connectsToOrigin(look, done)) {
                result = true;
                break;
            }
        }
    }
    return result;
}

Tile const *Board::getPtr(int n, int e) const {
    GridRef ref(n, e);
    Tile const *result = getCell(ref);

    return result;
}

Tile Board::getTile(int n, int e) const {
    Tile const *ptr = getPtr(n, e);
    ASSERT(ptr != NULL);
    Tile result = *ptr;

    return result;
}

Tiles Board::getAll(Locus const &squares) const {
    Tiles result;
    
    Locus::const_iterator s;
    for (s = squares.begin(); s != squares.end(); s++) {    
        Tile const *ptr = getCell(*s);
        if (ptr != NULL) {    
            result.insert(*ptr);
        }
    }

    return result;
}

void Board::getRowLimits(
    GridRef const &square,
    int &row,
    int &firstColumn,
    int &lastColumn) const
{
    row = square.getRow();
    
    firstColumn = square.getColumn();
    while (!isEmptyCell(row, firstColumn - 1)) {
        firstColumn--;
    }
    
    lastColumn = square.getColumn();
    while (!isEmptyCell(row, lastColumn + 1)) {
        lastColumn++;
    }
}

void Board::getColumnLimits(
    GridRef const &square,
    int &firstRow,
    int &lastRow,
    int &column) const
{
    column = square.getColumn();

    firstRow = square.getRow();
    while (!isEmptyCell(firstRow - 1, column)) {
        firstRow--;
    }

    lastRow = square.getRow();
    while (!isEmptyCell(lastRow + 1, column)) {
        lastRow++;
    }
}

bool Board::isRowCompatible(GridRef const &square) const {
    int row, firstColumn, lastColumn;
    getRowLimits(square, row, firstColumn, lastColumn);
    ASSERT(firstColumn <= lastColumn);
    bool result = true;   
    
    for (int c1 = firstColumn; c1 <= lastColumn; c1++) {
        Tile t1 = getTile(row, c1);
        for (int c2 = c1 + 1; c2 <= lastColumn; c2++) {
            Tile t2 = getTile(row, c2);
            if (!t1.isCompatibleWith(&t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

bool Board::isColumnCompatible(GridRef const &square) const {
    D(cout << "Board::isColumnCompatible(" << square.toString() << ")" << endl);
    int firstRow, lastRow, column;
    getColumnLimits(square, firstRow, lastRow, column);
    ASSERT(firstRow <= lastRow);
    bool result = true;
    
    for (int r1 = firstRow; r1 <= lastRow; r1++) {
        Tile t1 = getTile(r1, column);
        for (int r2 = r1 + 1; r2 <= lastRow; r2++) {
            Tile t2 = getTile(r2, column);
            if (!t1.isCompatibleWith(&t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

bool Board::isConnectedRow(Locus const &squares) const {
    bool result = true;
    
    if (squares.size() > 1) {
        Locus::const_iterator s = squares.begin();
        int row = s->getRow();
        int maxColumn = s->getColumn();
        int minColumn = s->getColumn();
        for ( ; s != squares.end(); s++) {
            if (row != s->getRow()) {
                return false;
            }
            
            int column = s->getColumn();
            if (column > maxColumn) {
                maxColumn = column;
            }
            if (column < minColumn) {
                minColumn = column;
            }
        }
        
        for (int iCol = minColumn; iCol <= maxColumn; iCol++) {
            if (isEmptyCell(row, iCol)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool Board::isConnectedColumn(Locus const &squares) const {
     bool result = true;
    
    if (squares.size() > 1) {
        Locus::const_iterator s = squares.begin();
        int column = s->getColumn();
        int maxRow = s->getRow();
        int minRow = s->getRow();
        for ( ; s != squares.end(); s++) {
            if (column != s->getColumn()) {
                return false;
            }
            
            int row = s->getRow();
            if (row > maxRow) {
                maxRow = row;
            }
            if (row < minRow) {
                minRow = row;
            }
        }
        
        for (int iRow = minRow; iRow <= maxRow; iRow++) {
            if (isEmptyCell(iRow, column)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool Board::isEmptyCell(int n, int e) const {
    Tile const *ptr = getPtr(n, e);
    bool result = (ptr == NULL);

    return result;
}

void Board::playTile(TileSquare const &ts) {
    GridRef square = ts.getSquare();
    Tile tile = ts.getTile();
    ASSERT(isEmptyCell(square));
    playOnCell(square, tile);
}

unsigned Board::scoreRow(GridRef const &square) const {
    int row, firstColumn, lastColumn;
    getRowLimits(square, row, firstColumn, lastColumn);
    ASSERT(firstColumn <= lastColumn);
    unsigned result = 0;

    if (firstColumn != lastColumn) {
        unsigned len = lastColumn + 1 - firstColumn;
        Tile firstTile = getTile(row, firstColumn);
        Tile lastTile = getTile(row, lastColumn);
        AIndex ind = firstTile.commonAttribute(lastTile);
        unsigned maxLen = 1 + (unsigned)Tile::getMaxAttribute(ind);
        if (len == maxLen) {
            result = 2*len;
        } else {
            result = len;
        }
    }

    return result;
}

unsigned Board::scoreColumn(GridRef const &square) const {
    int firstRow, lastRow, column;
    getColumnLimits(square, firstRow, lastRow, column);
    ASSERT(firstRow <= lastRow);
    unsigned result = 0;

    if (firstRow != lastRow) {
        unsigned len = lastRow + 1 - firstRow;
        Tile firstTile = getTile(firstRow, column);
        Tile lastTile = getTile(lastRow, column);
        AIndex ind = firstTile.commonAttribute(lastTile);
        unsigned maxLen = 1 + (unsigned)Tile::getMaxAttribute(ind);
        if (len == maxLen) {
            result = 2*len;
        } else {
            result = len;
        }
    }

    return result;
}

// public methods

bool Board::isEmptyCell(GridRef const &ref) const {
    Tile const *ptr = getCell(ref);
    bool result = (ptr == NULL);

    return result;
}

bool Board::isLegalPlay(Play const &play) const {
    // a pass (no tiles played or swapped) is always legal
    if (play.isPass()) {
        D(cout << "Legal pass." << endl);
        return true;
    }

    // check for repeated tiles
    if (play.repeatsTile()) {
        D(cout << "Not legal: repeated tile(s)." << endl);
        return false;
    }

	if (play.isSwap()) {
		if (!play.isPureSwap()) {
            D(cout << "Not legal: repeated tile(s)." << endl);
            return false;
		}
        
		D(cout << "Legal swap." << endl);
        return true;
	}

	// check for repeated squares
    if (play.repeatsSquare()) {
        D(cout << "Not legal: repeated square(s)." << endl);
        return false;
    }

    // get the set of board squares to be played
    Locus squares = play.getSquares();

    // make sure all those squares are empty
    if (!areAllEmpty(squares)) {
        D(cout << "Not legal: square(s) already played." << endl);
        return false;
    }

    // make sure the squares lie in a single row or column
    bool singleRow = squares.areAllInSameRow();
    bool singleColumn = squares.areAllInSameColumn();
    if (!singleRow && !singleColumn) {
        D(cout << "Not legal: multiple rows and columns." << endl);
        return false;
    }

    // make sure one of the squares will connect to the origin
    if (!anyConnectToOrigin(squares)) {
        D(cout << "Not legal: no connection to origin." << endl);
        return false;
    }

    // make a copy of the board and place the tiles on it
    Board after(*this);
    after.playTiles(play);

    // make sure there are no empty squares between played tiles
    if (singleRow) {
        if (!after.isConnectedRow(squares)) {
            D(cout << "Not legal: gaps in row." << endl);
            return false;
        }
    } else {
        if (!after.isConnectedColumn(squares)) {
            D(cout << "Not legal: gaps in column." << endl);
            return false;
        }
    }

    // check compatibility of connnected tiles in each row and column played
    if (!after.areAllRowsCompatible(squares)) {
        D(cout << "Not legal: incompatible tiles in same row." << endl);
        return false;
    }
    if (!after.areAllColumnsCompatible(squares)) {
        D(cout << "Not legal: incompatible tiles in same column." << endl);
        return false;
    }

    D(cout << "Legal play." << endl);
    return true;
}

void Board::playTiles(Play const &play) {
    Play::const_iterator ts;
    for (ts = play.begin(); ts != play.end(); ts++) {
        playTile(*ts);
    }
}

unsigned Board::scorePlay(Play const &play) const {
    unsigned result = 0;

    Play::const_iterator ts;
    Rows doneRows;
    Columns doneColumns;
    for (ts = play.begin(); ts != play.end(); ts++) {
        GridRef square = ts->getSquare();
        
        int row = square.getRow();
        if (doneRows.find(row) == doneRows.end()) {
            result += scoreRow(square);
            doneRows.insert(row);
        }
        int column = square.getColumn();
        if (doneColumns.find(column) == doneColumns.end()) {
            result += scoreColumn(square);
            doneColumns.insert(column);
        }
    }

    return result;
}
