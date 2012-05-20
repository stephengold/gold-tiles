// File:     gameopt.cpp
// Location: src
// Purpose:  implement GameOpt class
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

#include <iostream>    // std::cin
#include "gameopt.hpp"
#include "socket.hpp"
#include "strings.hpp"


// lifecycle

GameOpt::GameOpt(void) {
    mMinutesPerHand = MINUTES_PER_HAND_DEFAULT;
    mRandomizeFlag = true;
    mSeed = SEED_DEFAULT;
    mStyle = GAME_STYLE_DEFAULT;
    Standardize();
    Validate();
}

GameOpt::GameOpt(String const& rString) {
    Strings const lines(rString, "\n");
    Strings::ConstIterator i_line;
    for (i_line = lines.Begin(); i_line != lines.End(); i_line++) {
        String const line = *i_line;
        Strings const fields(line, "=");
        ASSERT(fields.Count() == 2); // TODO recovery
        String const name = fields.First();
        String const value = fields.Second();
        if (name == "AttrCnt") {
            mAttrCnt = ::string_to_attr_cnt(value);
            mMaxAttrValues.resize(mAttrCnt);
        } else if (name == "BoardHeight") {
            mBoardHeight = ::string_to_index(value);
        } else if (name == "BoardWidth") {
            mBoardWidth = ::string_to_index(value);
        } else if (name == "BonusPercent") {
            mBonusPercent = long(value);
        } else if (name == "ClonesPerCombo") {
            mClonesPerCombo = long(value);
        } else if (name == "DoesBoardWrap") {
            mDoesBoardWrap = bool(value);
        } else if (name == "Grid") {
            mGrid = ::string_to_grid(value);
        } else if (name == "HandsDealt") {
            mHandsDealt = long(value);
        } else if (name == "HandSize") {
            mHandSize = long(value);
        } else if (name.HasPrefix("MaxAttrValue")) {
            String const suffix = name.Suffix("MaxAttrValue");
            unsigned const index = long(suffix);
            mMaxAttrValues[index] = ::string_to_max_attr(value);
        } else if (name == "MinutesPerHand") {
            mMinutesPerHand = long(value);
        } else if (name == "RandomizeFlag") {
            mRandomizeFlag = bool(value);
        } else if (name == "Seed") {
            mSeed = long(value);
        } else if (name == "Style") {
            mStyle = ::string_to_game_style(value);
        } else {
            FAIL();  // TODO recovery
        }
    }
}

// The implicitly defined copy constructor is OK.
// The implicitly defined destructor is OK.


// operators

// The implicitly defined assignment method is OK.

GameOpt::operator GameStyleType(void) const {
    ASSERT(mStyle != GAME_STYLE_NONE);

    return mStyle;
}

GameOpt::operator GridType(void) const {
    return mGrid;
}

GameOpt::operator RulesType(void) const {
    return mRules;
}

GameOpt::operator String(void) const {
    String result;

    result += "AttrCnt=" + String(mAttrCnt) + "\n";
    result += "BoardHeight=" + ::index_to_string(mBoardHeight) + "\n";
    result += "BoardWidth=" + ::index_to_string(mBoardWidth) + "\n";
    result += "BonusPercent=" + String(mBonusPercent) + "\n";
    result += "ClonesPerCombo=" + String(mClonesPerCombo) + "\n";
    result += "DoesBoardWrap=" + String(mDoesBoardWrap) + "\n";
    result += "Grid=" + ::grid_to_string(mGrid) + "\n";
    result += "HandsDealt=" + String(mHandsDealt) + "\n";
    result += "HandSize=" + String(mHandSize) + "\n";
    for (AttrIndexType i_attr = 0; i_attr < mAttrCnt; i_attr++) {
        result += "MaxAttrValue" + String(i_attr) + "=" + String(mMaxAttrValues[i_attr]) + "\n";
    }
    result += "MinutesPerHand=" + String(mMinutesPerHand) + "\n";
    result += "RandomizeFlag=" + String(mRandomizeFlag) + "\n";
    if (!mRandomizeFlag) {
        result += "Seed=" + String(mSeed) + "\n";
    }
    result += "Style=" + ::game_style_to_string(mStyle) + "\n";

    result += "\n";

    return result;
}

