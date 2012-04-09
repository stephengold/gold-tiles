// File:    gamewindow.cpp
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

#include "gui/canvas.hpp"
#include "gui/menubar.hpp"
#include "gui/gamewindow.hpp"
#include "strings.hpp"

#ifdef _QT
#include "ui_gamewindow.h"

GameWindow::GameWindow(Game *pGame):
    QMainWindow(NULL),  // the top window has no parent
    mMouseLast(0, 0),
    mGameView(*pGame)
{
    mpUi = new Ui::GameWindow;
    mpUi->setupUi(this);

    mpGame = pGame;
    mpMenuBar = new MenuBar(mGameView);
    setMenuBar(mpMenuBar);
}
#endif

#ifdef  _WINDOWS
#include "gui/attrbox.hpp"
#include "gui/handbox.hpp"
#include "gui/hintbox.hpp"
#include "gui/parmbox1.hpp"
#include "gui/parmbox2.hpp"
#include "gui/parmbox3.hpp"
#include "gui/player.hpp"
#include "gui/resource.hpp"
#include "gui/tilebox.hpp"
#include "gui/win_types.hpp"
#include "gui/windowclass.hpp"
#include "handopts.hpp"


// static data of the class

WindowClass *GameWindow::mspClass = NULL;


// static callback functions

// message handler (callback) for game window
static LRESULT CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParam,
	LPARAM lParam)
{
	ASSERT(windowHandle != NULL);
    GameWindow * const window = (GameWindow *)Window::Lookup(windowHandle);

	LRESULT result;
	if (window == NULL) { // unknown window
		// invoke default message handler
		result = Win::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	ASSERT(HWND(*window) == windowHandle);
        result = window->HandleMessage(message, wParam, lParam);
	}

	return result;
}

// callback for Think fiber
static void CALLBACK think(void *pArgument) {
	GameWindow * const window = (GameWindow *)pArgument;

    window->Think();
}

static void yield(void *pArgument) {
	GameWindow * const window = (GameWindow *)pArgument;

	window->Yields();
}


// lifecycle

GameWindow::GameWindow(HINSTANCE applicationInstance, Game *pGame):
    mMouseLast(0, 0),
	mGameView(*pGame)
{
	ASSERT(HWND(*this) == 0);
	ASSERT(applicationInstance != NULL);

	char const *class_name = "GAMEWINDOW";
	char const *menubar_name = "MENUBAR";
    if (mspClass == NULL) {
		// for first instance:  create a Microsoft Windows window class
		mspClass = new WindowClass(applicationInstance, &message_handler, class_name, menubar_name);
	    ASSERT(mspClass != NULL);
		mspClass->RegisterClass();
	}

    mDragBoardFlag = false;
	mpGame = pGame;
	mInitialNewGame = (pGame == NULL);
    mpMenuBar = NULL;
	
	// set up Think fiber for background processing
	UseFibers();
	mThinkMode = THINK_IDLE;
	mThinkFiber = AddFiber(think);

	// set up keyboard shortcuts
	SetAcceleratorTable("HOTKEYS");

	// determine initial window size:  to cover 64% of desktop
	Rect const desktop_bounds = DesktopBounds();
	PixelCntType const height = PixelCntType(0.8*double(desktop_bounds.Height()));
	PixelCntType const width = PixelCntType(0.8*double(desktop_bounds.Width()));
	LogicalXType const x = width/8;
	LogicalYType const y = height/8;
	Rect const rect(y, x, width, height);

	// create Microsoft Windows window
	String const class_string(class_name);
	Window * const p_parent = NULL;
	Create(class_string, rect, p_parent, applicationInstance);

	// wait for message_handler() to receive a message with this handle
}

GameWindow::~GameWindow(void) {
    delete mpMenuBar;
}

void GameWindow::Initialize(CREATESTRUCT const &rCreateStruct) {
    // Initialization which takes place after the Microsoft Windows window
    // has received its WM_CREATE message.

	ASSERT(mpMenuBar == NULL);

	Window::Initialize(rCreateStruct);

	mpMenuBar = new MenuBar(rCreateStruct, mGameView);
	ASSERT(mpMenuBar != NULL);

	mGameView.SetWindow(this, mpMenuBar);
   	SetCursorSelect();

    Partial::SetYield(&yield, (void *)this);

    SetTileSize(GameView::TILE_SIZE_DEFAULT);
	if (HasGame()) {
	    Hands const hands = Hands(*mpGame);
		Hands::ConstIterator i_hand;
		for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
			if (i_hand->IsLocalUser()) {
			    SavePlayerOptions(*i_hand);
			}
		}
		ChangeHand("");
	}

	SetTimer(TIMEOUT_MSEC, ID_CLOCK_TIMER);
	UpdateMenuBar();
}


