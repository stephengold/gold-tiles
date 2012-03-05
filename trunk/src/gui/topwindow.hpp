#ifndef TOPWINDOW_HPP_INCLUDED
#define TOPWINDOW_HPP_INCLUDED

// File:    topwindow.hpp
// Purpose: TopWindow class
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
A TopWindow object represents the main window for the game.

In the native version, the TopWindow class extends the Window class.
In the Qt version, the TopWindow class extends the QMainWindow class.
*/

#include "project.hpp"

#ifdef _QT
#include <QMainWindow>

namespace Ui {
    class TopWindow;
}

class TopWindow: public QMainWindow {
    Q_OBJECT

public:
    // public lifecycle
    explicit TopWindow(QWidget *parent = 0);
    ~TopWindow(void);

private:
    // private data
    Ui::TopWindow *ui;
};
#endif


#ifdef _WINDOWS
#include "gui/color.hpp"
#include "gui/window.hpp"   // ISA Window
#include "gui/gameview.hpp" // HASA GameView

class TopWindow: public Window {
public:
	// public lifecycle
	TopWindow(Win::HINSTANCE, Game *pGame);
	// no default constructor
    ~TopWindow(void);

	// misc public methods
	long         DragTileDeltaX(void) const;
	long         DragTileDeltaY(void) const;
	Win::LRESULT HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);
	void         Think(void);

	// public inquiry methods
	bool IsDraggingBoard(void) const;

private:
	// private constants
	static const unsigned HAND_CNT_DEFAULT = 2;
    static const unsigned ID_CLOCK_TIMER = 1;
	static const unsigned TIMEOUT_MSEC = 500;

	// private data
	static WindowClass * 
		     mspClass;
	bool       mDragBoardFlag;
	PCntType   mDragBoardPixelCnt;
	long       mDragTileDeltaX, mDragTileDeltaY;
	Game *    mpGame;
	GameView   mGameView;  // view of the move in progress
	bool       mInitialNewGame;
	bool       mIsStartCentered;
	MenuBar * mpMenuBar;
	Point      mMouseLast; // coordinates of last mouse down
	unsigned   mMouseUpCnt;
	bool       mThinking;
	void *     mThinkFiber;

	// private lifecycle
    TopWindow(TopWindow const &); // not copyable
    void Initialize(Win::CREATESTRUCT const &);

	// private operator
    TopWindow &operator=(TopWindow const &); // not assignable

	// misc private methods
    char const * ClassName(void) const;
	int          CreateNewGame(void);
	void         DiscardGame(void);
    void         HandleButtonDown(Point const &);
	void         HandleButtonUp(Point const &);
    void         HandleMenuCommand(IdType);
	void         HandleMouseMove(Point const &);
	void         InfoBox(char const *message);
	void         LoadPlayerOptions(Hand const &);
	char const * Name(void) const;
	void		 OfferNewGame(void);
	void		 OfferSaveGame(void);
    void         Play(bool passFlag);
	void         ReleaseActiveTile(Point const &);
	void         Repaint(void);
	void         ResignHand(void);
    void         Resize(PCntType width, PCntType height);
	void         RuleBox(char const *message);
	void         SavePlayerOptions(Hand const &) const;
	void         SetGame(Game *pGame);
	void         SetTileWidth(IdType command);
	void         StopDragging(void);
	void         TogglePause(void);
	void         UpdateMenuBar(void);
	int          WarnBox(char const *message);

	// private inquiry methods
	bool AreUnsavedChanges(void) const;
	bool HasGame(void) const;
	bool IsGameOver(void) const;
	bool IsGamePaused(void) const;
};
#endif

#endif // !defined(TOPWINDOW_HPP_INCLUDED)
