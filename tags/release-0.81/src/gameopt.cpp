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

#include <iostream>
#include "gameopt.hpp"


// lifecycle

GameOpt::GameOpt(void) {
	mMinutesPerHand = MINUTES_PER_HAND_DEFAULT;
	mStyle = GAME_STYLE_DEFAULT;
	Standardize();
	Validate();
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.

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


// misc methods

AttrCntType GameOpt::AttrCnt(void) const {
	ASSERT(mAttrCnt >= Combo::ATTRIBUTE_CNT_MIN);
#ifdef _GUI
	ASSERT(mAttrCnt <= Combo::ATTRIBUTE_CNT_MAX);
#endif // defined(_GUI)

	return mAttrCnt;
}

String GameOpt::AttrReport(void) const {
	String result = ::plural(AttrCnt(), "attribute") + ":\n";
	for (AttrIndexType i_attr = 0; i_attr < AttrCnt(); i_attr++) {
		AttrType const value_max = MaxAttrValue(i_attr);
		result += " " + ::ordinal(i_attr + 1) + " attribute ranges from ";

		AttrModeType const display_mode = Combo::DefaultDisplayMode(i_attr);
		result += Combo::AttributeToString(display_mode, 0) + " to ";
		result += Combo::AttributeToString(display_mode, value_max) + "\n";
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
	// count the possible combinations
	long result = 1L;
	for (AttrIndexType i_attr = 0; i_attr < mAttrCnt; i_attr++) {
	    AttrType const possible_values = CountAttrValues(i_attr);
		result *= possible_values;
	}
	ASSERT(result >= Combo::COMBINATION_CNT_MIN);
#ifdef _GUI
    ASSERT(result <= Combo::COMBINATION_CNT_MAX);
#endif // defined(_GUI)

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

void GameOpt::GetUserChoice(void) {
	String const attr_report = AttrReport();
	std::cout << attr_report;
	
	int hands_dealt = 0;
	while (hands_dealt < int(HANDS_DEALT_MIN)) {
	    std::cout << "Deal how many hands? ";
		String line;
	    std::cin >> line;
		hands_dealt = int(line);
	}
	SetHandsDealt(hands_dealt);
	std::cout << std::endl;

	int hand_size = 0;
	while (hand_size < int(HAND_SIZE_MIN)) {
	    std::cout << "How many tiles per hand? ";
		String line;
	    std::cin >> line;
		hand_size = int(line);
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

unsigned GameOpt::SecondsPerHand(void) const {
	unsigned const result = MinutesPerHand() * SECONDS_PER_MINUTE;

	return result;
}

void GameOpt::SetAttrCnt(AttrCntType attrCnt) {
	ASSERT(attrCnt >= Combo::ATTRIBUTE_CNT_MIN);
#ifdef _GUI
	ASSERT(attrCnt <= Combo::ATTRIBUTE_CNT_MAX);
#endif // defined(_GUI)

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

void GameOpt::SetRules(RulesType rules) {
	mRules = rules;
}

void GameOpt::Standardize(void) {
    mAttrCnt = Combo::ATTRIBUTE_CNT_DEFAULT;
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
	mRules = RULES_STANDARD;
}

void GameOpt::StyleChange(void) {
	switch (mStyle) {
		case GAME_STYLE_PRACTICE:
		    mHandsDealt = 1;
			break;

		case GAME_STYLE_CHALLENGE:
	    case GAME_STYLE_DEBUG:
		case GAME_STYLE_FRIENDLY:
			break;

		default:
			FAIL();
	}
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
	ASSERT(mAttrCnt >= Combo::ATTRIBUTE_CNT_MIN);
#ifdef _GUI
	ASSERT(mAttrCnt <= Combo::ATTRIBUTE_CNT_MAX);
#endif // defined(_GUI)
	ASSERT(::is_even(mBoardHeight));
	ASSERT(mBoardHeight >= Cell::HEIGHT_MIN);
	ASSERT(mBoardHeight <= Cell::HEIGHT_MAX);
	ASSERT(::is_even(mBoardWidth));
	ASSERT(mBoardWidth >= Cell::WIDTH_MIN);
	ASSERT(mBoardWidth <= Cell::WIDTH_MAX);
	ASSERT(mBonusPercent < 100);
	ASSERT(mHandsDealt >= HANDS_DEALT_MIN);
	ASSERT(mHandSize >= HAND_SIZE_MIN);
	ASSERT(mMinutesPerHand >= MINUTES_PER_HAND_MIN);
	ASSERT(mStyle != GAME_STYLE_NONE);
}


// inquiry methods

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
