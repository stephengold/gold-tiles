#ifndef GAMEOPT_HPP_INCLUDED
#define GAMEOPT_HPP_INCLUDED

// File:     gameopt.hpp
// Location: src
// Purpose:  declare GameOpt class
// Author:   Stephen Gold sgold@sonic.net
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
#include "cell.hpp"     // HASA GridType
#include "handopt.hpp"  // HASA GameStyleType
#include "string.hpp"   // HASA String
#include "tile.hpp"     // HASA AttrCntType

typedef uint16_t MinutesType;  // up to 45 days
typedef int16_t PercentType;
 
enum RulesType {
    RULES_STANDARD,
    RULES_CUSTOM,
    RULES_REPLAY
};

class GameOpt {
public:
    // public constants
    static const SizeType    HAND_SIZE_MIN = 1;
    static const SizeType    HANDS_DEALT_MIN = 1;
    static const MinutesType MINUTES_PER_HAND_MIN = 2;

    // public lifecycle
    GameOpt(void);
    // GameOpt(GameOpt const&);  implicitly defined copy constructor
    explicit GameOpt(String const&);
    GameOpt(GameStyleType, SizeType clones, SizeType handSize, MinutesType minutes);
    // ~GameOpt(void);  implicitly defined destructor

    // public operators
    // GameOpt& operator=(GameOpt const&);  implicitly defined assignment operator
    operator GameStyleType(void) const;
    operator GridType(void) const;
    operator RulesType(void) const;
    operator String(void) const;

    // misc public methods
    AttrCntType  AttrCnt(void) const;
    void         AttrCntChange(void);
    String       AttrReport(void) const;
    RowType      BoardHeight(void) const;
    ColumnType   BoardWidth(void) const;
    PercentType  BonusPercent(void) const;
    ComboCntType CloneCnt(void) const;
    SizeType     ClonesPerCombo(void) const;
    ComboCntType ComboCnt(void) const;
    AttrType     CountAttrValues(AttrIndexType) const;
    String       Description(void) const;
    bool         GetFromClient(Socket&);
    void         GetUserChoice(void);
    SizeType     HandsDealt(void) const;
    SizeType     HandSize(void) const;
    AttrType     MaxAttrValue(AttrIndexType) const;
    MinutesType  MinutesPerHand(void) const;
    void         ReseedGenerator(void) const;
    SecondsType  SecondsPerHand(void) const;
    SeedType     Seed(void) const;
    void         SetAttrCnt(AttrCntType);
    void         SetBoardHeight(RowType);
    void         SetBoardWidth(ColumnType);
    void         SetBoardWrap(bool);
    void         SetBonusPercent(PercentType);
    void         SetChallenge(void);
    void         SetClonesPerCombo(SizeType);
    void         SetDebug(void);
    void         SetDoesBoardWrap(bool);
    void         SetFriendly(void);
    void         SetGrid(GridType);
    void         SetHandsDealt(SizeType);
    void         SetHandSize(SizeType);
    void         SetMaxAttrValue(AttrIndexType, AttrType);
    void         SetMinutesPerHand(MinutesType);
    void         SetNumAttrValues(AttrIndexType, AttrType);
    void         SetPractice(void);
    void         SetRandomizeFlag(bool);
    void         SetRules(RulesType);
    void         SetSeed(SeedType);
    void         Standardize(void);
    void         StyleChange(void);
    SizeType     TilesPerCombo(void) const;
    ComboCntType TotalTileCnt(void) const;
    void         Validate(void) const;

    // public inquiry methods
    bool DoesBoardWrap(void) const;
    bool HasFiniteHeight(void) const;
    bool HasFiniteWidth(void) const;
    bool HasTimeLimit(void) const;
    bool IsChallenge(void) const;
    bool IsDebug(void) const;
    bool IsFriendly(void) const;
    bool IsPractice(void) const;
    bool IsRandomized(void) const;

private:
    // private constants
    static const PercentType BONUS_PERCENT_DEFAULT = 0;
    static const SizeType    CLONES_PER_COMBO_DEFAULT = 2;
    static const SizeType    HAND_CNT_DEFAULT = 2;
    static const SizeType    HAND_SIZE_DEFAULT = 6;
    static const MinutesType MINUTES_PER_HAND_DEFAULT = 30;
    static const SeedType    SEED_DEFAULT = 123;

    // private data
    AttrCntType   mAttrCnt;
    RowType       mBoardHeight;
    ColumnType    mBoardWidth;
    PercentType   mBonusPercent;    // bonus tile percentage
    SizeType      mClonesPerCombo;
    bool          mDoesBoardWrap;   // used only if board is finite on one or both dimensions
    GridType      mGrid;
    SizeType      mHandsDealt;      // number of hands dealt
    SizeType      mHandSize;        // maximum number of tiles in a hand
    std::vector<AttrType>
                  mMaxAttrValues;   // maximum value of each attribute
    MinutesType   mMinutesPerHand;  // used only with GAME_STYLE_CHALLENGE
    bool          mRandomizeFlag;
    RulesType     mRules;
    SeedType      mSeed;            // used only if mRandomizeFlag == false
    GameStyleType mStyle;

    // misc private methods
    Strings CountValues(void) const;
    String  DescribeAttrs(void) const;
    String  DescribeBoard(void) const;
    String  TileShape(void) const;

    // private inquiry methods
    bool AreAllAttrsSimilar(void) const;
};
#endif  // !defined(GAMEOPT_HPP_INCLUDED)