// misc methods

AttrCntType GameOpt::AttrCnt(void) const {
    ASSERT(mAttrCnt >= Combo::ATTR_CNT_MIN);
    ASSERT(mAttrCnt <= Combo::ATTR_CNT_MAX);

    return mAttrCnt;
}

String GameOpt::AttrReport(void) const {
    String result = "Each tile has " + ::plural(AttrCnt(), "attribute") + ":\n";
    for (AttrIndexType i_attr = 0; i_attr < AttrCnt(); i_attr++) {
        result += " The " + ::ordinal(i_attr + 1) + " attribute ranges from ";

        AttrModeType const display_mode = Combo::DefaultDisplayMode(i_attr);
        result += Combo::AttrToString(display_mode, 0) + " to ";
        AttrType const value_max = MaxAttrValue(i_attr);
        result += Combo::AttrToString(display_mode, value_max) + ".\n";
    }
    result += "\n";

    return result;
}

RowType GameOpt::BoardHeight(void) const {
    ASSERT(::is_even(mBoardHeight));
    ASSERT(mBoardHeight >= Cell::HEIGHT_MIN);
    ASSERT(mBoardHeight <= Cell::HEIGHT_MAX);

    return mBoardHeight;
}

ColumnType GameOpt::BoardWidth(void) const {
    ASSERT(::is_even(mBoardWidth));
    ASSERT(mBoardWidth >= Cell::WIDTH_MIN);
    ASSERT(mBoardWidth <= Cell::WIDTH_MAX);

    return mBoardWidth;
}

unsigned GameOpt::BonusPercent(void) const {
    ASSERT(mBonusPercent < 100);

    return mBonusPercent;
}

long GameOpt::CloneCnt(void) const {
    long const result = ComboCnt() * ClonesPerCombo();

    return result;
}

unsigned GameOpt::ClonesPerCombo(void) const {
    return mClonesPerCombo;
}

long GameOpt::ComboCnt(void) const {
    // Compute the number of possible combinations.
    long result = 1L;
    for (AttrIndexType i_attr = 0; i_attr < mAttrCnt; i_attr++) {
        AttrType const possible_values = CountAttrValues(i_attr);
        result *= possible_values;
    }
    ASSERT(result >= Combo::COMBINATION_CNT_MIN);
#ifdef _GUI
    ASSERT(result <= Combo::COMBINATION_CNT_MAX);
#endif  // defined(_GUI)

    return result;
}

AttrType GameOpt::CountAttrValues(AttrIndexType ind) const {
    AttrType result = Combo::VALUE_CNT_DEFAULT;

    if (ind < mMaxAttrValues.size()) {
        result = mMaxAttrValues[ind] + 1;  // including zero
    }

    ASSERT(result >= Combo::VALUE_CNT_MIN);
    ASSERT(result <= Combo::VALUE_CNT_MAX);
    return result;
}

Strings GameOpt::CountValues(void) const {
    Strings result;

    for (AttrIndexType ind = 0; ind < mAttrCnt; ind++) {
        long const count = CountAttrValues(ind);
        String const count_string(count);
        result.Append(count_string);
    }

    return result;
}

String GameOpt::DescribeAttrs(void) const {
    String result;

    if (AreAllAttrsSimilar()) {
        AttrType const num_values = CountAttrValues(0);
        result = ::plural(num_values, "value") + " for each attribute";
    } else {
        Strings const counts = CountValues();
        result = "the attributes having " + String(counts, ", ", " and ") 
            + " values respectively";
    }

    return result;
}

