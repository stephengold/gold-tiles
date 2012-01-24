// File:    topwindow.cpp
// Purpose: TopWindow class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <windows.h>
//#include "about.h"
#include "game.hpp"
#include "gridref.hpp"
#include "gui/canvas.hpp"
#include "gui/resource.hpp"
#include "gui/topwindow.hpp"
#include "gui/windowclass.hpp"
#include "locus.hpp"
#include "project.hpp"

#define WIDTH_SMALL_TILE 21
#define WIDTH_MEDIUM_TILE 41
#define WIDTH_LARGE_TILE 61

// static member of TopWindow

WindowClass *TopWindow::_class = NULL;

// message handler (callback) for top window
static TopWindow *newlyCreatedTopWindow = NULL;
static LRESULT CALLBACK topMessageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
    TopWindow *window;
	if (message == WM_CREATE && newlyCreatedTopWindow != NULL) {
		window = newlyCreatedTopWindow;
		newlyCreatedTopWindow = NULL;
		window->setHandle(windowHandle);
	} else {
       window = (TopWindow *)Window::lookup(windowHandle);
	}

	LRESULT result;
	if (window == NULL) { // unknown window handle
		// invoke default message handler
		result = ::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	assert(window->getHandle() == windowHandle);
        result = window->handleMessage(message, wParam, lParam);
	}
	return result;
}

// non-static members of TopWindow

TopWindow::TopWindow(HINSTANCE applicationInstance):
	_application(applicationInstance),
    _fileName("Game1"),
	_pauseFlag(false),
    _playMenu(NULL),
	_showClocksFlag(false),
	_showGridFlag(true),
	_showScoresFlag(true),
	_showTilesFlag(false),
	_tileWidth(WIDTH_LARGE_TILE),
    _viewMenu(NULL)
{
	assert(getHandle() == 0);

	char const *className = getClassName();
    if (_class == NULL) {
		// constructing first instance:  create a Microsoft window class
		WNDPROC messageHandler = getMessageHandler();
		_class = new WindowClass(applicationInstance, messageHandler, className);
		_class->registerClass();
	}
	assert(_class != NULL);

	// Make this available to message handler prior to WM_CREATE.
    assert(newlyCreatedTopWindow == NULL);
	newlyCreatedTopWindow = this;

	// create Microsoft Windows window
	char const *windowName = getName();
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;
	int width = CW_USEDEFAULT;
	int height = CW_USEDEFAULT;
	HWND parent = NULL;
	HMENU menu = NULL;
	LPVOID parameters = NULL;
    HWND handle = ::CreateWindow(className, windowName, windowStyle, x, y, width, height,
	                         parent, menu, applicationInstance, parameters);
    assert(handle != 0);
    setHandle(handle);

    _originHasBeenCentered = false;
    _originX = 0;
    _originY = 0;

    _dragBoardFlag = false;
    _mouseLastX = 0;
    _mouseLastY = 0;
}

void TopWindow::buttonDown(const POINTS &point) {
    // Capture the mouse
    HWND thisWindow = getHandle();
	::SetCapture(thisWindow);

    _mouseLastX = point.x;
    _mouseLastY = point.y;
    _dragBoardFlag = true;
}

void TopWindow::buttonUp(const POINTS &point) {
    HWND thisWindow = getHandle();
	HWND captor = ::GetCapture();
    if (captor != thisWindow) {
		return;
    } else {
        ::ReleaseCapture();
    }
		
    int dragX = point.x - _mouseLastX;
    int dragY = point.y - _mouseLastY;
    if (_dragBoardFlag) {
        _originX += dragX;
        _originY += dragY;
        forceRepaint();
    }
    
    _dragBoardFlag = false;
    _mouseLastX = point.x;
    _mouseLastY = point.y;
}