// misc methods

void GameWindow::ChangeHand(String const &rOldPlayerName) {
	ASSERT(HasGame());
	ASSERT(IsGamePaused() || IsGameOver());
	ASSERT(mpMenuBar != NULL);
	ASSERT(mThinkMode == THINK_IDLE);

	Hand const playable_hand = Hand(*mpGame);

	if (playable_hand.IsLocalUser()) {
        LoadPlayerOptions(playable_hand);
	}
	double const skip_probability = playable_hand.SkipProbability();
    mGameView.Reset(skip_probability);
	mGameView.ResetTargetCell();

	if (!IsGameOver()) {
	    if (!playable_hand.IsLocalUser()) {
			mpGame->StartClock();
		} else if (rOldPlayerName == playable_hand.PlayerName())	{
		    mpGame->StartClock();
		} else if (!mpMenuBar->IsAutopause()) {
			mpGame->StartClock();
		}

		if (playable_hand.IsAutomatic() && !mpGame->CanRedo()) {
			// launch autoplay
		    mThinkMode = THINK_AUTOPLAY;
		}
	}
}

long GameWindow::DragTileDeltaX(void) const {
	return mDragTileDeltaX;
}

long GameWindow::DragTileDeltaY(void) const {
	return mDragTileDeltaY;
}

void GameWindow::GameOver(void) {
	ASSERT(HasGame());
	ASSERT(mpGame->IsOver());

    mpMenuBar->GameOver();
	String const report = mpGame->EndBonus();
    Window::InfoBox(report, "Game Over - Gold Tile");
    mGameView.Reset();
}

int GameWindow::GameWarnBox(char const *messageText) {
	String message(messageText);
	String title = "Information";

	// expand shortcuts
	if (::str_eq(message, "FEWTILES")) {
		message = "You haven't created enough tiles to fill all the hands.";
		title = "Too Few Tiles";
	}

	title += " - Gold Tile";
	int const result = WarnBox(message, title);

	return result;
}

void GameWindow::HandleButtonDown(Point const &rMouse) {
	ASSERT(HasGame());
	ASSERT(!IsGamePaused());
	ASSERT(!mGameView.IsDragging());
    mMouseLast = rMouse;

	TileIdType id = mGameView.GetTileId(rMouse);
    if (id != Tile::ID_NONE) {
        if (!IsMouseCaptured()) {
            // Capture mouse to drag the tile
            CaptureMouse();
			Point const point = mGameView.TileCenter(id);
            WarpCursor(point);
            mMouseLast = point;
            mGameView.Activate(id);
            mDragTileDeltaX = 0;
            mDragTileDeltaY = 0;
			mMouseUpCnt = 0;
		}
        
    } else if (!mGameView.IsInHandArea(rMouse) && !mGameView.IsInSwapArea(rMouse)) {
        // Capture mouse to drag the board
    	CaptureMouse();
       	SetCursorDrag();
        mDragBoardFlag = true;
		mDragBoardPixelCnt = 0;
    }
}

void GameWindow::HandleButtonUp(Point const &rMouse) {
	ASSERT(HasGame());

	HandleMouseMove(rMouse);

	if (mDragBoardFlag) {
		if (mDragBoardPixelCnt < 5 && mGameView.IsLocalUsersTurn()) {
		    // Drags shorter than five pixels 
			// are treated as normal mouse-clicks 
			// which change or deactivate the target cell.
			mGameView.ToggleTargetCell(rMouse);
		}
   		StopDragging();
    } else {
        ASSERT(mGameView.GetActive() != Tile::ID_NONE);        
		ReleaseActiveTile(rMouse);
    }
}