String GameOpt::DescribeBoard(void) const {
    String result;

    if (mBoardHeight == Cell::HEIGHT_MAX && mBoardWidth == Cell::WIDTH_MAX) {
        result = "an endless grid";

    } else if (mBoardHeight == Cell::HEIGHT_MAX) {
        result = "a vertical ";
        if (mDoesBoardWrap) {
            result += "cylinder ";
        } else {
            result += "strip ";
        }
        result += ::plural(mBoardWidth, "cell") + " wide";

    } else if (mBoardWidth == Cell::WIDTH_MAX) {
        result = "a horizontal ";
        if (mDoesBoardWrap) {
            result += "cylinder ";
        } else {
            result += "strip ";
        }
        result += ::plural(mBoardHeight, "cell") + " tall";

    } else if (mDoesBoardWrap) {
        result = "a torus "
            + ::plural(mBoardWidth, "cell") + " wide and "
            + ::plural(mBoardHeight, "cell") + " tall";

    } else if (mBoardWidth == mBoardHeight) {
        result = "a square " + ::plural(mBoardWidth, "cell") + " on a side";

    } else {
        result = "a rectangle "
            + ::plural(mBoardWidth, "cell") + " wide and "
            + ::plural(mBoardHeight, "cell") + " tall";
    }

    return result;
}

String GameOpt::Description(void) const {

    // Describe a few details which are separate from the rest.
    Strings details;

    if (!mRandomizeFlag) {
        details.Append("a stacked deck");
    }

    if (HasTimeLimit()) {
        String description = "a " + String(mMinutesPerHand);
        description += "-minute time limit for each hand";
        details.Append(description);
    }

    GameOpt const default_opt;  // default game options for comparison

    // Begin recording commonalities and exceptions.
    Strings commonalities;
    Strings exceptions;

    // tile shape, scoring directions, and number of tiles
    String tiles;
    long const tile_cnt = TotalTileCnt();
    String const shape = TileShape();
    if (tile_cnt == default_opt.TotalTileCnt()) {   // same number of tiles
        if (shape == default_opt.TileShape()) {  // same shape
            String const tile_shape = shape + " tile";
            tiles = ::plural(tile_cnt, tile_shape);
            if (mGrid != default_opt.mGrid) {  // different grid
                String const ways = "an " + ::grid_to_string(mGrid) + "-way grid";
                exceptions.Append(ways);
            }
        } else {   // different shape
            tiles = ::plural(tile_cnt, "tile");
            String const tile_shape = TileShape() + " tiles";
            exceptions.Append(tile_shape);
        }
    } else {    // different number of tiles
        if (shape == default_opt.TileShape()) {  // same shape
            tiles = shape + " tiles";
            String const number = ::plural(tile_cnt, "tile");
            exceptions.Append(number);
            if (mGrid != default_opt.mGrid) {  // different grid
                String const ways = "an " + ::grid_to_string(mGrid) + "-way grid";
                exceptions.Append(ways);
            }
        } else {   // different shape
            String const tile_shape = shape + " tile";
            String const description = ::plural(tile_cnt, tile_shape);
            exceptions.Append(description);
        }
    }

    // the board
    String const board = DescribeBoard();

    // combine tiles and board
    if (board == default_opt.DescribeBoard()) {
        if (tiles.IsEmpty()) {
            commonalities.Append("on " + board);
        } else {
            commonalities.Append(tiles + " on " + board);
        }
    } else {
        exceptions.Append("the board being " + board);
        if (!tiles.IsEmpty()) {
            commonalities.Append(tiles);
        }
    }

    // attributes
    String const attrs = ::plural(mAttrCnt, "attribute") + " per tile";
    if (mAttrCnt != default_opt.mAttrCnt) {
        exceptions.Append(attrs);
    } else {
        commonalities.Append(attrs);
    }
    String const describe_attrs = DescribeAttrs();
    if (describe_attrs != default_opt.DescribeAttrs()) {
        exceptions.Append(describe_attrs);
    } else {
        commonalities.Append(describe_attrs);
    }

    // number of hands and tiles per hand
    String description = ::plural(mHandsDealt, "hand");
    if (mHandsDealt == default_opt.mHandsDealt
        && mHandSize == default_opt.mHandSize)
    {
        description += " of " + ::plural(mHandSize, "tile") + " each";
        commonalities.Append(description);
    } else if (mHandsDealt == default_opt.mHandsDealt) {
        commonalities.Append(description);
        description = ::plural(mHandSize, "tile") + " per hand";
        exceptions.Append(description);
    } else if (mHandSize == default_opt.mHandSize) {
        exceptions.Append(description);
        description = ::plural(mHandSize, "tile") + " per hand";
        commonalities.Append(description);
    } else {
        description += " of " + ::plural(mHandSize, "tile") + " each";
        exceptions.Append(description);
    }

    if (mBonusPercent != default_opt.mBonusPercent) {
        String const description = String(mBonusPercent) + "% bonus tiles";
        exceptions.Append(description);
    }

    // Pull the description together into a single string.
    String result = "a " + ::game_style_to_string(mStyle) + " game";
    if (!details.IsEmpty()) {
        result += " with " + String(details, ", ", " and ") + ",";
    }
    if (!commonalities.IsEmpty()) {
        result += " using the standard rules";
        result += " (" + String(commonalities, ", ", " and ") + ")";
        if (!exceptions.IsEmpty()) {
            result += ", EXCEPT";
        }
    }
    if (!exceptions.IsEmpty()) {
        result += " with " + String(exceptions, ", ", " and ");
    }
    result += ".";

    return result;
}

