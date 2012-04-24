// File:     markings.cpp
// Location: src/gui
// Purpose:  implement Markings class
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

#include <vector>
#include "gui/displaymodes.hpp"
#include "gui/markings.hpp"


// static data

const ColorType Markings::msColors[Combo::VALUE_CNT_MAX] = {
    COLOR_BLACK,      COLOR_RED,       COLOR_DARK_BLUE, 
    COLOR_DARK_GREEN, COLOR_PURPLE,    COLOR_BROWN, 
    COLOR_DARK_GRAY,  COLOR_PINK,      COLOR_LIGHT_BLUE
};


// lifecycle

Markings::Markings(Tile const& rTile, DisplayModes const& rDisplayModes) {
	mMarkingColor = msColors[0];
	AttrIndexType i_marking = 0;

    for (AttrIndexType i_attr = 0; i_attr < Combo::AttributeCnt(); i_attr++) {
		 AttrModeType const display_mode = rDisplayModes.Mode(i_attr);
		 AttrType const attr_value = rTile.Attribute(i_attr);
		 ASSERT(attr_value < Combo::VALUE_CNT_MAX);

		 if (display_mode == ATTR_MODE_COLOR) {
	         mMarkingColor = msColors[attr_value];
		 } else {
			 ASSERT(i_marking < MARKING_CNT_MAX);
			 mModes[i_marking] = display_mode;
             mMarkings[i_marking] = attr_value;
			 i_marking++;
		 }
    }

	mMarkingCnt = i_marking;
	ASSERT(mMarkingCnt > 0);
	ASSERT(mMarkingCnt <= MARKING_CNT_MAX);
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment operator is fine.


// misc methods

AttrType Markings::Marking(AttrIndexType ind) const {
	ASSERT(ind < mMarkingCnt);

	AttrType const result = mMarkings[ind];

	ASSERT(result < Combo::VALUE_CNT_MAX);
	return result;
}

AttrCntType Markings::MarkingCnt(void) const {
    return mMarkingCnt;
}

ColorType Markings::MarkingColor(void) const {
	return mMarkingColor;
}

AttrModeType Markings::Mode(AttrIndexType index) const {
	ASSERT(index < mMarkingCnt);

	AttrModeType const result = mModes[index];

	return result;
}