void GameWindow::HandleMenuCommand(IdType command) {
    switch (command) {
    // File menu options
        case IDM_NEW: {
	        if (AreUnsavedChanges()) {
				OfferSaveGame();
			}
			OfferNewGame();
            break;
	    }

		case IDM_OPEN:
		case IDM_REOPEN:
		case IDM_REVERT:
		case IDM_SAVE:
		case IDM_SAVE_AS:
		case IDM_PRINT:
		    FAIL(); // TODO
		    break;

		case IDM_CLOSE:
	        if (AreUnsavedChanges()) {
				OfferSaveGame();
			}
			SetGame(NULL);
		    break;

        case IDM_EXIT: // terminate the application
			Close();
            break;

	    // Play menu options
	    case IDM_PASS:
	    case IDM_PLAY_PLAY: {
			    ASSERT(HasGame());
			    ASSERT(!IsGameOver());
				ASSERT(!IsGamePaused());
                bool pass_flag = (command == IDM_PASS);
			    Play(pass_flag);
			}
		    break;

	    case IDM_TAKE_BACK:
			ASSERT(HasGame());
		    ASSERT(!IsGameOver());
			ASSERT(!IsGamePaused());
			mGameView.Reset();
		    break;

        case IDM_SUGGEST:
			ASSERT(HasGame());
		    ASSERT(!IsGameOver());
			ASSERT(!IsGamePaused());
            mGameView.Reset();
			ASSERT(mThinkMode == THINK_IDLE);
            mThinkMode = THINK_SUGGEST;
            break;
             
	    case IDM_PAUSE:
			ASSERT(HasGame());
			mpGame->TogglePause();
		    break;

		case IDM_SWAP_ALL:
			ASSERT(HasGame());
		    ASSERT(!IsGameOver());
			ASSERT(!IsGamePaused());
			mGameView.SwapAll();
			break;

		case IDM_RESIGN:
			ASSERT(HasGame());
		    ASSERT(!IsGameOver());
			ASSERT(!IsGamePaused());
			ResignHand();
			break;

		case IDM_RESTART:
			ASSERT(HasGame());
			ASSERT(!IsGamePaused());
            RestartGame();
			break;

		case IDM_UNDO:
			ASSERT(HasGame());
			ASSERT(!IsGamePaused());
			ASSERT(mpGame->CanUndo());
			UndoTurn();
			break;

		case IDM_REDO:
			ASSERT(HasGame());
		    ASSERT(!IsGameOver());
			ASSERT(!IsGamePaused());
            ASSERT(mpGame->CanRedo());
			RedoTurn();
			break;

		case IDM_AUTOPAUSE:
        case IDM_PEEK:
	    case IDM_SHOW_CLOCKS:
	    case IDM_SHOW_GRID:
        case IDM_SHOW_SCORES:
            mpMenuBar->HandleMenuCommand(command);
            break;

	    // Tile Size menu options
        case IDM_TS1:
            SetTileSize(1);
            break;
        case IDM_TS2:
            SetTileSize(2);
            break;
        case IDM_TS3:
            SetTileSize(3);
            break;
        case IDM_TS4:
            SetTileSize(4);
            break;
        case IDM_TS5:
            SetTileSize(5);
            break;
        case IDM_TS6:
            SetTileSize(6);
            break;
        case IDM_TS7:
            SetTileSize(7);
            break;

	    // View menu options
        case IDM_RECENTER:
            Resize(ClientAreaWidth(), ClientAreaHeight());
            break;
		case IDM_ATTRIBUTES: {
			DisplayModes modes = DisplayModes(mGameView);
		    AttrBox box(modes);
			box.Run(this);
			modes = DisplayModes(box);
			mGameView.SetDisplayModes(modes);
			break;
		}
        case IDM_HINTS: {
			HintType hint_strength = HintType(mGameView);
			GameStyleType const game_style = mGameView.GameStyle();
			HintBox box(hint_strength, game_style);
			int result = box.Run(this);
			if (result == Dialog::RESULT_OK) {
			    hint_strength = HintType(box);
			    mGameView.SetHintStrength(hint_strength);
			} else {
        	    ASSERT(result == Dialog::RESULT_CANCEL);
			}
			break;
		}
		case IDM_ANIMATION:
		    FAIL(); // TODO
			break;

        // Help menu options
        case IDM_RULES:
	   	    InfoBox("RULES");
            break;
        case IDM_ABOUT: {
	   	    Dialog about("ABOUT");
			about.Run(this);
            break;
		}
		case IDM_WARRANTY:
	   	    InfoBox("WARRANTY");
            break;

		default:
			FAIL();
    }

	UpdateMenuBar();
	ForceRepaint();
}