void TopWindow::createdWindow(CREATESTRUCT const *createStruct) {
    HMENU topMenu = createStruct->hMenu;
    HDC privateDC = initialize(createStruct);
    HWND windowHandle = getHandle();

    _playMenu = new PlayMenu(topMenu, IDM_PLAY);
	assert(_playMenu != NULL);
	_viewMenu = new ViewMenu(topMenu, IDM_VIEW);
	assert(_viewMenu != NULL);
	updateMenus();
}

void TopWindow::dragMouse(POINTS const &point) {
    HWND thisWindow = getHandle();
	HWND captor = ::GetCapture();
    if (captor != thisWindow) {
		return;
    }

    int dragX = point.x - _mouseLastX;
    int dragY = point.y - _mouseLastY;
    if (_dragBoardFlag) {
        _originX += dragX;
        _originY += dragY;
        forceRepaint();
    }

    _mouseLastX = point.x;
    _mouseLastY = point.y;
}

void TopWindow::drawCell(Canvas &canvas, GridRef const &square) {
    int row = square.getRow();
    int column = square.getColumn();
    int ulcX = getCellX(column);
    int ulcY = getCellY(row);

    Board board = game->getBoard();
    Locus done;
    bool connected = board.connectsToOrigin(square, done);

    COLORREF cellColor = DARK_GREEN_COLOR;
    COLORREF gridColor = cellColor;
    if (!connected) {
        cellColor = BLACK_COLOR;
    }
    if (_showGridFlag) {
        gridColor = GREEN_COLOR;
    }

    unsigned cellWidth = getCellWidth();
    canvas.drawCell(ulcY, ulcX, cellWidth, cellColor, gridColor);
    
    Player activePlayer = game->getActivePlayer();
    Tiles hand = activePlayer.getHand();
    set<Tile>::iterator it = hand.begin();
    Tile tile = *it;
    drawTile(canvas, ulcY+1, ulcX+1, tile);
}

void TopWindow::drawTile(Canvas &canvas, int top, int left, Tile const &tile) {
    COLORREF baseColor = LIGHT_GRAY_COLOR;
    COLORREF glyphColor = BLACK_COLOR;

#if 1
	ACount numberOfColorAttributes = 1;

	AIndex colorInd;
	if (numberOfColorAttributes == 1) {
        colorInd = tile.getAttribute(0);
	} else {
		assert(numberOfColorAttributes == 0);
        colorInd = 0;
	}
	assert(colorInd < 10);
	static COLORREF Colors[10] = {
		BLACK_COLOR, RED_COLOR,       DARK_BLUE_COLOR, DARK_GREEN_COLOR, PURPLE_COLOR,
		BROWN_COLOR, DARK_GRAY_COLOR, PINK_COLOR,      LIGHT_BLUE_COLOR, LIGHT_GREEN_COLOR
	};
	glyphColor = Colors[colorInd];

    ACount numberOfGlyphAttributes = Tile::getNumAttributes() - numberOfColorAttributes;
    AValue glyphs[4];
    for (AIndex gi = 0; gi < 4; gi++) {
		AIndex ind = gi + numberOfColorAttributes;
         if (gi < numberOfGlyphAttributes) {
             glyphs[gi] = tile.getAttribute(ind);
         } else {
             glyphs[gi] = 0;
         }
    }
#else
    AIndex colorInd = 0;
    ACount numberOfGlyphAttributes = Tile::getNumAttributes();
    AValue glyphs[4];
    for (AIndex ind = 0; ind < 4; ind++) {
         if (ind < numberOfGlyphAttributes) {
             glyphs[ind] = tile.getAttribute(ind);
         } else {
             glyphs[ind] = 0;
         }
    }
#endif

    unsigned width = getCellWidth() - 2;
    canvas.drawTile(top, left, width, baseColor,
          glyphColor, numberOfGlyphAttributes, glyphs);
}

unsigned TopWindow::getCellWidth(void) const {
    unsigned result = _tileWidth;
    if (_showGridFlag) {
        result = _tileWidth + 1;
    }

    return result;
}