// Return true if successful, false if canceled.
bool GameOpt::GetFromClient(Socket& rClient) {
    String opt_text;
    bool const was_successful = rClient.GetParagraph(opt_text);
    if (was_successful) {
        *this = GameOpt(opt_text);
        Validate();
    }

    return was_successful;
}

void GameOpt::GetUserChoice(void) {
    String const attr_report = AttrReport();
    std::cout << attr_report;

    long hands_dealt = 0;
    while (hands_dealt < long(HANDS_DEALT_MIN)) {
        std::cout << "Deal how many hands? ";
        String line;
        std::getline(std::cin, line);
        hands_dealt = long(line);
    }
    SetHandsDealt(hands_dealt);
    std::cout << std::endl;

    long hand_size = 0;
    while (hand_size < long(HAND_SIZE_MIN)) {
        std::cout << "How many tiles per hand? ";
        String line;
        std::getline(std::cin, line);
        hand_size = long(line);
    }
    SetHandSize(hand_size);
    std::cout << std::endl;

    // Clone tiles so that there are enough to fill each hand at least three times.
    unsigned const tiles_needed = 3 * hand_size * hands_dealt;
    long const combo_cnt = ComboCnt();
    unsigned clones_per_combo = unsigned(tiles_needed / combo_cnt);
    SetClonesPerCombo(clones_per_combo);
}

unsigned GameOpt::HandsDealt(void) const {
    ASSERT(mHandsDealt >= HANDS_DEALT_MIN);

    return mHandsDealt;
}

unsigned GameOpt::HandSize(void) const {
    ASSERT(mHandSize >= HAND_SIZE_MIN);

    return mHandSize;
}

AttrType GameOpt::MaxAttrValue(AttrIndexType ind) const {
    AttrType const result = CountAttrValues(ind) - 1; // excluding zero

    return result;
}

unsigned GameOpt::MinutesPerHand(void) const {
    ASSERT(mMinutesPerHand >= MINUTES_PER_HAND_MIN);

    return mMinutesPerHand;
}

void GameOpt::ReseedGenerator(void) const {
    // Re-seed the pseudo-random generator at the start of a game.
    unsigned seed = mSeed; 
    if (mRandomizeFlag || !IsDebug()) {
        seed = ::milliseconds();
    }
    Fraction::ReseedGenerator(seed);
}

unsigned GameOpt::SecondsPerHand(void) const {
    unsigned const result = MinutesPerHand() * SECONDS_PER_MINUTE;

    return result;
}

unsigned GameOpt::Seed(void) const {
    return mSeed;
}

void GameOpt::SetAttrCnt(AttrCntType attrCnt) {
    ASSERT(attrCnt >= Combo::ATTR_CNT_MIN);
    ASSERT(attrCnt <= Combo::ATTR_CNT_MAX);

    mAttrCnt = attrCnt;
}