LRESULT GameWindow::HandleMessage(MessageType message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
	    case WM_CLOSE: {
		    if (HasGame() && mpGame->HasUnsavedChanges()) {
				OfferSaveGame();
			}
			if (HasGame()) {
			    SetGame(NULL);
			}
		    result = Window::HandleMessage(message, wParam, lParam);
			break;
	    }

        case WM_COMMAND: { // menu command
	        IdType const command = LOWORD(wParam);
            HandleMenuCommand(command);
            break;
  	    }

        case WM_CREATE: { // initialize window
		    CREATESTRUCT * const p_create_struct = (CREATESTRUCT *)lParam;
			ASSERT(p_create_struct != NULL);
            Initialize(*p_create_struct);
            break;
        }

        case WM_LBUTTONDOWN: // begin left-click
			if (HasGame()) {
			    if (IsGamePaused()) {
				    mpGame->StartClock();
                } else if (!mGameView.IsDragging()) {
			        POINTS points = MAKEPOINTS(lParam);
			        Point mouse(points);
                    HandleButtonDown(mouse);
				}
			    ForceRepaint();
			    UpdateMenuBar();
			}
            break;

        case WM_LBUTTONUP: // complete left-click
			if (mGameView.IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS const points = MAKEPOINTS(lParam);
				    Point const mouse(points);
                    HandleButtonUp(mouse);
					UpdateMenuBar();
				} else {
					StopDragging();
				}
    		    ForceRepaint();
        	}
  		    break;

        case WM_MOUSEMOVE: // mouse position update
			if (mGameView.IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS const points = MAKEPOINTS(lParam);
				    Point const mouse(points);
	                HandleMouseMove(mouse);
				} else {
					StopDragging();
				}
                ForceRepaint();
			}
            break;

		case WM_MOUSEWHEEL: { // wheel position update
            int const z_delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mGameView.StartCellOffset(0, -z_delta/5);
			ForceRepaint();
			break;
		}

	    case WM_PAINT: // request to repaint the client area
            Repaint();
            break;
           
        case WM_SIZE: { // resize request
            PixelCntType const client_area_width = LOWORD(lParam);
            PixelCntType const client_area_height = HIWORD(lParam);
            Resize(client_area_width, client_area_height);
            break;
        }

		case WM_TIMER: { // timer popped
			int const timer_id = int(wParam);
			if (timer_id == ID_CLOCK_TIMER) {
			    if (mpMenuBar->AreClocksVisible() && !IsGamePaused() && !IsGameOver()) {
					// don't update menus because they would flicker
			        ForceRepaint();  // to update active player's clock display
			    } else {
					SetTimer(TIMEOUT_MSEC, timer_id);
				}
			}
			break;
		}

	    default:  // invoke message handler of the base class
		    result = Window::HandleMessage(message, wParam, lParam);
		    break;
    }

	// give the Think fiber an opportunity to run
	if (mThinkMode != THINK_IDLE) {
	    ASSERT(mThinkFiber != NULL);
	    Win::SwitchToFiber(mThinkFiber);
	}

	return result;
}

void GameWindow::HandleMouseMove(Point const &rMouse) {
    long const drag_x = rMouse.X() - mMouseLast.X();
    long const drag_y = rMouse.Y() - mMouseLast.Y();
    mMouseLast = rMouse;

    if (mDragBoardFlag) {
        mGameView.StartCellOffset(drag_x, drag_y);
		mDragBoardPixelCnt += ::abs(drag_x) + ::abs(drag_y);

	} else {
        ASSERT(mGameView.GetActive() != Tile::ID_NONE);        
        mDragTileDeltaX += drag_x;
        mDragTileDeltaY += drag_y;
    }
}