int TopWindow::getCellX(int column) const {
    unsigned gridUnit = getGridUnit();
    int result = _originX + gridUnit*column;

    return result;
}

int TopWindow::getCellY(int row) const {
    unsigned gridUnit = getGridUnit();
    int result = _originY - gridUnit*row;

    return result;
}

char const *TopWindow::getClassName(void) const {
	return "GOLDTILETOP";
}

unsigned TopWindow::getGridUnit(void) const {
    unsigned result = _tileWidth - 1;
    if (_showGridFlag) {
        result = _tileWidth;
    }

    return result;
}

WNDPROC TopWindow::getMessageHandler(void) const {
    return &topMessageHandler;
}

char const *TopWindow::getName(void) const {
	return "Gold Tile - a game by Stephen Gold";
}

LRESULT TopWindow::handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
        case WM_COMMAND: { // menu command
			int command = LOWORD(wParam);
            menuCommand(command);
            break;
		}

        case WM_CREATE: { // initialize window
			CREATESTRUCT *createStruct = (CREATESTRUCT *)lParam;
            createdWindow(createStruct);
            break;
		}

        case WM_DESTROY: { // destroy window
			destroy();
            break;
	   }

		case WM_LBUTTONDOWN: { // left-click
			POINTS xy = MAKEPOINTS(lParam);
            buttonDown(xy);
            break;
		}

        case WM_LBUTTONUP: {
			POINTS xy = MAKEPOINTS(lParam);
            buttonUp(xy);
            break;
		}

        case WM_MOUSEMOVE:
            if (wParam & MK_LBUTTON) { // dragging with left button
				POINTS xy = MAKEPOINTS(lParam);
                dragMouse(xy);
			}
            break;

		case WM_PAINT: // repaint
            repaint();
            break;
            
        case WM_SIZE: { // resize
            unsigned long clientAreaWidth = LOWORD(lParam);
            unsigned long clientAreaHeight = HIWORD(lParam);
            resize(clientAreaWidth, clientAreaHeight);
            break;
        }

		default:  // invoke base class message handler
			Window *base = (Window *)this;
			result = base->handleMessage(message, wParam, lParam);
			break;
    }

    return result;
}

void TopWindow::menuCommand(int command) {
	assert(_playMenu != NULL);
	assert(_viewMenu != NULL);

	HWND window = getHandle();

    switch (command) {
  	    // File menu options
        case IDM_NEW: {
			// TODO
			_fileName = "Game1";
            break;
	    }

        case IDM_EXIT: { // terminate the application
			// TODO prompt for save
			UINT message = WM_CLOSE;
            ::SendMessage(window, message, 0, 0L);
            break;
	    }

		// Play menu options
		case IDM_PLAY_PLAY:
			// TODO
			break;

		case IDM_SWAP:
			// TODO
			break;

		case IDM_PAUSE:
			_pauseFlag = !_pauseFlag;
			_playMenu->pause(_pauseFlag);
			break;
        case IDM_AUTOPAUSE:
            _autopauseFlag = !_autopauseFlag;
			_playMenu->autopause(_autopauseFlag);
            break;

	    // View menu options
        case IDM_SMALL_TILES:
            _tileWidth = WIDTH_SMALL_TILE;
			_viewMenu->tileSize(command);
			forceRepaint();
            break;
        case IDM_MEDIUM_TILES:
            _tileWidth = WIDTH_MEDIUM_TILE;
			_viewMenu->tileSize(command);
			forceRepaint();
            break;
        case IDM_LARGE_TILES:
            _tileWidth = WIDTH_LARGE_TILE;
			_viewMenu->tileSize(command);
			forceRepaint();
            break;
        case IDM_RECENTER:
            _originHasBeenCentered = false;
            resize(_clientAreaWidth, _clientAreaHeight);
            break;

        case IDM_SHOW_CLOCKS:
            _showClocksFlag = !_showClocksFlag;
			_viewMenu->showClocks(_showClocksFlag);
			forceRepaint();
            break;
        case IDM_SHOW_GRID:
            toggleShowGrid();
            break;
        case IDM_SHOW_HINTS:
            _showHintsFlag = !_showHintsFlag;
			_viewMenu->showHints(_showHintsFlag);
			forceRepaint();
            break;
        case IDM_SHOW_SCORES:
            _showScoresFlag = !_showScoresFlag;
			_viewMenu->showScores(_showScoresFlag);
			forceRepaint();
            break;
        case IDM_SHOW_TILES:
            _showTilesFlag = !_showTilesFlag;
			_viewMenu->showTiles(_showTilesFlag);
			forceRepaint();
            break;

    	// Help menu options
        case IDM_RULES:
            ::MessageBox(window, "The Rules of Gold Tile", "Gold Tile", MB_ICONINFORMATION | MB_OK);
            break;
        case IDM_ABOUT:
	    	//AboutDialog about(_hInst, "About", window);
            break;
    }
}

