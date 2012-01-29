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
#include "play.hpp"
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
		// constructing first instance:  create a Microsoft Windows window class
		WNDPROC messageHandler = getMessageHandler();
		_class = new WindowClass(applicationInstance, messageHandler, className);
		_class->registerClass();
	}
	assert(_class != NULL);

	// Make this TopWindow object accessable to message handler before WM_CREATE.
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
    HWND handle = ::CreateWindow(className, windowName, windowStyle, x, y, 
                             width, height, parent, menu, applicationInstance, 
                             parameters);
    assert(handle != 0);
    setHandle(handle);

    _board = game->getBoard();
    _dragBoardFlag = false;
    _dragTileFlag = false;
    Player activePlayer = game->getActivePlayer();
    _handTiles = activePlayer.getHand();
    _mouseLastX = 0;
    _mouseLastY = 0;
    _newStep = true;
    _numberOfColorAttributes = 0;
    _originHasBeenCentered = false;
    _originX = 0;
    _originY = 0;
    _playedTileCount = 0;   
    
}

void TopWindow::buttonDown(int x, int y) {
    HWND thisWindow = getHandle();
    POINT pt; pt.x = x; pt.y = y;
    
    TileMap::const_iterator it;
    for (it = _tileMap.begin(); it != _tileMap.end(); it++) {
        RECT rect = it->second;
        if (::PtInRect(&rect, pt)) {
            break;
        }
    }
    if (it != _tileMap.end()) {
        // Capture mouse to drag a tile
        ::SetCapture(thisWindow);
        _mouseLastX = x;
        _mouseLastY = y;
        _dragTileDeltaX = 0;
        _dragTileDeltaY = 0;
        _dragTileFlag = true;
        _dragTileId = it->first;
        
    } else if (!::PtInRect(&_handRect, pt) &&
               !::PtInRect(&_swapRect, pt))
    {
        // Capture mouse to drag the board
    	::SetCapture(thisWindow);

        _mouseLastX = x;
        _mouseLastY = y;
        _dragBoardFlag = true;
    }
}