void GameWindow::InfoBox(char const *messageText) {
	String message = messageText;
	String title = "Information";

	// expand shortcuts
	if (::str_eq(message, "RULES")) {
		message = "The rules of Gold Tile are available online at http://code.google.com/p/gold-tiles/wiki/Playing";
		title = "Rules";
	} else if (::str_eq(message, "WARRANTY")) {
		message =  "There is NO warranty for the program, to the extent permitted by ";
		message += "applicable law.  Except when otherwise stated in writing the ";
		message += "copyright holders and/or other parties provide the program ";
		message += "\"AS IS\" without warranty of any kind, either expressed or implied, ";
	    message += "including, but not limited to, the implied warranties of ";
		message += "merchantability and fitness for a particular purpose.  The entire ";
		message += "risk as to the quality and performance of the program is with you. ";
		message += "Should the program prove defective, you assume the cost of all ";
		message += "necessary servicing, repair, or correction.";
		title = "Warranty";
	}

	title += " - Gold Tile";
	Window::InfoBox(message, title);
}

void GameWindow::LoadPlayerOptions(Hand const &rHand) {
	String const player_name = rHand.PlayerName();
	Player const &r_player = Player::rLookup(player_name);

	mpMenuBar->LoadPlayerOptions(r_player);
	mGameView.LoadPlayerOptions(r_player);
}

char const *GameWindow::Name(void) const {
	return "Gold Tile - a game by Stephen Gold";
}

void GameWindow::OfferNewGame(void) {
    // set up default options
	GameOpt game_options;
	ParmBox1 parmbox1(game_options);
	ParmBox2 parmbox2(game_options);
	ParmBox3 parmbox3(game_options);
	TileBox tilebox(game_options);

	HandOpts hand_options;

	if (HasGame()) {
		// copy options from game
		game_options = *mpGame;
		game_options.SetRules(RULES_REPLAY);

	    Hands hands(*mpGame);
		Hands::ConstIterator i_hand;
		for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
			HandOpt const opt(*i_hand);
			hand_options.Append(opt);
		}
		ASSERT(hand_options.Count() == hands.Count());
	}

STEP1:
	GameStyleType const old_game_style = game_options;

	// first dialog:  prompt for game style, time limit, and standard/custom rules
	int result = parmbox1.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
		return;
	}
	ASSERT(result == Dialog::RESULT_OK);

	if (GameStyleType(game_options) != old_game_style) {  // changing game styles
		game_options.StyleChange();

		// invalidate all hand options
		hand_options.MakeEmpty();
	}
	RulesType const rules = RulesType(game_options);
	if (rules == RULES_STANDARD) {
		game_options.Standardize();
	}

STEP2:
	if (rules == RULES_CUSTOM) {
	    // second dialog:  prompt for grid type and topology
	    result = parmbox2.Run(this);
	}
    if (result == Dialog::RESULT_CANCEL) {
	    return;
    } else if (result == Dialog::RESULT_BACK) {
	    goto STEP1;
    }
    ASSERT(result == Dialog::RESULT_OK);

STEP3:
	if (rules == RULES_CUSTOM) {
	    // third dialog:  prompt for attribute count, clones, hand size, and number of hands
	    result = parmbox3.Run(this);
	}
    if (result == Dialog::RESULT_CANCEL) {
	    return;
    } else if (result == Dialog::RESULT_BACK) {
	    goto STEP2;
    }
    ASSERT(result == Dialog::RESULT_OK);

	unsigned const hand_cnt = game_options.HandsDealt();
	if (hand_cnt > hand_options.Count()) { // adding new hands
		// initialize a new hand_option for each new hand
	    GameStyleType const game_style = GameStyleType(game_options);
		Strings const player_names = hand_options.AllPlayerNames();
		for (unsigned i_hand = hand_options.Count(); i_hand < hand_cnt; i_hand++) {
			HandOpt options = HandOpt(game_style, player_names);
			hand_options.Append(options);
			ASSERT(i_hand + 1 == hand_options.Count());
		}
	}
	ASSERT(hand_options.Count() >= hand_cnt);

