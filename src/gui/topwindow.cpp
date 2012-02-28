// File:    topwindow.cpp
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

#include "gui/canvas.hpp"
#include "gui/handbox.hpp"
#include "gui/hintbox.hpp"
#include "gui/menubar.hpp"
#include "gui/parmbox1.hpp"
#include "gui/parmbox2.hpp"
#include "gui/parmbox3.hpp"
#include "gui/player.hpp"
#include "gui/resource.hpp"
#include "gui/tilebox.hpp"
#include "gui/topwindow.hpp"
#include "gui/win_types.hpp"
#include "gui/windowclass.hpp"
#include "gui/yesno.hpp"
#include "hand.hpp"
#include "move.hpp"
#include "strings.hpp"

// static data of the class

WindowClass *TopWindow::mspClass = NULL;

// message handler (callback) for top window
static LRESULT CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParam,
	LPARAM lParam)
{
	ASSERT(windowHandle != NULL);
    TopWindow *window = (TopWindow *)Window::Lookup(windowHandle);

	LRESULT result;
	if (window == NULL) { // unknown window
		// invoke default message handler
		result = Win::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	ASSERT(window->Handle() == windowHandle);
        result = window->HandleMessage(message, wParam, lParam);
	}

	return result;
}

// lifecycle

TopWindow::TopWindow(HINSTANCE applicationInstance, Game *pGame):
    mMouseLast(0, 0),
	mGameView(*pGame)
{
	ASSERT(Handle() == 0);
	ASSERT(applicationInstance != NULL);

	LPCTSTR class_name = "TOPWINDOW";
    if (mspClass == NULL) {
		// for first instance:  create a Microsoft Windows window class
		mspClass = new WindowClass(applicationInstance, &message_handler, class_name);
	    ASSERT(mspClass != NULL);
		mspClass->RegisterClass();
	}

	// Make this TopWindow object accessable to its message handler before WM_CREATE.
    ASSERT(mspNewlyCreatedWindow == NULL);
	mspNewlyCreatedWindow = this;

    mDragBoardFlag = false;
	mpGame = pGame;
	mInitialNewGame = (pGame == NULL);
    mpMenuBar = NULL;

	HWND desktop_handle = Win::GetDesktopWindow();
	RECT rect;
	Win::GetWindowRect(desktop_handle, &rect);
	Rect desktop_bounds(rect);

	// create Microsoft Windows window
	LPCTSTR name = Name();
	DWORD window_style = WS_OVERLAPPEDWINDOW;
	PCntType height = PCntType(0.8*double(desktop_bounds.Height()));
	PCntType width = PCntType(0.8*double(desktop_bounds.Width()));
	LogicalXType x = width/8;
	LogicalYType y = height/8;
	HWND parent = NULL;
	HMENU menu = NULL;
	LPVOID parameters = NULL;
	ASSERT(class_name != NULL);
	ASSERT(name != NULL);
    HWND handle = Win::CreateWindow(class_name, name, window_style, x, y, 
                             width, height, parent, menu, applicationInstance, 
                             parameters);
    ASSERT(Handle() == handle);

	// wait for message_handler() to receive a message with this handle
}

TopWindow::~TopWindow(void) {
    delete mpMenuBar;
}

void TopWindow::Initialize(CREATESTRUCT const &rCreateStruct) {
    // Initialization which takes place after the Microsoft Windows window
    // has received its WM_CREATE message.

	ASSERT(mpMenuBar == NULL);

	Window::Initialize(rCreateStruct);

	mpMenuBar = new MenuBar(rCreateStruct, mGameView);
	ASSERT(mpMenuBar != NULL);

	mGameView.SetWindow(this, mpMenuBar);
    mGameView.SetTileWidth(IDM_LARGE_TILES);
	if (mpGame != NULL) {
	    Hands hands = Hands(*mpGame);
		Hands::ConstIterator i_hand;
		for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
			SavePlayerOptions(*i_hand);
		}
	}

	if (mpGame != NULL && mpGame->Style() != GAME_STYLE_CHALLENGE) {
		mpGame->StartClock();
	}

	SetTimer(TIMEOUT_MSEC, ID_CLOCK_TIMER);
	UpdateMenuBar();
}