void GameOpt::SetBoardHeight(RowType height) {
    ASSERT(::is_even(height));
    ASSERT(height >= Cell::HEIGHT_MIN);
    ASSERT(height <= Cell::HEIGHT_MAX);

    mBoardHeight = height;
}

void GameOpt::SetBoardWidth(ColumnType width) {
    ASSERT(::is_even(width));
    ASSERT(width >= Cell::WIDTH_MIN);
    ASSERT(width <= Cell::WIDTH_MAX);

    mBoardWidth = width;
}

void GameOpt::SetBonusPercent(unsigned percent) {
    ASSERT(percent < 100);

    mBonusPercent = percent;
}

void GameOpt::SetChallenge(void) {
    mStyle = GAME_STYLE_CHALLENGE;
}

void GameOpt::SetClonesPerCombo(unsigned clonesPerCombo) {
    mClonesPerCombo = clonesPerCombo;
}

void GameOpt::SetDebug(void) {
    mStyle = GAME_STYLE_DEBUG;
}

void GameOpt::SetDoesBoardWrap(bool wrap) {
    mDoesBoardWrap = wrap;
}

void GameOpt::SetFriendly(void) {
    mStyle = GAME_STYLE_FRIENDLY;
}

void GameOpt::SetGrid(GridType grid) {
    mGrid = grid;
}

void GameOpt::SetHandsDealt(unsigned handsDealt) {
    ASSERT(mHandsDealt >= HANDS_DEALT_MIN);

    mHandsDealt = handsDealt;
}

void GameOpt::SetHandSize(unsigned handSize) {
    ASSERT(handSize >= HAND_SIZE_MIN);

    mHandSize = handSize;
}

void GameOpt::SetMinutesPerHand(unsigned minutes) {
    ASSERT(minutes >= MINUTES_PER_HAND_MIN);

    mMinutesPerHand = minutes;
}

void GameOpt::SetNumAttrValues(AttrIndexType ind, AttrType numValues) {
    ASSERT(numValues >= Combo::VALUE_CNT_MIN);
    ASSERT(numValues <= Combo::VALUE_CNT_MAX);

    AttrCntType const original_count = AttrCntType(mMaxAttrValues.size());
    AttrType const max_value = Combo::VALUE_CNT_DEFAULT - 1; // excluding zero
    for (AttrIndexType i_attr = original_count; i_attr <= ind; i_attr++) {
        mMaxAttrValues.push_back(max_value);
    }
    ASSERT(mMaxAttrValues.size() > ind);
    mMaxAttrValues[ind] = numValues - 1;  // excluding zero
}

void GameOpt::SetPractice(void) {
    mStyle = GAME_STYLE_PRACTICE;
}

void GameOpt::SetRandomizeFlag(bool value) {
    mRandomizeFlag = value;
}

void GameOpt::SetRules(RulesType rules) {
    mRules = rules;
}

void GameOpt::SetSeed(unsigned seed) {
    mSeed = seed;
}

void GameOpt::Standardize(void) {
    mAttrCnt = Combo::ATTR_CNT_DEFAULT;
    mMaxAttrValues.clear();
    for (unsigned i_attr = 0; i_attr < mAttrCnt; i_attr++) {
        mMaxAttrValues.push_back(Combo::VALUE_CNT_DEFAULT - 1);
    }

    mBoardHeight = Cell::HEIGHT_MAX;
    mBoardWidth = Cell::WIDTH_MAX;
    mBonusPercent = BONUS_PERCENT_DEFAULT;
    mClonesPerCombo = CLONES_PER_COMBO_DEFAULT;
    mDoesBoardWrap = false;
    mGrid = GRID_DEFAULT;
    mHandsDealt = HAND_CNT_DEFAULT;
    mHandSize = HAND_SIZE_DEFAULT;
    // don't set mMinutesPerHand
    // don't set mRandomizeFlag
    mRules = RULES_STANDARD;
    // don't set mStyle
}