STEP4:
	if (rules == RULES_CUSTOM) {
	    // fourth dialog:  number of values for each attribute
	    result = tilebox.Run(this);
	}
	if (result == Dialog::RESULT_CANCEL) {
	    return;
	} else if (result == Dialog::RESULT_BACK) {
	    goto STEP3;
	}
	ASSERT(result == Dialog::RESULT_OK);

	// check the sanity of the parameters so far
	unsigned const hand_size = game_options.HandSize();
	long const tiles_needed = hand_cnt*hand_size;
	long const total_tile_cnt = game_options.TotalTileCnt();
	if (total_tile_cnt < tiles_needed) {
		result = GameWarnBox("FEWTILES");
		if (result == IDCANCEL) {
		    return;
	    } else if (result == IDTRYAGAIN) {
			goto STEP3;
	    }
		ASSERT(result == IDCONTINUE);
	}

	// fifth dialog and onward:  parameters of each hand
	for (unsigned i_hand = 0; i_hand < hand_cnt; ) {
		HandBox handbox(i_hand+1, hand_cnt, hand_options[i_hand]);
		result = handbox.Run(this);
		if (result == Dialog::RESULT_CANCEL) {
		    return;
		} else if (result == Dialog::RESULT_BACK) {
			if (i_hand == 0) {
				goto STEP4;
			} else {
				i_hand--;
			}
		} else {
		    ASSERT(result == Dialog::RESULT_OK);
			hand_options[i_hand] = HandOpt(handbox);
			i_hand++;
		}
	}

	// can't cancel now - set up the new game
	hand_options.Truncate(hand_cnt);

	SetCursorBusy(); // constructing tiles may cause a noticeable delay
	Game *const p_new_game = new Game(game_options, hand_options);
	ASSERT(p_new_game != NULL);
	SetCursorSelect();

	SetGame(p_new_game);
}

void GameWindow::OfferSaveGame(void) {
#if 0
    YesNo box("UNSAVED");
	box.Run(this);
	// TODO
#endif
}

void GameWindow::Play(bool passFlag) {
	ASSERT(HasGame());
	ASSERT(mpMenuBar != NULL);
	ASSERT(mGameView.GetActive() == Tile::ID_NONE);
	ASSERT(!IsGameOver());
	ASSERT(!IsGamePaused());

    Move move;

	// check whether the player has run out of time
	if (!mpGame->IsOutOfTime()) {
		move = mGameView.GetMove(true);
	} else {
		// hand ran out of time:  force resignation
		move.MakeResign(mpGame->ActiveTiles()); 
	}

	// check whether the move is a legal one
	char const *reason;
	bool const is_legal = mpGame->IsLegalMove(move, reason);
    if (is_legal && move.IsPass() == passFlag) {
        mpGame->FinishTurn(move);

        if (!mpGame->IsOver()) {
			// the game isn't over yet, so proceed to the next hand
			String const old_player_name = SaveHandOptions();
            mpGame->ActivateNextHand();
			ChangeHand(old_player_name);

        } else {
			GameOver();
        }

    } else if (!is_legal) { // explain the issue
        RuleBox(reason);
		if (::str_eq(reason, "FIRST")) {
			mGameView.Reset();
		}
	}
}

void GameWindow::RedoTurn(void) {
	ASSERT(HasGame());
	ASSERT(mpMenuBar != NULL);
	ASSERT(!IsGamePaused());

	mpGame->StopClock();
	String const old_player_name = SaveHandOptions();

    mpGame->Redo();
	mGameView.Reset();

	ChangeHand(old_player_name);
}