void TopWindow::recenter(unsigned oldHeight, unsigned oldWidth) {
    if (_originHasBeenCentered) {
        //_originX += (_clientAreaWidth - oldWidth)/2;
        //_originY += (_clientAreaHeight - oldHeight)/2;
    } else if (_clientAreaWidth > 250 && _clientAreaHeight > 100) {
        _originX = _clientAreaWidth/2; // TODO
        _originY = _clientAreaHeight/2;
        _originHasBeenCentered = true;
    }
}

void TopWindow::repaint(void) {
    PAINTSTRUCT paintStruct;
    HWND windowHandle = getHandle();
    HDC contextHandle = ::BeginPaint(windowHandle, &paintStruct);
    assert(contextHandle != NULL);
    
    Canvas canvas(contextHandle, windowHandle, false);
    
    Board board = game->getBoard();
    int bottomRow = board.getMaxN();
    int topRow = -(int)board.getMaxS();
    int rightColumn = board.getMaxE();
    int leftColumn = -(int)board.getMaxW();
    ASSERT(bottomRow <= topRow);
    ASSERT(leftColumn <= rightColumn);
    
    for (int row = bottomRow; row <= topRow; row++) {
        if (getCellY(row) > (int)_clientAreaHeight) {
            break;
        }
        for (int column = leftColumn; column <= rightColumn; column++) {
            if (getCellX(column) > (int)_clientAreaWidth) {
                break;
            }
            GridRef square(row, column);
            drawCell(canvas, square);
        }
    }

    ::EndPaint(windowHandle, &paintStruct);
}

void TopWindow::resize(unsigned clientAreaWidth, unsigned clientAreaHeight) {
    unsigned oldHeight = _clientAreaHeight;
    unsigned oldWidth = _clientAreaWidth;
    setClientArea(clientAreaWidth, clientAreaHeight);
    recenter(oldHeight, oldWidth);
    forceRepaint();
}

void TopWindow::toggleShowGrid(void) {
    _showGridFlag = !_showGridFlag;
	_viewMenu->showGrid(_showGridFlag);
	forceRepaint();
}

void TopWindow::updateMenus(void) {
    _playMenu->autopause(_autopauseFlag);
    _playMenu->pause(_pauseFlag);

	switch (_tileWidth) {
 	    case WIDTH_SMALL_TILE:
	        _viewMenu->tileSize(IDM_SMALL_TILES);
			break;
 	    case WIDTH_MEDIUM_TILE:
	        _viewMenu->tileSize(IDM_MEDIUM_TILES);
			break;
 	    case WIDTH_LARGE_TILE:
	        _viewMenu->tileSize(IDM_LARGE_TILES);
			break;
		default:
			assert(false);
	}

    _viewMenu->showClocks(_showClocksFlag);
    _viewMenu->showGrid(_showGridFlag);
    _viewMenu->showHints(_showHintsFlag);
    _viewMenu->showScores(_showScoresFlag);
    _viewMenu->showTiles(_showTilesFlag);
}
