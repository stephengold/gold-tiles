// File:     gamewindow.cpp
// Location: src/gui
// Purpose:  implement GameWindow class
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

#include "gui/canvas.hpp"
#include "gui/gamewindow.hpp"
#include "gui/menubar.hpp"
#include "gui/user.hpp"
#include "handopts.hpp"
#include "network.hpp"
#include "strings.hpp"

#ifdef _QT
# include "ui_gamewindow.h"

GameWindow::GameWindow(Game* pGame):
    mMouseLast(0, 0),
    mGameView(*pGame)
{
    mpUi = new Ui::GameWindow;
    mpUi->setupUi(this);

    mpGame = pGame;
    mpMenuBar = new MenuBar(mGameView);
    setMenuBar(mpMenuBar);
}
#endif  // defined(_QT)

#ifdef  _WINDOWS
# include "gui/attrbox.hpp"
# include "gui/handbox.hpp"
# include "gui/hintbox.hpp"
# include "gui/parmbox1.hpp"
# include "gui/parmbox2.hpp"
# include "gui/parmbox3.hpp"
# include "gui/resource.hpp"
# include "gui/tilebox.hpp"
# include "gui/win_types.hpp"
# include "gui/windowclass.hpp"
#endif  // defined(_WINDOWS)

// static data of the class

WindowClass* GameWindow::mspClass = NULL;


// static callback functions

#ifdef  _WINDOWS
// message handler (callback) for game window
static LRESULT CALLBACK message_handler(
    HWND windowHandle,
    MessageType message,
    WPARAM wParam,
    LPARAM lParam)
{
    ASSERT(windowHandle != NULL);
    GameWindow* const window = (GameWindow*)Window::Lookup(windowHandle);

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
static void CALLBACK think(void* pArgument) {
    GameWindow* const window = (GameWindow*)pArgument;

    window->Think();
}
#endif  // defined(_WINDOWS)

static void yield(void* pArgument, bool& rCancel) {
    GameWindow* const window = (GameWindow*)pArgument;

    window->Yields();
    rCancel = window->IsThinkCanceled();
}


// lifecycle

#ifdef _WINDOWS
GameWindow::GameWindow(HINSTANCE applicationInstance, Game* pGame):
mGameView(*pGame),
    mMouseLast(0, 0)
{
    ASSERT(HWND(*this) == 0);
    ASSERT(applicationInstance != NULL);

    TextType class_name = "GAMEWINDOW";
    TextType menubar_name = "MENUBAR";
    if (mspClass == NULL) {
        // for first instance:  create a Microsoft Windows window class
        mspClass = new WindowClass(applicationInstance, &message_handler, class_name, menubar_name);
        ASSERT(mspClass != NULL);
        mspClass->RegisterClass();
    }

    mDragBoardFlag = false;
    mpGame = pGame;
    mHaveInvitation = false;
#ifdef _CLIENT
    mInitialNewGame = (pGame == NULL);
#else  // !defined(_CLIENT)
    mInitialNewGame = false;
#endif  // !defined(_CLIENT)
    mpMenuBar = NULL;

    // Set up Think fiber for background processing.
    UseFibers();
    mThinkMode = THINK_IDLE;
    mThinkFiber = AddFiber(think);

    // Set up keyboard shortcuts.
    SetAcceleratorTable("HOTKEYS");

    // determine initial window size:  centered and covering 64% of desktop
    Rect const desktop_bounds = DesktopBounds();
    PixelCntType const height = PixelCntType(0.8*double(desktop_bounds.Height()));
    PixelCntType const width = PixelCntType(0.8*double(desktop_bounds.Width()));
    LogicalXType const x = width/8;
    LogicalYType const y = height/8;
    Rect const rect(y, x, width, height);

    // Create a Microsoft Windows window.
    String const class_string(class_name);
    Window* const p_parent = NULL;
    Create(class_string, rect, p_parent, applicationInstance);

    // Wait for message_handler() to receive a message with this handle.
}
#endif  // defined(_WINDOWS)

GameWindow::~GameWindow(void) {
    delete mpMenuBar;
}

#ifdef _WINDOWS
void GameWindow::Initialize(CREATESTRUCT const& rCreateStruct) {
    // Initialization which takes place after the Microsoft Windows window
    // has received its WM_CREATE message.

    ASSERT(mpMenuBar == NULL);

    Window::Initialize(rCreateStruct);

    mpMenuBar = new MenuBar(rCreateStruct, mGameView);
    ASSERT(mpMenuBar != NULL);

    Network::SetWindow(this);

    mGameView.SetWindow(this, mpMenuBar);
    SetCursorSelect();

    Partial::SetYield(&yield, (void*)this);
    Socket::SetYield(&yield, (void*)this);

    SetBoardTileSize(GameView::TILE_SIZE_DEFAULT);
    SetHandTileSize(GameView::TILE_SIZE_DEFAULT);
    if (HasGame()) {
        Hands const hands = Hands(*mpGame);
        Hands::ConstIterator i_hand;
        for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
            if (i_hand->IsLocalUser()) {
                SaveUserOptions(*i_hand);
            }
        }
        ChangeHand("");
    }

    SetTimer(TIMEOUT_MSEC, ID_CLOCK_TIMER);
    UpdateMenuBar();
}
#endif  // defined(_WINDOWS)