// misc methods

String TopWindow::ClockText(Hand &rHand) const {
	ASSERT(mpGame != NULL);

	int seconds = mpGame->Seconds(rHand);

	// convert to minutes and seconds
	bool minus_sign;
	if (seconds >= 0) {
		minus_sign = false;
	} else {
		minus_sign = true;
		seconds = -seconds;
	}

	unsigned minutes, tens_of_seconds;
	minutes = seconds / SECONDS_PER_MINUTE;
	seconds -= minutes*SECONDS_PER_MINUTE;
    tens_of_seconds = seconds / 10;
	seconds -= tens_of_seconds*10;

	ASSERT(seconds >= 0);
	ASSERT(seconds <= 9);
	ASSERT(tens_of_seconds >= 0);
	ASSERT(tens_of_seconds <= 5);
	ASSERT(minutes >= 0);

	String result;
	if (minus_sign) {
		result += "-";
	}
	result += String(minutes) + ":" 
		   + String(tens_of_seconds) + String(seconds);

	return result;
}

long TopWindow::DragTileDeltaX(void) const {
	return mDragTileDeltaX;
}

long TopWindow::DragTileDeltaY(void) const {
	return mDragTileDeltaY;
}

void TopWindow::HandleButtonDown(Point const &rMouse) {
	ASSERT(mpGame != NULL);
	ASSERT(!IsGamePaused());
	ASSERT(!mGameView.IsDragging());
    mMouseLast = rMouse;

	TileIdType id = mGameView.GetTileId(rMouse);
    if (id != Tile::ID_NONE) {
        if (!IsMouseCaptured()) {
            // Capture mouse to drag the tile
            CaptureMouse();
			Point point = mGameView.TileCenter(id);
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

void TopWindow::HandleButtonUp(Point const &rMouse) {
	ASSERT(mpGame != NULL);
	HandleMouseMove(rMouse);

	if (mDragBoardFlag) {
		if (mDragBoardPixelCnt < 5 && !mpGame->IsOver()) {
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

void TopWindow::HandleMenuCommand(IdType command) {
    switch (command) {
    // File menu options
        case IDM_NEW: {
	        if (mpGame != NULL && mpGame->HasUnsavedChanges()) {
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
	        if (mpGame != NULL && mpGame->HasUnsavedChanges()) {
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
			    ASSERT(!IsGameOver());
				ASSERT(!IsGamePaused());
                bool pass_flag = (command == IDM_PASS);
			    Play(pass_flag);
			}
		    break;

	    case IDM_TAKE_BACK:
			mGameView.Reset();
		    break;

	    case IDM_PAUSE:
			TogglePause();
		    break;

		case IDM_SWAP_ALL:
			mGameView.SwapAll();
			break;

		case IDM_RESIGN:
			break;

		case IDM_RESTART:
		case IDM_UNDO:
		case IDM_REDO:
		    FAIL(); // TODO
			break;

		case IDM_AUTOPAUSE:
        case IDM_PEEK:
	    case IDM_SHOW_CLOCKS:
	    case IDM_SHOW_GRID:
        case IDM_SHOW_SCORES:
            mpMenuBar->HandleMenuCommand(command);
            break;

	    // View menu options
        case IDM_SMALL_TILES:
        case IDM_MEDIUM_TILES:
        case IDM_LARGE_TILES:
            mGameView.SetTileWidth(command);
            break;
        case IDM_RECENTER:
            Resize(ClientAreaWidth(), ClientAreaHeight());
            break;
		case IDM_ATTRIBUTES:
		    FAIL(); // TODO
			break;
        case IDM_HINTS: {
			HintType hint_strength = HintType(mGameView);
			GameStyleType game_style = mGameView.GameStyle();
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
        case IDM_RULES: {
	   	    InfoBox("RULES");
            break;
		}
        case IDM_ABOUT: {
	   	    Dialog about("ABOUT");
			about.Run(this);
            break;
		}
        case IDM_WARRANTY: {
	   	    Dialog warranty("WARRANTY");
			warranty.Run(this);
            break;
		}

		default:
			FAIL();
    }

	UpdateMenuBar();
	ForceRepaint();
}

LRESULT TopWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
	    case WM_CLOSE: {
		    if (mpGame != NULL && mpGame->HasUnsavedChanges()) {
				OfferSaveGame();
			}
			if (mpGame != NULL) {
			    SetGame(NULL);
			}
		    result = Window::HandleMessage(message, wParam, lParam);
			break;
	    }

        case WM_COMMAND: { // menu command
	        IdType command = LOWORD(wParam);
            HandleMenuCommand(command);
            break;
  	    }

        case WM_CREATE: { // initialize window
		    CREATESTRUCT *p_create_struct = (CREATESTRUCT *)lParam;
			ASSERT(p_create_struct != NULL);
            Initialize(*p_create_struct);
            break;
        }

        case WM_LBUTTONDOWN: // begin left-click
			if (mpGame != NULL) {
			    if (IsGamePaused()) {
				    TogglePause();
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
				    POINTS points = MAKEPOINTS(lParam);
				    Point mouse(points);
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
				    POINTS points = MAKEPOINTS(lParam);
				    Point mouse(points);
	                HandleMouseMove(mouse);
				} else {
					StopDragging();
				}
                ForceRepaint();
			}
            break;

		case WM_MOUSEWHEEL: {
            int z_delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mGameView.StartCellOffset(0, -z_delta/5);
			ForceRepaint();
			break;
		}

	    case WM_PAINT: // repaint
            Repaint();
            break;
           
        case WM_SIZE: { // resize
            PCntType clientAreaWidth = LOWORD(lParam);
            PCntType clientAreaHeight = HIWORD(lParam);
            Resize(clientAreaWidth, clientAreaHeight);
            break;
        }

		case WM_TIMER: { // timeout
			int timer_id = int(wParam);
			if (timer_id == ID_CLOCK_TIMER) {
			    if (mpMenuBar->AreClocksVisible() && !IsGamePaused() && !IsGameOver()) {
					// don't update menus because that would cause flicker
			        ForceRepaint();  // for active player's clock
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

    return result;
}

void TopWindow::HandleMouseMove(Point const &rMouse) {
    long drag_x = rMouse.X() - mMouseLast.X();
    long drag_y = rMouse.Y() - mMouseLast.Y();
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

void TopWindow::InfoBox(char const *messageText) {
	String message = messageText;
	String title = "Information";

	// expand shortcuts
	if (::str_eq(message, "RULES")) {
		message = "The rules of Gold Tile are available online at http://code.google.com/p/gold-tiles/wiki/Playing";
		title = "Rules";
	}

	title += " - Gold Tile";
	Window::InfoBox(message, title);
}

void TopWindow::LoadPlayerOptions(Hand const &rHand) {
	String player_name = rHand.PlayerName();
	Player const &r_player = Player::rLookup(player_name);
	mpMenuBar->LoadPlayerOptions(r_player);
	mGameView.SetStartCellPosition(Point(r_player));
}

int TopWindow::MessageDispatchLoop(void) {
    int exit_code;

	HACCEL table = GetAcceleratorTable("HOTKEYS");

	for (;;) {
        MSG message;
	    HWND any_window = NULL;
		UINT no_filtering = 0;
        BOOL success = Win::GetMessage(&message, any_window, no_filtering, no_filtering);
        if (success == 0) {   // retrieved a WM_QUIT message
			exit_code = message.wParam;
			break;
		} else if (success == -1) { // error in GetMessage()
            exit_code = -1;
			break;
		}

		int translated = Win::TranslateAccelerator(Handle(), table, &message);
		if (!translated) {
            Win::TranslateMessage(&message); 
            Win::DispatchMessage(&message); 
        } 
    }

	return exit_code;
}

char const *TopWindow::Name(void) const {
	return "Gold Tile - a game by Stephen Gold";
}

void TopWindow::OfferNewGame(void) {
	// set up first dialog box
	GameStyleType old_game_style = mGameView.GameStyle();
	unsigned seconds_per_hand = SECONDS_PER_MINUTE * ParmBox1::PLAYER_MINUTES_DEFAULT;
	if (mpGame != NULL) {
		seconds_per_hand = mpGame->SecondsPerHand();
	}
	ParmBox1 parmbox1(old_game_style, seconds_per_hand);

	// set up second dialog box
	bool wrap_flag;
	IndexType height, width;
	Cell::GetTopology(wrap_flag, height, width);
	GridType grid = Cell::Grid();
	ParmBox2 parmbox2(wrap_flag, height, width, grid);

	// begin setting up third dialog box
	unsigned attribute_cnt = Tile::AttributeCnt();
	unsigned tile_redundancy = Game::TILE_REDUNDANCY_DEFAULT;
	unsigned hand_cnt = HAND_CNT_DEFAULT;
	unsigned hand_size = Game::HAND_SIZE_DEFAULT;

	unsigned max_attribute_cnt = Tile::AttributeCnt();
	AValueType *max_attribute_values = new AValueType[max_attribute_cnt];
	ASSERT(max_attribute_values != NULL);
	for (unsigned i_attr = 0; i_attr < max_attribute_cnt; i_attr++) {
		max_attribute_values[i_attr] = Tile::ValueMax(i_attr);
	}

	// begin setting up fifth dialog box
	unsigned max_hand_cnt = 0;
	Strings player_names;
    Indices auto_hands;
    Indices remote_hands;
	std::vector<IpAddressType> ips;
	if (mpGame != NULL) {
	    Hands hands = Hands(*mpGame);
		max_hand_cnt = hands.Count();
		Hands::ConstIterator i_hand;
		for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
			String name = i_hand->PlayerName();
			player_names.Append(name);
			// TODO add to auto_hands and/or remote_hands
			ips.push_back(0);  // produces 0.0.0.0
		}
	}

STEP1:
	// first dialog:  prompt for game style and time limit
	int result = parmbox1.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
	    delete[] max_attribute_values;
		return;
	}
	ASSERT(result == Dialog::RESULT_OK);
	GameStyleType game_style = GameStyleType(parmbox1);
	seconds_per_hand = parmbox1.PlayerSeconds();

	bool new_game_style = (game_style != old_game_style);
	old_game_style = GAME_STYLE_NONE;
	if (new_game_style && max_hand_cnt > 0) {
		max_hand_cnt = 0;
		player_names.MakeEmpty();
		auto_hands.MakeEmpty();
		remote_hands.MakeEmpty();
		ips.clear();
	}

STEP2:
	// second dialog:  prompt for grid type and topology
	result = parmbox2.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
	    delete[] max_attribute_values;
		return;
	} else if (result == Dialog::RESULT_BACK) {
		goto STEP1;
	}
	ASSERT(result == Dialog::RESULT_OK);
	grid = GridType(parmbox2);
	wrap_flag = parmbox2.DoesWrap();
	height = parmbox2.Height();
	width = parmbox2.Width();

	// finish setting up third dialog
	if (new_game_style) {
		if (game_style == GAME_STYLE_PRACTICE) {
		    hand_cnt = 1;
		}
		new_game_style = false;

	} else if (mpGame != NULL) {
		// set based on previous game
		tile_redundancy = mpGame->Redundancy();
	    hand_size = mpGame->HandSize();
	    hand_cnt = Hands(*mpGame).Count();
	}

STEP3:
	// third dialog:  prompt for attribute count, clones, hand size, and number of hands
	unsigned clones_per_tile = tile_redundancy - 1;
	ParmBox3 parmbox3(attribute_cnt, clones_per_tile, hand_size, hand_cnt);
	result = parmbox3.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
	    delete[] max_attribute_values;
		return;
	} else if (result == Dialog::RESULT_BACK) {
		goto STEP2;
	}
	ASSERT(result == Dialog::RESULT_OK);
	attribute_cnt = parmbox3.AttributeCnt();
	tile_redundancy = 1 + parmbox3.ClonesPerTile();
	hand_size = parmbox3.HandSize();
	hand_cnt = parmbox3.HandCnt();

	// set up fourth dialog
   	if (attribute_cnt > max_attribute_cnt) {
		// allocate storage for more attribute limits
	    AValueType *new_max_attribute_values = new AValueType[attribute_cnt];

		// copy old limits
	    for (unsigned i_attr = 0; i_attr < max_attribute_cnt; i_attr++) {
		    new_max_attribute_values[i_attr] = max_attribute_values[i_attr];
	    }
		delete[] max_attribute_values;

		// initialize limits for new attributes
		for (unsigned i_attr = max_attribute_cnt; i_attr < attribute_cnt; i_attr++) {
			new_max_attribute_values[i_attr] = Tile::VALUE_CNT_DEFAULT - 1;
		}

		max_attribute_cnt = attribute_cnt;
		max_attribute_values = new_max_attribute_values;
	}
	String template_name = "TILEBOX" + String(attribute_cnt);
	TileBox tilebox((char const *)template_name, attribute_cnt, max_attribute_values);

	// further work on fifth dialog
   	if (hand_cnt > max_hand_cnt) {
		// initialize new addresses and names
		for (unsigned i = max_hand_cnt; i < hand_cnt; i++) {
			String name;
			switch (game_style) {
			case GAME_STYLE_PRACTICE:
				name = "Player";
				break;
			case GAME_STYLE_DEBUG:
				name = "Tester";
				break;
			case GAME_STYLE_FRIENDLY:
			case GAME_STYLE_CHALLENGE:
				name = player_names.InventUnique("Player");
				break;
			default:
				FAIL();
			}
			player_names.Append(name);
			// TODO add to auto_hands and/or remote_hands
			ips.push_back(0);  // produces 0.0.0.0
		}
		max_hand_cnt = hand_cnt;
	}
	ASSERT(max_hand_cnt >= hand_cnt);

STEP4:
	// fourth dialog:  number of values for each attribute
	result = tilebox.Run(this);
    TileBox::ValueType *num_values = tilebox.NumValues();

	// copy new limits
    for (unsigned i_attr = 0; i_attr < max_attribute_cnt; i_attr++) {
		ASSERT(num_values[i_attr] >= Tile::VALUE_CNT_MIN);
		ASSERT(num_values[i_attr] <= Tile::VALUE_CNT_MAX);
	    max_attribute_values[i_attr] = num_values[i_attr] - 1;
    }
	if (result == Dialog::RESULT_CANCEL) {
	    delete[] max_attribute_values;
		return;
	} else if (result == Dialog::RESULT_BACK) {
		goto STEP3;
	}
	ASSERT(result == Dialog::RESULT_OK);

	// check sanity of the parameters so far
	unsigned tiles_needed = hand_cnt*hand_size;

	unsigned long tile_cnt = tile_redundancy;
	for (unsigned i_attr = 0; i_attr < max_attribute_cnt; i_attr++) {
		tile_cnt *= num_values[i_attr];
	}

	if (tile_cnt < tiles_needed) {
		result = WarnBox("FEWTILES");
		if (result == IDCANCEL) {
	        delete[] max_attribute_values;
		    return;
	    } else if (result == IDTRYAGAIN) {
			goto STEP3;
	    }
		ASSERT(result == IDCONTINUE);
	}

	// fifth dialog and onward:  parameters of each hand
	Strings::Iterator i_name = player_names.Begin();
	for (unsigned i = 0; i < hand_cnt; ) {
		ASSERT(i_name != player_names.End());
		bool more_flag = (i+1 < hand_cnt);
		bool auto_flag = auto_hands.Contains(i);
		bool remote_flag = remote_hands.Contains(i);
		IpAddressType ip_address = ips[i];
		HandBox handbox(i+1, more_flag, *i_name, auto_flag, remote_flag, ip_address);
		result = handbox.Run(this);
		if (result == Dialog::RESULT_CANCEL) {
	        delete[] max_attribute_values;
		    return;
		} else if (result == Dialog::RESULT_BACK) {
			if (i == 0) {
				goto STEP4;
			} else {
				i--;
				i_name--;
			}
		} else {
		    ASSERT(result == Dialog::RESULT_OK);
		    *i_name = handbox.PlayerName();
		    auto_hands.AddRemove(i, handbox.IsAutomatic());
		    remote_hands.AddRemove(i, handbox.IsRemote());
		    ips[i] = IpAddressType(handbox);

			i++;
			i_name++;
		}
	}

	// can't cancel now - go ahead and set up the new game
	Cell::SetGrid(grid);
	Cell::SetTopology(wrap_flag, height, width);
	Tile::SetStatic(attribute_cnt, max_attribute_values);
	delete[] max_attribute_values;

	while (player_names.Count() > hand_cnt) {
		player_names.Unappend();
	}

    // TODO: this part should be done in Game constructor
    Strings hand_names;
    Strings unique = player_names.Unique();
	for (i_name = player_names.Begin(); i_name != player_names.End(); i_name++) {
		String hand_name = *i_name;
		if (player_names.Count(hand_name) > 1) {
		    hand_name = unique.InventUnique(*i_name, "'s ", " hand");
		    unique.Append(hand_name);
		}
		hand_names.Append(hand_name);
	}

	Game *p_new_game = new Game(hand_names, player_names, game_style, 
		                        tile_redundancy, hand_size, seconds_per_hand);
	ASSERT(p_new_game != NULL);
	SetGame(p_new_game);
}

void TopWindow::OfferSaveGame(void) {
#if 0
    YesNo box("UNSAVED");
	box.Run(this);
	// TODO
#endif
}

void TopWindow::Play(bool passFlag) {
	ASSERT(mpGame != NULL);
	ASSERT(mpMenuBar != NULL);
	ASSERT(mGameView.GetActive() == Tile::ID_NONE);
	ASSERT(!IsGameOver());
	ASSERT(!IsGamePaused());

    Move move = mGameView.GetMove(true);
    
	char const *reason;
	bool is_legal = mpGame->IsLegalMove(move, reason);
    if (move.IsPass() == passFlag && is_legal) {
        mpGame->FinishTurn(move);
		mpGame->StopClock();
        if (mpGame->IsOver()) {
            mpMenuBar->GameOver();                  
            mpGame->GoingOutBonus();
        } else {
			// the game isn't over, so proceed to the next hand
			Hand hand = Hand(*mpGame);
		    SavePlayerOptions(hand);

            mpGame->ActivateNextHand();
			hand = Hand(*mpGame);
		    LoadPlayerOptions(hand);
			IdType tile_size = mpMenuBar->TileSize();
			SetTileWidth(tile_size);
			if (!mpMenuBar->IsAutopause()) {
				mpGame->StartClock();
			}
        }

        mGameView.Reset();
		mGameView.ResetTargetCell();
               
    } else if (!is_legal) { // explain the issue
        RuleBox(reason);
		if (::str_eq(reason, "FIRST")) {
			mGameView.Reset();
		}

	}
}

void TopWindow::ReleaseActiveTile(Point const &rMouse) {
	ASSERT(mpGame != NULL);

    TileIdType id = mGameView.GetActive(); 
     
	// Determine where the active tile came from.
    bool from_hand = mGameView.IsInHand(id);
    bool from_swap = mGameView.IsInSwap(id);
	ASSERT(!(from_hand && from_swap)); 
	bool from_board = !(from_hand || from_swap);
	Cell from_cell;
    if (from_board) {
        from_cell = mGameView.LocateTile(id);
    }

	// Determine where the active tile was released to.
	bool to_hand = mGameView.IsInHandArea(rMouse);
	bool to_swap = (!to_hand) && mGameView.IsInSwapArea(rMouse); // overlap is possible
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
    Move move_so_far = mGameView.GetMove(true);
    char const *reason;
	bool legal = mpGame->IsLegalMove(move_so_far, reason);

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

		// perhaps the problem has to do with the target cell
		mGameView.ResetTargetCell();
	}

    if (mGameView.IsTargetUsed()) {
		mGameView.ResetTargetCell();
    }

	StopDragging();
}

void TopWindow::Repaint(void) {
    HWND this_window = Handle();
    PAINTSTRUCT paint_struct;    
    HDC context = Win::BeginPaint(this_window, &paint_struct);
    ASSERT(context != NULL);
    
    bool release_me = false;
    PCntType width = ClientAreaWidth();
    PCntType height = ClientAreaHeight();
    Canvas canvas(context, this_window, release_me, width, height);
    
	mGameView.Repaint(canvas);
    Win::EndPaint(this_window, &paint_struct);

	// restart the timer
	SetTimer(TIMEOUT_MSEC, ID_CLOCK_TIMER);

	if (mInitialNewGame) {
		ASSERT(mpGame == NULL);
		mInitialNewGame = false;
		OfferNewGame();
	}
}

void TopWindow::Resize(PCntType clientAreaWidth, PCntType clientAreaHeight) {
    PCntType old_height = ClientAreaHeight();
    PCntType old_width = ClientAreaWidth();
    SetClientArea(clientAreaWidth, clientAreaHeight);
    mGameView.Recenter(old_height, old_width);
    ForceRepaint();
}

void TopWindow::RuleBox(char const *messageText) {
	String message = messageText;
	String title = "Information";

	// expand shortcuts
	if (::str_eq(message, "COLUMNCOMPAT")) {
		message = "Tiles in a column (with no intervening empty cells) must all be mutually compatible.";
		title = "Column Compatibility Rule";
	} else if (::str_eq(message, "DIAGCOMPAT")) {
		message = "Tiles on a diagonal (with no intervening empty cells) must all be mutually compatible.";
		title = "Diagonal Compatibility Rule";
	} else if (::str_eq(message, "EMPTY")) {
		message = "That cell has already been used.";
		title = "Empty Cell Rule";
	} else if (::str_eq(message, "FIRST")) {
		message = "On the first turn, you must play as many tiles as possible.  Keep looking!";
		title = "First Turn Rule";
	} else if (::str_eq(message, "GAP")) {
		message = "You can't leave any empty cells between the tiles you play.";
		title = "Gap Rule";
	} else if (::str_eq(message, "NEIGHBOR")) {
		message = "Each cell you use must be a neighbor of a used cell.";
		title = "Neighbor Rule";
	} else if (::str_eq(message, "ROWCOLUMN")) {
		String dirs;
		switch (Cell::Grid()) {
		case GRID_TRIANGLE:
			dirs = "row or diagonal";
			break;
		case GRID_4WAY:
			dirs = "row or column";
			break;
		case GRID_HEX:
			dirs = "column or diagonal";
			break;
		case GRID_8WAY:
		    dirs = "row, column, or diagonal";
			break;
		default:
			FAIL();
		}
		message = String("The cells you use must all lie in a single ") + dirs + String(".");
		title = "Row/Column Rule";
	} else if (::str_eq(message, "ROWCOMPAT")) {
		message = "Tiles in a row (with no intervening empty cells) must all be mutually compatible.";
		title = "Row Compatibility Rule";
	} else if (::str_eq(message, "RULES")) {
		message = "The rules of Gold Tile are available online at http://code.google.com/p/gold-tiles/wiki/Playing";
		title = "Rules";
	} else if (::str_eq(message, "START")) {
		message = "Your first tile must use the start cell. To change this tile, you must take back ALL your tiles.";
		title = "Start Rule";
	} else if (::str_eq(message, "STARTSIMPLE")) {
		message = "Your first tile must use the start cell.";
		title = "Start Rule";
	} else if (::str_eq(message, "STOCK")) {
		message = "You can't swap more tiles than the number remaining in the stock bag.";
		title = "Stock Rule";
	} else if (::str_eq(message, "SWAP")) {
		message = "You can play tiles or swap them, but you can't do both in the same turn.";
		title = "Swap Rule";
	}

	title += " - Gold Tile";
	ErrorBox(message, title);
}

void TopWindow::SavePlayerOptions(Hand const &rHand) const {
	String player_name = rHand.PlayerName();
	Player &r_player = Player::rLookup(player_name);
	mpMenuBar->SavePlayerOptions(r_player);
	Point start_cell_position = mGameView.StartCellPosition();
	r_player.SetStartCellPosition(start_cell_position);
}

void TopWindow::SetGame(Game *pGame) {
	ASSERT(mpMenuBar != NULL);

	// TODO: free old Game object?
	mpGame = pGame;

	mGameView.SetGame(pGame);
	mpMenuBar->NewGame();

	if (pGame != NULL) {
	    Hands hands = Hands(*pGame);
		Hands::ConstIterator i_hand;
		for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
			SavePlayerOptions(*i_hand);
		}
	}

	if (mpGame != NULL && !mpMenuBar->IsAutopause()) {
		mpGame->StartClock();
	}

    UpdateMenuBar();
	ForceRepaint();
}

void TopWindow::SetTileWidth(IdType command) {
	ASSERT(mpMenuBar != NULL);

	mGameView.SetTileWidth(command);
	mpMenuBar->SetTileSize(command);
}

void TopWindow::StopDragging(void) {
	ASSERT(mGameView.IsDragging());

	mGameView.Deactivate();
   	SetCursorSelect();
	Win::ReleaseCapture();
	mDragBoardFlag = false;

	ASSERT(!mGameView.IsDragging());
	ASSERT(!IsMouseCaptured());
}

void TopWindow::TogglePause(void) {
	if (mpGame != NULL && !IsGameOver()) {
		if (IsGamePaused()) {
		    mpGame->StartClock();
		} else {
			mpGame->StopClock();
		}
	}
}

void TopWindow::UpdateMenuBar(void) {
	mpMenuBar->Update();
    Window::UpdateMenuBar();
}

int TopWindow::WarnBox(char const *messageText) {
	String message = messageText;
	String title = "Information";

	// expand shortcuts
	if (::str_eq(message, "FEWTILES")) {
		message = "You haven't created enough tiles to fill all the hands.";
		title = "Too Few Tiles";
	}

	title += " - Gold Tile";
	int result = Window::WarnBox(message, title);

	return result;
}


// inquiry methods

bool TopWindow::IsDraggingBoard(void) const {
	return mDragBoardFlag;
}

bool TopWindow::IsGameOver(void) const {
	bool result = false;
	if (mpGame != NULL) {
		result = mpGame->IsOver();
	}

	return result;
}

bool TopWindow::IsGamePaused(void) const {
	bool result = false;
	if (mpGame != NULL && !mpGame->IsOver()) {
		result = mpGame->IsPaused();
	}

	return result;
}