void GameWindow::ReleaseActiveTile(Point const &rMouse) {
	ASSERT(HasGame());

    TileIdType const id = mGameView.GetActive(); 
     
	// Determine where the active tile came from.
    bool const from_hand = mGameView.IsInHand(id);
    bool const from_swap = mGameView.IsInSwap(id);
	ASSERT(!(from_hand && from_swap)); 
	bool const from_board = !(from_hand || from_swap);
	Cell from_cell;
    if (from_board) {
        from_cell = mGameView.LocateTile(id);
    }

	// Determine where the active tile was released to.
	bool const to_hand = mGameView.IsInHandArea(rMouse);
	bool const to_swap = (!to_hand) && mGameView.IsInSwapArea(rMouse); // overlap is possible
	bool to_board = !(to_hand || to_swap);
	Cell to_cell;
	if (to_board) {
		to_cell = mGameView.GetPointCell(rMouse);
		if (!mGameView.IsInCellArea(rMouse, to_cell)) {
			return;
		}
	}

	if (from_hand && to_hand ||
	    from_swap && to_swap ||
	    from_board && to_board && from_cell == to_cell)
	{
		// Trivial drags which don't actually move the tile
		// are treated as normal mouse-clicks which
		// activate/deactivate the tile or play it to the
		// target cell.
		if (mMouseUpCnt == 1) {
			StopDragging(); // deactivates the tile
    		return;
		} else {
		    ASSERT(mMouseUpCnt == 0);
			if (mGameView.IsTargetSet()) {
			    to_board = true;
				to_cell = mGameView.TargetCell();
			} else {
        		mMouseUpCnt = 1;
				return;
			}
		}
	}

	if (to_board && (!to_cell.IsValid() || mGameView.GetCellTile(to_cell) != Tile::ID_NONE)) {
		// cell conflict - can't construct a move in the usual way
	    RuleBox("EMPTY");
    	StopDragging();
		return;
	}

	// move the active tile back to the active hand
	if (from_board) {
        mGameView.BoardToHand();
    } else if (from_swap) {
        mGameView.SwapToHand();
    }

	// move tile from the active hand to its destination
    if (to_board) {
        mGameView.HandToCell(to_cell);
    } else if (to_swap) {
        ASSERT(!from_swap);
        mGameView.HandToSwap();
    }

	// Check whether the new partial move is legal.
    Move const move_so_far = mGameView.GetMove(true);
    char const *reason;
	bool const legal = mpGame->IsLegalMove(move_so_far, reason);

	if (!legal && (to_swap || !::str_eq(reason, "FIRST"))) {  
		// It's illegal, even as a partial move:  reverse it.
        if (to_board) {
            mGameView.BoardToHand();
        } else if (to_swap) {
            mGameView.SwapToHand();
        }
     	if (from_board) {
            mGameView.HandToCell(from_cell);
        } else if (from_swap) {
            mGameView.HandToSwap();
        }

		// Explain to the player why it was illegal.
		if (::str_eq(reason, "START") && !from_board) {
			reason = "STARTSIMPLE";
	    }
	    RuleBox(reason);
	}

	mGameView.ResetTargetCell();
	StopDragging();
}

void GameWindow::Repaint(void) {
	BeginPaint();
    Canvas canvas(*this);
	mGameView.Repaint(canvas);
	EndPaint();

	// restart the timer
	SetTimer(TIMEOUT_MSEC, ID_CLOCK_TIMER);

	if (mInitialNewGame) {
		ASSERT(!HasGame());
		mInitialNewGame = false;
		OfferNewGame();
	}
}

void GameWindow::ResignHand(void) {
	ASSERT(HasGame());
	ASSERT(mpMenuBar != NULL);
	ASSERT(!IsGameOver());
	ASSERT(!IsGamePaused());

	mGameView.Reset();

	Move move;
	move.MakeResign(Tiles(mGameView));
    mpGame->FinishTurn(move);

    if (!mpGame->IsOver()) {
		// the game isn't over yet, so proceed to the next hand
		String const old_player_name = SaveHandOptions();
		mpGame->ActivateNextHand();
		ChangeHand(old_player_name);

	} else {
        GameOver();
    }
}

void GameWindow::Resize(PixelCntType clientAreaWidth, PixelCntType clientAreaHeight) {
    PixelCntType old_height = ClientAreaHeight();
    PixelCntType old_width = ClientAreaWidth();
    SetClientArea(clientAreaWidth, clientAreaHeight);
    mGameView.Recenter(old_height, old_width);
    ForceRepaint();
}

void GameWindow::RestartGame(void) {
	ASSERT(HasGame());
	ASSERT(mpMenuBar != NULL);
	ASSERT(!IsGamePaused());

	if (!IsGameOver()) {
	    mpGame->StopClock();
	}
	String const old_player_name = SaveHandOptions();

	mpGame->Restart();
	mGameView.Reset();

	ChangeHand(old_player_name);
}

void GameWindow::RuleBox(char const *reason) {
	// expand reason shortcuts
	String title;
	String const message = Board::ReasonMessage(reason, title);

	title += " - Gold Tile";
	ErrorBox(message, title);
}

String GameWindow::SaveHandOptions(void) const {
	Hand const old_hand = Hand(*mpGame);
	if (old_hand.IsLocalUser()) {
	    SavePlayerOptions(old_hand);
	}
	String const result = old_hand.PlayerName();

	return result;
}