// misc methods

void GameWindow::ChangeHand(String const& rOldPlayerName) {
    ASSERT(HasGame());
    ASSERT(IsGamePaused() || IsGameOver());
    ASSERT(mpMenuBar != NULL);
    ASSERT(mThinkMode == THINK_IDLE || mThinkMode == THINK_CANCEL);

    Hand const playable_hand = *mpGame;
    if (playable_hand.IsLocalUser()) {
        LoadUserOptions(playable_hand);
    }
    Fraction const skip_probability = playable_hand.SkipProbability();
    mGameView.Reset(skip_probability);
    mGameView.ResetTargetCell();

    if (!IsGameOver()) {
        Indices tiles;
        if (mpGame->CanUndo()) {
            tiles = mpGame->UndoTiles();
        }
        mGameView.SetWarmTiles(tiles);
        if (!playable_hand.IsLocalUser()) {
            mpGame->StartClock();
        } else if (rOldPlayerName == playable_hand.PlayerName())	{
            mpGame->StartClock();
        } else if (!mpMenuBar->IsAutopause()) {
            mpGame->StartClock();
        }

        if (!mpGame->CanRedo()) {
            if (mpGame->IsDisabled()) {
                mThinkMode = THINK_DISABLE;
            } else if (playable_hand.IsAutomatic()) {
                // Launch autoplay.
                mThinkMode = THINK_AUTOPLAY;
            } else if (playable_hand.IsRemote()) {
                // Expect to get a remote move over the network.
                mThinkMode = THINK_REMOTE;
            }
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

int GameWindow::GameWarnBox(TextType messageText) {
    String message(messageText);
    String title = "Warning";

    // expand shortcuts
    if (::str_eq(message, "FEWTILES")) {
        message = "You haven't created enough tiles to fill all the hands.";
        title = "Too Few Tiles";
    }

    title += " - Gold Tile";
    int const result = WarnBox(message, title);

    return result;
}

void GameWindow::HandleButtonDown(Point const& rMouse) {
    ASSERT(HasGame());
    ASSERT(!IsGamePaused());
    ASSERT(!mGameView.IsDragging());
    mMouseLast = rMouse;

    Tile::IdType const id = mGameView.GetTileId(rMouse);
    if (id != Tile::ID_NONE) {
        if (!IsMouseCaptured()) {
            // Capture mouse to drag the tile
            CaptureMouse();
            Point const point = mGameView.TileCenter(id, rMouse);
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

void GameWindow::HandleButtonUp(Point const& rMouse) {
    ASSERT(HasGame());

    HandleMouseMove(rMouse);

    if (mDragBoardFlag) {
        if (mDragBoardPixelCnt < DRAG_THRESHOLD 
            && mGameView.IsLocalUsersTurn()) {
                /*
                Drags shorter than six pixels 
                are treated as normal mouse-clicks 
                which change or deactivate the target cell.
                */
                mGameView.ToggleTargetCell(rMouse);
        }
        StopDragging();
    } else {
        ASSERT(mGameView.GetActive() != Tile::ID_NONE);        
        ReleaseActiveTile(rMouse);
    }
}

// SERVER
void GameWindow::HandleInvitation(Socket& rSocket) {
    Address const address = rSocket.Peer();
    String const question = "Consider invitation from " + String(address) + "?";
    int const consider = QuestionBox(question, "Consider Invitation - Gold Tile");
    if (consider != IDYES) {
        return;
    }

    GameOpt game_opt;
    bool success = game_opt.GetFromClient(rSocket);
    if (!success) {
        return;
    }
    unsigned const hand_cnt = game_opt.HandsDealt();

    HandOpts hand_opts;
    success = hand_opts.GetFromClient(rSocket, hand_cnt);
    if (!success) {
        return;
    }

    Game* p_new_game = Network::ConsiderInvitation(rSocket, game_opt, hand_opts);
    if (p_new_game != NULL) {
        success = p_new_game->Initialize();
        if (success) {
            SetGame(p_new_game);
        } else {
            delete p_new_game;
        }
    }
}

#ifdef _WINDOWS
void GameWindow::HandleMenuCommand(IdType command) {
    switch (command) {
        // Arrow keys
    case IDM_UP:
        mGameView.MoveTarget(+1, 0);
        break;
    case IDM_DOWN:
        mGameView.MoveTarget(-1, 0);
        break;
    case IDM_LEFT:
        mGameView.MoveTarget(0, -1);
        break;
    case IDM_RIGHT:
        mGameView.MoveTarget(0, +1);
        break;

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

    // Board Size menu options
    case IDM_BS_ZOOM_OUT:
        IncreaseBoardTileSize(-1);
        break;
    case IDM_BS1:
        SetBoardTileSize(1);
        break;
    case IDM_BS2:
        SetBoardTileSize(2);
        break;
    case IDM_BS3:
        SetBoardTileSize(3);
        break;
    case IDM_BS4:
        SetBoardTileSize(4);
        break;
    case IDM_BS5:
        SetBoardTileSize(5);
        break;
    case IDM_BS6:
        SetBoardTileSize(6);
        break;
    case IDM_BS7:
        SetBoardTileSize(7);
        break;
    case IDM_BS_ZOOM_IN:
        IncreaseBoardTileSize(+1);
        break;

    // Tile Size menu options
    case IDM_TS1:
        SetHandTileSize(1);
        break;
    case IDM_TS2:
        SetHandTileSize(2);
        break;
    case IDM_TS3:
        SetHandTileSize(3);
        break;
    case IDM_TS4:
        SetHandTileSize(4);
        break;
    case IDM_TS5:
        SetHandTileSize(5);
        break;
    case IDM_TS6:
        SetHandTileSize(6);
        break;
    case IDM_TS7:
        SetHandTileSize(7);
        break;

        // View menu options
    case IDM_RECENTER:
        Recenter();
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
        int const result = box.Run(this);
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

        // Thinking menu options
    case IDM_CANCEL:
        if (mThinkMode != THINK_AUTOPLAY && mThinkMode != THINK_DISABLE) {
            ASSERT(mThinkMode == THINK_SUGGEST || mThinkMode == THINK_REMOTE);
            mThinkMode = THINK_CANCEL;
        }
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

    if (command != IDM_EXIT) {
        UpdateMenuBar();
        ForceRepaint();
    }
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
        LPCREATESTRUCT const p_create_struct = LPCREATESTRUCT(lParam);
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
        PixelCntType const width = LOWORD(lParam);
        PixelCntType const height = HIWORD(lParam);
        Area const new_client_area(width, height);
        Resize(new_client_area);
        break;
                  }

    case WM_TIMER: { // timer popped
        int const timer_id = int(wParam);
        if (timer_id == ID_CLOCK_TIMER) {
            if (mpMenuBar->AreClocksVisible() && !IsGamePaused() && !IsGameOver()) {
                // don't update menus because they would flicker
                ForceRepaint();  // to update the playable hand's clock display
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

#ifdef _SERVER
    // Check for a new invitation from a client.
    PollForInvitation();
#endif // defined(_SERVER)

    return result;
}
#endif // defined(_WINDOWS)

void GameWindow::HandleMouseMove(Point const& rMouse) {
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

void GameWindow::IncreaseBoardTileSize(int delta) {
    ASSERT(mpMenuBar != NULL);

    int size = mpMenuBar->BoardTileSize();
    ASSERT(size >= GameView::TILE_SIZE_MIN);
    ASSERT(size <= GameView::TILE_SIZE_MAX);

    size += delta;
    if (size < GameView::TILE_SIZE_MIN) {
        size = GameView::TILE_SIZE_MIN;
    } else if (size > GameView::TILE_SIZE_MAX) {
        size = GameView::TILE_SIZE_MAX;
    }

    SetBoardTileSize(size);
}

void GameWindow::InfoBox(TextType messageText) {
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

void GameWindow::LoadUserOptions(Hand const& rHand) {
    String const user_name = rHand.PlayerName();
    User const& r_user = User::rLookup(user_name);

    mpMenuBar->LoadUserOptions(r_user);
    mGameView.LoadUserOptions(r_user);
}

TextType GameWindow::Name(void) const {
#ifdef _SERVER
# ifdef _CLIENT
#  ifdef _DEBUG
    return "DEBUG BUILD, CLIENT/SERVER CONFIGURATION";
#  else // !defined(_DEBUG)
    return "Gold Tile - a game by Stephen Gold";
#  endif // !defined(_DEBUG)
# else // !defined _CLIENT
    return "SERVER-ONLY CONFIGURATION";
# endif // !defined _CLIENT
#else // !defined _SERVER
    return "CLIENT-ONLY CONFIGURATION";
#endif // !defined _SERVER
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

    /*
    first dialog:  prompt for game style, random seed, 
       time limit, and standard/custom rules
    */
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
        // Initialize a new hand_option for each new hand.
        GameStyleType const game_style = GameStyleType(game_options);
        Strings player_names = hand_options.AllPlayerNames();
        for (unsigned i_hand = hand_options.Count(); i_hand < hand_cnt; i_hand++) {
            HandOpt const options = HandOpt(game_style, player_names);
            String const new_name = options.PlayerName();
            player_names.Append(new_name);
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

    // Check the sanity of the parameters so far.
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
            i_hand++;
        }
    }

    // can't cancel now - set up the new game
    hand_options.Truncate(hand_cnt);

    SetCursorBusy(); // Constructing tiles may take a noticeable amount of time.
    Socket const no_client;
    Game* const p_new_game = Game::New(game_options, hand_options, no_client);
    SetCursorSelect();
    ASSERT(p_new_game != NULL);
    bool const success = p_new_game->Initialize();
    if (success) {
        SetGame(p_new_game);
    } else {
        delete p_new_game;
    }
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

    // Check whether the playable hand has run out of time.
    if (!mpGame->IsOutOfTime()) {
        move = mGameView.GetMove(true);
    } else {
        // If it has, force a resignation.
        move.MakeResign(mpGame->ActiveTiles());
        passFlag = false;
    }

    // Check whether the move is a legal one.
    UmType reason;
    bool const is_legal = mpGame->IsLegalMove(move, reason);
    if (is_legal && move.IsPass() == passFlag) {
        mpGame->FinishTurn(move);

        if (!mpGame->IsOver()) {
            // The game isn't over yet, so proceed to the next hand.
            String const old_player_name = SaveHandOptions();
            mpGame->ActivateNextHand();
            ChangeHand(old_player_name);

        } else {
            GameOver();
        }

    } else if (!is_legal) { // explain the issue
        RuleBox(reason);
        if (reason == UM_FIRST) {
            mGameView.Reset();
        }
    }
}

void GameWindow::PollForInvitation(void) {
    if (mHaveInvitation || !Network::IsServerStarted()) {
        return;
    }

    // Check for a game invitation from a client.
    if (mpGame == NULL) {
        String const description = String("an invitation on ") 
                                 + Network::DescribeListenPort();
        WaitingFor(description);
    }
    Socket socket = Network::CheckForConnection();
    if (!socket.IsValid()) {
        return;
    }

    // Got an invitation.  Deal with it!
    mHaveInvitation = true;
    DoneWaiting();
    HandleInvitation(socket);
    mHaveInvitation = false;
}

void GameWindow::Recenter(void) {
    mGameView.Recenter();
    ForceRepaint();
}

void GameWindow::RedoTurn(void) {
    ASSERT(HasGame());
    ASSERT(mpMenuBar != NULL);
    ASSERT(!IsGamePaused());
    ASSERT(mpGame->CanRedo());

    if (!IsGameOver()) {
        mpGame->StopClock();
    }
    String const old_player_name = SaveHandOptions();
    mpGame->Redo();
    ChangeHand(old_player_name);
}

void GameWindow::ReleaseActiveTile(Point const& rMouse) {
    ASSERT(HasGame());

    Tile::IdType const id = mGameView.GetActive(); 

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
        if (Cell::DoesBoardWrap()) {
            to_cell.Wrap();
        }
    }

    if (from_hand && to_hand ||
        from_swap && to_swap ||
        from_board && to_board && from_cell == to_cell)
    {
        /* 
        Trivial drags (drags which don't actually move the tile)
        are treated as normal mouse-clicks which
        activate/deactivate the tile or play it to the
        target cell.
        */
        if (mMouseUpCnt == 1) {
            StopDragging(); // deactivates the tile
            return;
        } else {
            ASSERT(mMouseUpCnt == 0);
            if (mGameView.IsTargetSet() && !from_board) {
                to_board = true;
                to_cell = mGameView.TargetCell();
            } else {
                mMouseUpCnt = 1;
                mGameView.ResetTargetCell();
                return;
            }
        }
    }

    if (to_board && (!to_cell.IsValid() || mGameView.GetCellTile(to_cell) != Tile::ID_NONE)) {
        // cell conflict - can't construct a move in the usual way
        RuleBox(UM_EMPTY);
        StopDragging();
        return;
    }

    // move the active tile back to the playable hand
    if (from_board) {
        mGameView.BoardToHand();
    } else if (from_swap) {
        mGameView.SwapToHand();
    }

    // move tile from the playable hand to its destination
    if (to_board) {
        mGameView.HandToCell(to_cell);
    } else if (to_swap) {
        ASSERT(!from_swap);
        mGameView.HandToSwap();
    }

    // Check whether the new partial move is legal.
    Move const move_so_far = mGameView.GetMove(true);
    UmType reason;
    bool const legal = mpGame->IsLegalMove(move_so_far, reason);

    if (!legal && (to_swap || reason != UM_FIRST)) {  
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

        // Explain to the user why it was illegal.
        if (reason == UM_START && !from_board) {
            reason = UM_STARTSIMPLE;
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
        // The game isn't over yet, so proceed to the next hand.
        String const old_player_name = SaveHandOptions();
        mpGame->ActivateNextHand();
        ChangeHand(old_player_name);

    } else {
        GameOver();
    }
}

void GameWindow::Resize(Area const& rClientArea) {
    SetClientArea(rClientArea);
    Recenter();
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
    ChangeHand(old_player_name);
}

void GameWindow::RuleBox(UmType reason) {
    // expand reason shorthand
    String title;
    String const message = Board::ReasonMessage(reason, title);

    title += " - Gold Tile";
    ErrorBox(message, title);
}

String GameWindow::SaveHandOptions(void) const {
    Hand const old_hand = Hand(*mpGame);
    if (old_hand.IsLocalUser()) {
        SaveUserOptions(old_hand);
    }
    String const result = old_hand.PlayerName();

    return result;
}

void GameWindow::SaveUserOptions(Hand const& rHand) const {
    ASSERT(mpMenuBar != NULL);

    String const user_name = rHand.PlayerName();
    User& r_user = User::rLookup(user_name);

    mpMenuBar->SaveUserOptions(r_user);
    mGameView.SaveUserOptions(r_user);
}

void GameWindow::SetGame(Game* pGame) {
    ASSERT(mpMenuBar != NULL);

    // Cancel any operation which is using the Think fiber.
    while (mThinkMode != THINK_IDLE) {
        ASSERT(mThinkFiber != NULL);
        mThinkMode = THINK_CANCEL;
        Win::SwitchToFiber(mThinkFiber);
    }

    GameStyleType old_style = GAME_STYLE_NONE;
    if (HasGame()) {
        old_style = mpGame->Style();
        //delete mpGame;
    }

    mpGame = pGame;

    DoneWaiting();

    mGameView.SetGame(mpGame);
    mpMenuBar->NewGame(old_style);
    SetBoardTileSize(GameView::TILE_SIZE_DEFAULT);
    SetHandTileSize(GameView::TILE_SIZE_DEFAULT);

    if (HasGame()) {
        Hands const hands = Hands(*mpGame);
        Hands::ConstIterator i_hand;
        for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
            SaveUserOptions(*i_hand);
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

void GameWindow::SetBoardTileSize(unsigned size) {
    ASSERT(mpMenuBar != NULL);
    ASSERT(size >= GameView::TILE_SIZE_MIN);
    ASSERT(size <= GameView::TILE_SIZE_MAX);

    mpMenuBar->SetBoardTileSize(size);
    mGameView.SetBoardTileSize(size);
}

void GameWindow::SetHandTileSize(unsigned size) {
    ASSERT(mpMenuBar != NULL);
    ASSERT(size >= GameView::TILE_SIZE_MIN);
    ASSERT(size <= GameView::TILE_SIZE_MAX);

    mpMenuBar->SetHandTileSize(size);
    mGameView.SetHandTileSize(size);
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
        while (mThinkMode == THINK_IDLE || mThinkMode == THINK_CANCEL) {
            if (mThinkMode == THINK_CANCEL) {
                mThinkMode = THINK_IDLE;
            }
            Yields();
        }

        ASSERT(HasGame());
        UpdateMenuBar(); // to indicate that the fiber is busy

        if (mThinkMode == THINK_AUTOPLAY) {
            mGameView.Suggest();
            bool const is_pass = mGameView.IsPass();

            // Commit the move.
            mThinkMode = THINK_IDLE;
            Play(is_pass);

        } else if (mThinkMode == THINK_DISABLE) {
            ASSERT(!mpGame->AmClient());
            Hand playable_hand = *mpGame;
            Move move;
            move.MakeResign(Tiles(playable_hand));

            // Commit the move.
            mpGame->FinishTurn(move);
            mThinkMode = THINK_IDLE;
            if (!mpGame->IsOver()) {
                // The game isn't over yet, so proceed to the next hand.
                String const old_player_name = SaveHandOptions();
                mpGame->ActivateNextHand();
                ChangeHand(old_player_name);
            } else {
                GameOver();
            }

        } else if (mThinkMode == THINK_REMOTE) {
            Hand playable_hand = *mpGame;
            Move move;
            bool const success = playable_hand.GetRemoteMove(move);
            if (!success) {
                move.MakeResign(Tiles(playable_hand));
                if (mpGame->AmClient()) {
                    Address const address = Address(playable_hand);
                    mpGame->DisableServers(address);
                } else {
                    mpGame->Disable();
                }
            }

            // Commit the move.
            mpGame->FinishTurn(move);
            mThinkMode = THINK_IDLE;
            if (!mpGame->IsOver()) {
                // The game isn't over yet, so proceed to the next hand.
                String const old_player_name = SaveHandOptions();
                mpGame->ActivateNextHand();
                ChangeHand(old_player_name);
            } else {
                GameOver();
            }

        } else {
            ASSERT(mThinkMode == THINK_SUGGEST);
            mGameView.Suggest();
            mGameView.ResetTargetCell();

            mThinkMode = THINK_IDLE;
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
    ChangeHand(old_player_name);
}

void GameWindow::UpdateMenuBar(void) {
    mpMenuBar->Update(mThinkMode);

    // reveal any menubar changes
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

bool GameWindow::IsThinkCanceled(void) const {
    bool const result = (mThinkMode == THINK_CANCEL);

    return result;
}