void GameOpt::StyleChange(void) {
    switch (mStyle) {
    case GAME_STYLE_PRACTICE:
        mHandsDealt = 1;
        mRandomizeFlag = true;
        break;

    case GAME_STYLE_CHALLENGE:
    case GAME_STYLE_FRIENDLY:
        mRandomizeFlag = true;
        break;

    case GAME_STYLE_DEBUG:
        break;

    default:
        FAIL();
    }
}

String GameOpt::TileShape(void) const {
    String result;

    switch (mGrid) {
    case GRID_TRIANGLE:
        result = "triangular";
        break;
    case GRID_HEX:
        result = "hexagonal";
        break;
    case GRID_4WAY:
    case GRID_8WAY:
        result = "square";
        break;
    default:
        FAIL();
    }

    return result;
}

unsigned GameOpt::TilesPerCombo(void) const {
    unsigned const result = ClonesPerCombo() + 1;

    return result;
}

long GameOpt::TotalTileCnt(void) const {
    long const result = ComboCnt() * (1 + ClonesPerCombo());

    return result;
}

void GameOpt::Validate(void) const {
    ASSERT(mAttrCnt >= Combo::ATTR_CNT_MIN);
    ASSERT(mAttrCnt <= Combo::ATTR_CNT_MAX);
    ASSERT(::is_even(mBoardHeight));
    ASSERT(mBoardHeight >= Cell::HEIGHT_MIN);
    ASSERT(mBoardHeight <= Cell::HEIGHT_MAX);
    ASSERT(::is_even(mBoardWidth));
    ASSERT(mBoardWidth >= Cell::WIDTH_MIN);
    ASSERT(mBoardWidth <= Cell::WIDTH_MAX);
    ASSERT(mBonusPercent < 100);
#ifdef _CONSOLE
    ASSERT(!mDoesBoardWrap);
    ASSERT(mGrid == GRID_4WAY);
#endif  // defined(_CONSOLE)
    ASSERT(mHandsDealt >= HANDS_DEALT_MIN);
    ASSERT(mHandSize >= HAND_SIZE_MIN);
    ASSERT(mMinutesPerHand >= MINUTES_PER_HAND_MIN);
    ASSERT(IsDebug() || mRandomizeFlag);
    ASSERT(mStyle != GAME_STYLE_NONE);
#ifdef _CONSOLE
    ASSERT(mStyle != GAME_STYLE_CHALLENGE);
#endif  // defined(_CONSOLE)
}


// inquiry methods

bool GameOpt::AreAllAttrsSimilar(void) const {
    bool result = true;

    for (AttrIndexType i_attr = 1; i_attr < AttrCnt(); i_attr++) {
        AttrType const value_max = MaxAttrValue(i_attr);
        if (value_max != MaxAttrValue(0)) {
            result = false;
            break;
        }
    }

    return result;
}

bool GameOpt::DoesBoardWrap(void) const {
    return mDoesBoardWrap;
}

bool GameOpt::HasFiniteHeight(void) const {
    bool const result = (BoardHeight() < Cell::HEIGHT_MAX);

    return result;
}

bool GameOpt::HasFiniteWidth(void) const {
    bool const result = (BoardWidth() < Cell::WIDTH_MAX);

    return result;
}

bool GameOpt::HasTimeLimit(void) const {
    bool const result = IsChallenge();

    return result;
}

bool GameOpt::IsChallenge(void) const {
    bool const result = (mStyle == GAME_STYLE_CHALLENGE);

    return result;
}

bool GameOpt::IsDebug(void) const {
    bool const result = (mStyle == GAME_STYLE_DEBUG);

    return result;
}

bool GameOpt::IsFriendly(void) const {
    bool const result = (mStyle == GAME_STYLE_FRIENDLY);

    return result;
}

bool GameOpt::IsPractice(void) const {
    bool const result = (mStyle == GAME_STYLE_PRACTICE);

    return result;
}

bool GameOpt::IsRandomized(void) const {
    return mRandomizeFlag;
}