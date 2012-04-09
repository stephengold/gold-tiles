#ifndef GAMEWINDOW_HPP_INCLUDED
#define GAMEWINDOW_HPP_INCLUDED

// File:    gamewindow.hpp
// Purpose: GameWindow class
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
A GameWindow object represents the main window for the game.

In the native version, the GameWindow class extends the Window class.
In the Qt version, the GameWindow class extends the QMainWindow class.
*/

#include "gui/gameview.hpp" // HASA GameView
#ifdef _QT
# include <QMainWindow>
# include "gui/rect.hpp"
#elif defined(_WINDOWS)
# include "gui/color.hpp"
# include "gui/window.hpp"   // ISA Window
#endif // defined(_WINDOWS)

enum ThinkModeType {
	THINK_IDLE,
	THINK_SUGGEST,
	THINK_AUTOPLAY
};

#ifdef _QT
namespace Ui {
    class GameWindow;
}
class GameWindow: public QMainWindow {
    Q_OBJECT
#elif defined(_WINDOWS)
class GameWindow: public Window {
#endif // defined(_QT)

public:
    // public lifecycle
#ifdef _QT
    GameWindow(Game *pGame = NULL);
#elif defined(_WINDOWS)
    GameWindow(Win::HINSTANCE, Game *pGame = NULL);
#endif // defined(_QT)
    // no default constructor
    ~GameWindow(void);

    // misc public methods
    long         DragTileDeltaX(void) const;
    long         DragTileDeltaY(void) const;
#ifdef _WINDOWS
    Win::LRESULT HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);
#endif // defined(_WINDOWS)
    void         Think(void);

    // public inquiry methods
    bool IsDraggingBoard(void) const;

private:
    // private constants
    static const unsigned         ID_CLOCK_TIMER = 1;
    static const MsecIntervalType PAUSE_MSEC = 800;
    static const MsecIntervalType TIMEOUT_MSEC = 500;

	// private data
	static WindowClass * 
		        mspClass;
	bool          mDragBoardFlag;
	PixelCntType  mDragBoardPixelCnt;
	long          mDragTileDeltaX;
	long          mDragTileDeltaY;
	Game *       mpGame;
	GameView      mGameView;  // view of the move in progress
	bool          mInitialNewGame;
	bool          mIsStartCentered;
	MenuBar *    mpMenuBar;
	Point         mMouseLast; // coordinates of last mouse update
	unsigned      mMouseUpCnt;
	void *        mThinkFiber;
	ThinkModeType mThinkMode;
#ifdef _QT
    Ui::GameWindow * mpUi;
#endif // defined(_QT)

	// private lifecycle
    GameWindow(GameWindow const &); // not copyable
    void Initialize(Win::CREATESTRUCT const &);

	// private operator
    GameWindow &operator=(GameWindow const &); // not assignable

	// misc private methods
	void         ChangeHand(String const &old_player);
    char const * ClassName(void) const;
	int          CreateNewGame(void);
	void         DiscardGame(void);
	void         GameOver(void);
	int          GameWarnBox(char const *message);
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
	void         RedoTurn(void);
	void         ReleaseActiveTile(Point const &);
	void         Repaint(void);
	void         ResignHand(void);
    void         Resize(PixelCntType width, PixelCntType height);
	void         RestartGame(void);
	void         RuleBox(char const *message);
	String       SaveHandOptions(void) const;
	void         SavePlayerOptions(Hand const &) const;
	void         SetGame(Game *pGame);
	void         SetTileSize(IdType);
	void         StopDragging(void);
	void         UndoTurn(void);
	void         UpdateMenuBar(void);

	// private inquiry methods
	bool AreUnsavedChanges(void) const;
	bool HasGame(void) const;
	bool IsGameOver(void) const;
	bool IsGamePaused(void) const;
};

#endif // !defined(GAMEWINDOW_HPP_INCLUDED)
