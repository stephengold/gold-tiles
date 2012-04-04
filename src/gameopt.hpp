#ifndef GAMEOPT_HPP_INCLUDED
#define GAMEOPT_HPP_INCLUDED

// File:    gameopt.hpp
// Purpose: GameOpt class header file
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
A GameOpt object represents the configuration options of a Game.

The GameOpt class ...
*/

#include <vector>
#include "cell.hpp"    // HASA GridType
#include "handopt.hpp" // HASA GameStyleType
#include "string.hpp"  // HASA String
#include "tile.hpp"    // HASA AttrCntType

class GameOpt {
public:
	// public constants
	static const unsigned BONUS_PERCENT_DEFAULT = 10;
    static const unsigned CLONES_PER_COMBO_DEFAULT = 0;
    static const unsigned HAND_CNT_DEFAULT = 2;
    static const unsigned HAND_SIZE_MIN = 1;
    static const unsigned HAND_SIZE_DEFAULT = 6;
	static const unsigned HANDS_DEALT_MIN = 1;
	static const unsigned MINUTES_PER_HAND_MIN = 2;
	static const unsigned MINUTES_PER_HAND_DEFAULT = 30;

	// public lifecycle
	GameOpt(void);
	GameOpt(GameStyleType, unsigned clones, unsigned handSize, unsigned minutes);
    // GameOpt(GameOpt const &);  compiler-generated copy constructor is OK
    // ~GameOpt(void);  compiler-generated destructor is OK

	// public operators
    // GameOpt &operator=(GameOpt const &);  compiler-generated assignment operator is OK
	operator GameStyleType(void) const;
	operator GridType(void) const;

	// misc public methods
	AttrCntType AttrCnt(void) const;
	void        AttrCntChange(void);
	String      AttrReport(void) const;
	IndexType   BoardHeight(void) const;
	IndexType   BoardWidth(void) const;
	unsigned    BonusPercent(void) const;
	long        CloneCnt(void) const;
	unsigned    ClonesPerCombo(void) const;
	long        ComboCnt(void) const;
	AttrType    CountAttrValues(AttrIndexType) const;
	void        GetUserChoice(void);
	unsigned    HandsDealt(void) const;
	unsigned    HandSize(void) const;
	AttrType    MaxAttrValue(AttrIndexType) const;
	unsigned    MinutesPerHand(void) const;
	unsigned    SecondsPerHand(void) const;
	void        SetAttrCnt(AttrCntType);
	void        SetBoardHeight(IndexType);
	void        SetBoardWidth(IndexType);
	void        SetBoardWrap(bool);
	void        SetBonusPercent(unsigned);
	void        SetChallenge(void);
	void        SetClonesPerCombo(unsigned);
	void        SetDebug(void);
	void        SetDoesBoardWrap(bool);
	void        SetFriendly(void);
	void        SetGrid(GridType);
	void        SetHandsDealt(unsigned);
	void        SetHandSize(unsigned);
	void        SetMaxAttrValue(AttrIndexType, AttrType);
	void        SetMinutesPerHand(unsigned);
	void        SetNumAttrValues(AttrIndexType, AttrType);
	void        SetPractice(void);
	void        StyleChange(void);
	unsigned    TilesPerCombo(void) const;
	long        TotalTileCnt(void) const;

	// public inquiry methods
	bool DoesBoardWrap(void) const;
	bool HasFiniteHeight(void) const;
	bool HasFiniteWidth(void) const;
	bool HasTimeLimit(void) const;
	bool IsChallenge(void) const;
	bool IsDebug(void) const;
	bool IsFriendly(void) const;
	bool IsPractice(void) const;

private:
	// private data
    AttrCntType   mAttrCnt;
    IndexType     mBoardHeight;
	IndexType     mBoardWidth;
	unsigned      mBonusPercent;    // bonus tile percentage
	unsigned      mClonesPerCombo;
	bool          mDoesBoardWrap;
	GridType      mGrid;
	unsigned      mHandsDealt;      // number of hands dealt
	unsigned      mHandSize;        // maximum number of tiles in a hand
	std::vector<AttrType>
		          mMaxAttrValues;   // maximum value of each attribute
	unsigned      mMinutesPerHand;  // used only with GAME_STYLE_CHALLENGE
	GameStyleType mStyle;
};
#endif // !defined(GAMEOPT_HPP_INCLUDED)