void GameWindow::SavePlayerOptions(Hand const &rHand) const {
	ASSERT(mpMenuBar != NULL);

	String const player_name = rHand.PlayerName();
	Player &r_player = Player::rLookup(player_name);

	mpMenuBar->SavePlayerOptions(r_player);
	mGameView.SavePlayerOptions(r_player);
}

void GameWindow::SetGame(Game *pGame) {
	ASSERT(mpMenuBar != NULL);

	GameStyleType old_style = GAME_STYLE_NONE;
	if (HasGame()) {
    	// TODO: free old Game object?
		old_style = mpGame->Style();
	}

	mpGame = pGame;

	mGameView.SetGame(mpGame);
	mpMenuBar->NewGame(old_style);

    SetTileSize(GameView::TILE_SIZE_DEFAULT);
	if (HasGame()) {
	    Hands const hands = Hands(*mpGame);
		Hands::ConstIterator i_hand;
		for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
			SavePlayerOptions(*i_hand);
		}

		ChangeHand("");
	}

	ForceRepaint();
    UpdateMenuBar();

	if (HasGame()) {
		ASSERT(!mpGame->CanRedo());
		ASSERT(!mpGame->IsOver());
    	String const report = pGame->BestRunReport();
	    Window::InfoBox(report, "Opening Bids - Gold Tile");
	}
}

void GameWindow::SetTileSize(unsigned size) {
	ASSERT(mpMenuBar != NULL);
	ASSERT(size >= GameView::TILE_SIZE_MIN);
	ASSERT(size <= GameView::TILE_SIZE_MAX);

    mpMenuBar->SetTileSize(size);
    mGameView.SetTileSize(size);
}

void GameWindow::StopDragging(void) {
	ASSERT(mGameView.IsDragging());

	mGameView.Deactivate();
   	SetCursorSelect();
	ReleaseMouse();
	mDragBoardFlag = false;

	ASSERT(!mGameView.IsDragging());
	ASSERT(!IsMouseCaptured());
}

// code executed by the think fiber
void GameWindow::Think(void) {
	for (;;) {
		while (mThinkMode == THINK_IDLE) {
			Yields();
		}

		ASSERT(mThinkMode == THINK_AUTOPLAY || mThinkMode == THINK_SUGGEST);
	    UpdateMenuBar();
	    mGameView.Suggest();

		if (mThinkMode == THINK_SUGGEST) {
			mGameView.ResetTargetCell();
	        mThinkMode = THINK_IDLE;

		} else {
			ASSERT(mThinkMode == THINK_AUTOPLAY);

			// reveal the computer's move
		    ForceRepaint();

		    // pause for human comprehension
		    MsecIntervalType const start = ::milliseconds();
		    while (::milliseconds() <= start + PAUSE_MSEC) {
		        Yields();
		    }

			// commit the move
	        mThinkMode = THINK_IDLE;
			bool const is_pass = mGameView.IsPass();
            Play(is_pass);
		}

	    ForceRepaint();
	    UpdateMenuBar();
	}
}

void GameWindow::UndoTurn(void) {
	ASSERT(HasGame());
	ASSERT(mpMenuBar != NULL);
	ASSERT(!IsGamePaused());
	ASSERT(mpGame->CanUndo());

	if (!IsGameOver()) {
	    mpGame->StopClock();
	}
	String const old_player_name = SaveHandOptions();

    mpGame->Undo();
	mGameView.Reset();

	ChangeHand(old_player_name);
}

void GameWindow::UpdateMenuBar(void) {
	mpMenuBar->Update(mThinkMode != THINK_IDLE);
    Window::UpdateMenuBar();
}


// inquiry methods

bool GameWindow::AreUnsavedChanges(void) const {
     bool result = false;
     if (HasGame()) {
         result = mpGame->HasUnsavedChanges();
     }
     
     return result;
}

bool GameWindow::HasGame(void) const {
	bool const result = (mpGame != NULL);

	return result;
}

bool GameWindow::IsDraggingBoard(void) const {
	return mDragBoardFlag;
}

bool GameWindow::IsGameOver(void) const {
	bool result = false;
	if (HasGame()) {
		result = mpGame->IsOver();
	}

	return result;
}

bool GameWindow::IsGamePaused(void) const {
	bool result = false;
	if (HasGame() && !mpGame->IsOver()) {
		result = mpGame->IsPaused();
	}

	return result;
}
#endif // !defined(_WINDOWS)
