// File:    board.cpp
// Purpose: Board class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "board.hpp"
#include "tiles.hpp"

class Rows : public set<int> {};
class Columns : public set<int> {};

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
        if (!isEmpty(*s)) {
            result = false;
            break;
        }
    }
    
    return result;
}

bool Board::areAllRowsCompatible(Locus const &squares) const {
    bool result = true;
    
    if (squares.size() > 1) {
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
    }
    
    return result; 
}

bool Board::areAllColumnsCompatible(Locus const &squares) const {
    bool result = true;
    
    if (squares.size() > 1) {
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
    }
    
    return result; 
}

Tiles Board::getAll(Locus const &squares) const {
    Tiles result;
    
    Locus::const_iterator s;
    for (s = squares.begin(); s != squares.end(); s++) {    
        Tile *ptr = get(*s);
        if (ptr != NULL) {    
            result.insert(*ptr);
        }
    }

    return result;
}

bool Board::isRowCompatible(int row, int firstColumn, int lastColumn) const {
    ASSERT(firstColumn <= lastColumn);
    bool result = true;   
    
    for (int c1 = firstColumn; c1 <= lastColumn; c1++) {
        Tile *t1 = get(row, c1);
        ASSERT(t1 != NULL);
        for (int c2 = c1 + 1; c2 <= lastColumn; c2++) {
            Tile *t2 = get(row, c2);
            if (!t1->isCompatibleWith(t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

bool Board::isColumnCompatible(int firstRow, int lastRow, int column) const {
    ASSERT(firstRow <= lastRow);
    bool result = true;
    
    for (int r1 = firstRow; r1 <= lastRow; r1++) {
        Tile *t1 = get(r1, column);
        ASSERT(t1 != NULL);
        for (int r2 = r1 + 1; r2 <= lastRow; r2++) {
            Tile *t2 = get(r2, column);
            if (!t1->isCompatibleWith(t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

bool Board::isRowCompatible(GridRef const &square) const {
    int row = square.getRow();
     
    int startColumn = square.getColumn();
    while (!isEmpty(row, startColumn - 1)) {
        startColumn--;
    } 
    int endColumn = square.getColumn();
    while (!isEmpty(row, endColumn + 1)) {
        endColumn++;
    }
    
    bool result = isRowCompatible(row, startColumn, endColumn);
    
    return result; 
}

bool Board::isColumnCompatible(GridRef const &square) const {
    int column = square.getColumn();
     
    int startRow = square.getRow();
    while (!isEmpty(startRow - 1, column)) {
        startRow--;
    } 
    int endRow = square.getRow();
    while (!isEmpty(startRow + 1, column)) {
        endRow++;
    }
    
    bool result = isColumnCompatible(startRow, endRow, column);
    
    return result;
}

bool Board::isContiguousRow(Locus const &squares) const {
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
            if (isEmpty(row, iCol)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool Board::isContiguousColumn(Locus const &squares) const {
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
            if (isEmpty(iRow, column)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

void Board::placeTile(TileSquare const &ts) {
    GridRef square = ts.getSquare();
    Tile tile = ts.getTile();

    cout << "Place " << tile.toString() << " at " << square.toString() << endl;
    ASSERT(isEmpty(square));
    validate("before set");
    set(square, tile);
    validate("after set");
}

void Board::placeTiles(Play const &play) {
    Play::const_iterator ts;

    for (ts = play.begin(); ts != play.end(); ts++) {
        placeTile(*ts);
    }
}