void TopWindow::buttonUp(int x, int y) {
    HWND thisWindow = getHandle();
	HWND captor = ::GetCapture();
    if (captor != thisWindow) {
		return;
    } else {
        ::ReleaseCapture();
    }
		
    int dragX = x - _mouseLastX;
    int dragY = y - _mouseLastY;
    if (_dragBoardFlag) {
        _originX += dragX;
        _originY += dragY;
        
    } else {
        assert(_dragTileFlag);
        
        _dragTileDeltaX += dragX;
        _dragTileDeltaY += dragY;
        
        // Determine where the tile was dragged from.
        bool fromBoard = false;
        bool fromSwap = false;
        GridRef fromSquare;
        
        Tile dragTile = _handTiles.findTile(_dragTileId);
        if (_board.findTile(dragTile, fromSquare)) {
            fromBoard = true;
            _board.emptyCell(fromSquare);
            --_playedTileCount;
        } else if (_swapTiles.contains(dragTile)) {
            fromSwap = true;
            _swapTiles.removeTile(dragTile);
        }

        // Determine where the tile got dragged to.
        POINT point; point.x = x; point.y = y;
        
        if (::PtInRect(&_swapRect, point)) { // to swap
            if (_playedTileCount == 0) {
                _swapTiles.addTile(dragTile);
            } else if (fromBoard) {
                _board.playOnCell(fromSquare, dragTile);
                ++_playedTileCount;
            }
            
        } else if (!::PtInRect(&_handRect, point)) { // to board
            GridRef toSquare = getCellRef(x, y);
            Play play;
            play.add(dragTile, toSquare);
            if (_swapTiles.size() == 0 && _board.isLegalPlay(play)) {
                _board.playTiles(play);
                ++_playedTileCount;
            } else if (fromBoard) {
                _board.playOnCell(fromSquare, dragTile);
                ++_playedTileCount;
            } else if (fromSwap) {
                _swapTiles.addTile(dragTile);
            }
        }
    }
    forceRepaint();
    
    _dragBoardFlag = false;
    _dragTileFlag = false;
    _mouseLastX = x;
    _mouseLastY = y;
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

void TopWindow::dragMouse(int x, int y) {
    HWND thisWindow = getHandle();
	HWND captor = ::GetCapture();
    if (captor != thisWindow) {
		return;
    }

    int dragX = x - _mouseLastX;
    int dragY = y - _mouseLastY;
    if (_dragBoardFlag) {
        _originX += dragX;
        _originY += dragY;
        forceRepaint();
    } else if (_dragTileFlag) {
        _dragTileDeltaX += dragX;
        _dragTileDeltaY += dragY;
        forceRepaint();
    }

    _mouseLastX = x;
    _mouseLastY = y;
}

void TopWindow::drawCell(Canvas &canvas, GridRef const &square) {
    int row = square.getRow();
    int column = square.getColumn();
    int ulcX = getCellX(column);
    int ulcY = getCellY(row);

    Locus done;
    bool connected = _board.connectsToOrigin(square, done);

    COLORREF cellColor = DARK_GREEN_COLOR;
    COLORREF gridColor = cellColor;
    if (!connected) {
        cellColor = BLACK_COLOR;
    } else if (_swapTiles.size() != 0) {
        cellColor = BROWN_COLOR;
    }
    if (_showGridFlag) {
        gridColor = GREEN_COLOR;
    }

    unsigned cellWidth = getCellWidth();
    canvas.drawCell(ulcY, ulcX, cellWidth, cellColor, gridColor);
    
    Tile const *tile = _board.getCell(square);
    if (tile != NULL) {
        if (!_dragTileFlag || tile->getId() != _dragTileId) {
            drawTile(canvas, ulcY + 1, ulcX + 1, *tile);
        }
    }
}

RECT TopWindow::drawTile(Canvas &canvas, int topY, int leftX, Tile const &tile) {
    COLORREF baseColor = LIGHT_GRAY_COLOR;
    if (_dragTileFlag && tile.getId() == _dragTileId) {
       topY += _dragTileDeltaY;
       leftX += _dragTileDeltaX;
       baseColor = WHITE_COLOR;
    }
	
	AIndex colorInd;
	if (_numberOfColorAttributes == 1) {
        colorInd = tile.getAttribute(0);
	} else {
		assert(_numberOfColorAttributes == 0);
        colorInd = 0;
	}
	assert(colorInd < 9);
	static COLORREF GlyphColors[9] = {
		BLACK_COLOR, RED_COLOR,       DARK_BLUE_COLOR, DARK_GREEN_COLOR, PURPLE_COLOR,
		BROWN_COLOR, DARK_GRAY_COLOR, PINK_COLOR,      LIGHT_BLUE_COLOR
	};
    COLORREF glyphColor = GlyphColors[colorInd];

    ACount numberOfGlyphAttributes = Tile::getNumAttributes() - _numberOfColorAttributes;
    AValue glyphs[4];
    for (AIndex gi = 0; gi < 4; gi++) {
		AIndex ind = gi + _numberOfColorAttributes;
         if (gi < numberOfGlyphAttributes) {
             glyphs[gi] = tile.getAttribute(ind);
         } else {
             glyphs[gi] = 0;
         }
    }

    unsigned width = getCellWidth() - 2;
    RECT result = canvas.drawTile(topY, leftX, width, baseColor, glyphColor, 
            numberOfGlyphAttributes, glyphs);
    
    return result;
}

GridRef TopWindow::getCellRef(int x, int y) {
    unsigned gridUnit = getGridUnit();
    int column = -100 + (x - _originX + 100*gridUnit)/gridUnit;
    int row = 100 - (y - _originY + 100*gridUnit)/gridUnit;
    GridRef result(row, column);
    
    return result;
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
            buttonDown(xy.x, xy.y);
            break;
		}

        case WM_LBUTTONUP: {
			POINTS xy = MAKEPOINTS(lParam);
            buttonUp(xy.x, xy.y);
            break;
		}

        case WM_MOUSEMOVE:
            if (wParam & MK_LBUTTON) { // dragging with left button
				POINTS xy = MAKEPOINTS(lParam);
                dragMouse(xy.x, xy.y);
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

		case IDM_PAUSE:
			_pauseFlag = !_pauseFlag;
			_playMenu->pause(_pauseFlag);
			forceRepaint();
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
    
    bool releaseMe = false;
    Canvas canvas(contextHandle, windowHandle, releaseMe);
    
    if (_pauseFlag) {
        ::EndPaint(windowHandle, &paintStruct);
        return;
    }

    // draw the board
    int topRow = 1 + _board.getMaxN();
    int bottomRow = -1 - (int)_board.getMaxS();
    int rightColumn = 1 + _board.getMaxE();
    int leftColumn = -1 - (int)_board.getMaxW();
    ASSERT(bottomRow <= topRow);
    ASSERT(leftColumn <= rightColumn);
    
    for (int row = topRow; row >= bottomRow; row--) {
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
    
    // draw the active player's hand
    unsigned cellWidth = getCellWidth();
    unsigned cellHeight = cellWidth;
    unsigned bagHeight = cellHeight;
    unsigned pad = 6;
    COLORREF edgeColor = WHITE_COLOR;

    unsigned tileCount = _swapTiles.size();
    int top = 4;
    int left = 4;
    int width = cellWidth + 2*pad;
    int height = tileCount*cellHeight + bagHeight + 3*pad;
    COLORREF areaColor;
    if (tileCount < _handTiles.size() && _playedTileCount == 0) {
        height += cellHeight/2;
        areaColor = DARK_GREEN_COLOR;
    } else { // can't add to swap
        areaColor = BROWN_COLOR;
    }
    _swapRect = canvas.drawRectangle(top, left, width, height, areaColor, edgeColor);
    
    unsigned clockHeight = 0;
    if (_showClocksFlag) {
        clockHeight = cellWidth + pad;
    }
    tileCount = _handTiles.size() - _swapTiles.size() - _playedTileCount;
    top = _swapRect.bottom - 1;
    left = _swapRect.left;
    height = tileCount*cellHeight + clockHeight + 2*pad;
    if (tileCount != _handTiles.size()) {
        height += cellHeight/2;
        areaColor = DARK_GREEN_COLOR;
    } else { // hand is full
        areaColor = BROWN_COLOR;
    }
    _handRect = canvas.drawRectangle(top, left, width, height, areaColor, edgeColor);

    _tileMap.clear();
    int handY = _handRect.top + pad + 1;
    if (tileCount != _handTiles.size()) {
        handY += cellHeight/2;
    }
    int swapY = _swapRect.top + bagHeight + 2*pad;
    Tiles::const_iterator it;
    for (it = _handTiles.begin(); it != _handTiles.end(); it++) {
        Tile tile = *it;
        int x, y;
        GridRef square;
        if (_board.findTile(tile, square)) {
            int row = square.getRow();
            int column = square.getColumn();
            x = getCellX(column) + 1;
            y = getCellY(row) + 1;
        } else {
            x = left + pad + 1;
            if (_swapTiles.contains(tile)) {
                y = swapY;
                swapY += cellHeight;
            } else { 
                y = handY;
                handY += cellHeight;
            }
        }
        RECT rect = drawTile(canvas, y, x, tile);
        TileId id = tile.getId();
        TilePair pair(id, rect);
        std::pair<TileMap::iterator, bool> ins;
        ins = _tileMap.insert(pair);
        bool success = ins.second;
        ASSERT(success);
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
