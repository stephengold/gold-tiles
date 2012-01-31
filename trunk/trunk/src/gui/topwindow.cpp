// File:    topwindow.cpp
// Purpose: TopWindow class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
//#include "about.h"
#include "game.hpp"
#include "gridref.hpp"
#include "gui/canvas.hpp"
#include "gui/color.hpp"
#include "gui/resource.hpp"
#include "gui/topwindow.hpp"
#include "gui/windowclass.hpp"
#include "locus.hpp"
#include "play.hpp"

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
    _playMenu(NULL),
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

	// Make this TopWindow object accessable to its message handler before WM_CREATE.
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

    assert(game != NULL);
    Player activePlayer = game->getActivePlayer();
    _board = game->getBoard();

    _dragBoardFlag = false;
    _dragTileFlag = false;
    _handTiles = activePlayer.getHand();
    _mouseLastX = 0;
    _mouseLastY = 0;
    _numberOfColorAttributes = 0;
    _originHasBeenCentered = false;
    _originX = 0;
    _originY = 0;
    _padPixels = 6;
	_pauseFlag = false;
    _playedTileCount = 0;    
	_showClocksFlag = false;
	_showGridFlag = true;
	_showScoresFlag = true;
	_showTilesFlag = false;    
}

void TopWindow::buttonDown(int x, int y) {
    if (_pauseFlag) {
        menuCommand(IDM_PAUSE);
		return;
    }
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
    // Initializatio that happens after the Microsoft Windows window
    // has been created.
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


void TopWindow::drawActivePlayer(Canvas &canvas) {
    // draw header
    int top = _padPixels;
    int left = _padPixels;
    Player activePlayer = game->getActivePlayer();
    COLORREF areaColor = BLACK_COLOR;
    bool leftFlag = true;
    RECT headerRect = drawPlayerHeader(canvas, top, left, activePlayer, areaColor, leftFlag);
    left = headerRect.left;
    unsigned width = headerRect.right - left;
    
    // draw hand area (_handRect)
    unsigned cellHeight = getCellWidth();
    unsigned tileCount = _handTiles.size() - _swapTiles.size() - _playedTileCount;
    unsigned height = tileCount*cellHeight + 2*_padPixels;
    if (tileCount < _handTiles.size()) {
        height += cellHeight/2;
        areaColor = DARK_GREEN_COLOR;
    } else { // hand is full
        areaColor = BROWN_COLOR;
    }
    COLORREF edgeColor = WHITE_COLOR;
    canvas.useColors(areaColor, edgeColor);
    top = headerRect.bottom - 1;
    _handRect = canvas.drawRectangle(top, left, width, height);
    left = _handRect.left;
    width = _handRect.right - left;
    
    // draw swap area (_swapRect)
    tileCount = _swapTiles.size();
    unsigned bagHeight = canvas.getTextHeight();
    height = tileCount*cellHeight + bagHeight + 3*_padPixels;
    if (tileCount < _handTiles.size() && _playedTileCount == 0) {
        height += cellHeight/2;
        areaColor = DARK_GREEN_COLOR;
    } else { // can't add tiles to swap area
        areaColor = BROWN_COLOR;
    }
    canvas.useColors(areaColor, edgeColor);
    top = _handRect.bottom - 1;
    _swapRect = canvas.drawRectangle(top, left, width, height);
    
    unsigned stock = game->getStock();
    string stockText = plural(stock, "tile");
    int y = _swapRect.bottom - _padPixels - bagHeight;
    canvas.drawText(y, left, width, bagHeight, stockText);
}

RECT TopWindow::drawBlankTile(Canvas &canvas, int topY, int leftX) {
    COLORREF baseColor = LIGHT_GRAY_COLOR;
    canvas.useColors(baseColor, baseColor);
    unsigned width = getCellWidth() - 2;
    AValue glyphs[4];
    RECT result = canvas.drawTile(topY, leftX, width, 0, glyphs);
    
    return result;
}

void TopWindow::drawBoard(Canvas &canvas) {
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
}

void TopWindow::drawCell(Canvas &canvas, GridRef const &square) {
    int row = square.getRow();
    int column = square.getColumn();
    int ulcX = getCellX(column);
    int ulcY = getCellY(row);

    Locus done;
    bool connected = _board.connectsToOrigin(square, done);

    COLORREF cellColor = DARK_GREEN_COLOR;
    if (!connected) {
        cellColor = BLACK_COLOR;
    } else if (_swapTiles.size() != 0) {
        cellColor = BROWN_COLOR;
    }
    COLORREF gridColor = cellColor;
    if (_showGridFlag) {
        if (_swapTiles.size() != 0) {
            gridColor = YELLOW_COLOR;
        } else {
            gridColor = GREEN_COLOR;
        }
    }

    unsigned cellWidth = getCellWidth();
    canvas.drawCell(ulcY, ulcX, cellWidth, cellColor, gridColor);
    
    Tile const *tile = _board.getCell(square);
    if (tile != NULL && (!_dragTileFlag || tile->getId() != _dragTileId)) {
        drawTile(canvas, ulcY + 1, ulcX + 1, *tile);
    }
}

void TopWindow::drawHandTiles(Canvas &canvas) {   
    int handY = _handRect.top + _padPixels;
    
    unsigned cellWidth = getCellWidth();
    unsigned cellHeight = cellWidth;
    int swapY = _swapRect.top + _padPixels;
    if (_swapTiles.size() < _handTiles.size() && _playedTileCount == 0) {
        swapY += cellHeight/2;
    }

    _tileMap.clear();
    Tiles::const_iterator it;
    for (it = _handTiles.begin(); it != _handTiles.end(); it++) {
        Tile tile = *it;
        int x, y;
        GridRef square;
        if (_board.findTile(tile, square)) {
            int row = square.getRow();
            int column = square.getColumn();
            x = getCellX(column);
            y = getCellY(row);
        } else {
            if (_swapTiles.contains(tile)) {
                int left = _swapRect.left;
                int width = _swapRect.right - left;
                int pad = (width - cellWidth)/2;
                x = left + pad;
                y = swapY;
                swapY += cellHeight;
            } else { 
                int left = _handRect.left;
                int width = _handRect.right - left;
                int pad = (width - cellWidth)/2;
                x = left + pad;
                y = handY;
                handY += cellHeight;
            }
        }
        RECT rect = drawTile(canvas, y + 1, x + 1, tile);
        
        TileId id = tile.getId();
        TilePair pair(id, rect);
        std::pair<TileMap::iterator, bool> ins;
        ins = _tileMap.insert(pair);
        bool success = ins.second;
        ASSERT(success);
    }
}

void TopWindow::drawInactivePlayers(Canvas &canvas) {     
    unsigned cellWidth = getCellWidth();
    unsigned cellHeight = cellWidth;
    unsigned width = cellWidth + 2*_padPixels;
    int right = _clientAreaWidth - _padPixels;
    COLORREF areaColor = DARK_BLUE_COLOR;
    COLORREF edgeColor = LIGHT_GRAY_COLOR;
    vector<Player> otherPlayers = game->getInactivePlayers();
    vector<Player>::const_iterator player;
    for (player = otherPlayers.begin(); player < otherPlayers.end(); player++) {
        // draw header
        int top = _padPixels;
        bool leftFlag = false;
        COLORREF headerColor = BLACK_COLOR;
        RECT headerRect = drawPlayerHeader(canvas, top, right, *player, headerColor, leftFlag);

        // draw hand area below the header
        top = headerRect.bottom - 1;
        int left = headerRect.left;
        int width = headerRect.right - left;
        Tiles handTiles = player->getHand();
        unsigned tileCount = handTiles.size();
        unsigned height = tileCount*cellHeight + 2*_padPixels;
        areaColor = DARK_BLUE_COLOR;
        canvas.useColors(areaColor, edgeColor);
        RECT handRect = canvas.drawRectangle(top, left, width, height);

        // draw tiles
        int pad = (width - cellWidth)/2;
        int x = left + pad + 1;
        int y = handRect.top + _padPixels + 1;

        Tiles::const_iterator it;
        for (it = handTiles.begin(); it != handTiles.end(); it++) {
            Tile tile = *it;
            if (_showTilesFlag) {
                drawTile(canvas, y, x, tile);
            } else {
                drawBlankTile(canvas, y, x);
            } 
            y += cellHeight;
        }

        right = headerRect.left - _padPixels;
    }
}

RECT TopWindow::drawPlayerHeader(
    Canvas &canvas, 
    int top, 
    int leftRight, 
    Player const &player, 
    COLORREF areaColor, 
    bool leftFlag)
{
    unsigned cellWidth = getCellWidth();

    string nameText = player.getName();    
    unsigned w = canvas.getTextWidth(nameText);
    unsigned width = (cellWidth > w) ? cellWidth : w;

    string scoreText;
    if (_showScoresFlag) {
        unsigned score = player.getScore();
        scoreText = plural(score, "point");
        w = canvas.getTextWidth(scoreText);
        if (w > width) {
            width = w;
        }
    }
    
    char const *clockText = "";
    if (_showClocksFlag) {
        clockText = "0:00";
        w = canvas.getTextWidth(clockText);
        if (w > width) {
            width = w;
        }
    }
                                 
    width += 2*_padPixels;
    int left, right;
    if (leftFlag) {
        left = leftRight;
        right = left + width;
    } else {
        right = leftRight;
        left = right - width;
    }
    unsigned textHeight = canvas.getTextHeight();

    unsigned scoreHeight = 0;
    if (_showScoresFlag) {
        scoreHeight = textHeight;
    }
    unsigned clockHeight = 0;
    if (_showClocksFlag) {
        clockHeight = textHeight;
    }
    unsigned height = textHeight + scoreHeight + clockHeight + 2*_padPixels;
    canvas.useColors(areaColor, areaColor);
    RECT result = canvas.drawRectangle(top, left, width, height);

    COLORREF textColor = WHITE_COLOR;
    canvas.useColors(areaColor, textColor);

    int y = top + _padPixels;
    canvas.drawText(y, left, width, textHeight, nameText);
    y += textHeight;
    
    if (_showScoresFlag) {
        canvas.drawText(y, left, width, scoreHeight, scoreText);
        y += scoreHeight;
    }

    if (_showClocksFlag) {
        canvas.drawText(y, left, width, textHeight, clockText);
    }
    
    return result;
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
		BLACK_COLOR,      RED_COLOR,       DARK_BLUE_COLOR, 
        DARK_GREEN_COLOR, PURPLE_COLOR,    BROWN_COLOR, 
        DARK_GRAY_COLOR,  PINK_COLOR,      LIGHT_BLUE_COLOR
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
    canvas.useColors(baseColor, glyphColor);
    RECT result = canvas.drawTile(topY, leftX, width, 
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
		    updateMenus();
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
		case IDM_PAUSE:
			_pauseFlag = !_pauseFlag;
			forceRepaint();
			break;
		case IDM_PLAY_PLAY: {
            bool passFlag = false;
			play(passFlag);
			break;
        }
		case IDM_PASS: {
            bool passFlag = true;
            play(passFlag);
            break;
        }
            
        case IDM_AUTOPAUSE:
            _autopauseFlag = !_autopauseFlag;
			updateMenus();
            break;

	    // View menu options
        case IDM_SMALL_TILES:
            _tileWidth = WIDTH_SMALL_TILE;
			forceRepaint();
            break;
        case IDM_MEDIUM_TILES:
            _tileWidth = WIDTH_MEDIUM_TILE;
			forceRepaint();
            break;
        case IDM_LARGE_TILES:
            _tileWidth = WIDTH_LARGE_TILE;
			forceRepaint();
            break;
        case IDM_RECENTER:
            _originHasBeenCentered = false;
            resize(_clientAreaWidth, _clientAreaHeight);
            break;

        case IDM_SHOW_CLOCKS:
            _showClocksFlag = !_showClocksFlag;
			forceRepaint();
            break;
        case IDM_SHOW_GRID:
            _showGridFlag = !_showGridFlag;
	        forceRepaint();
            break;
        case IDM_SHOW_HINTS:
            _showHintsFlag = !_showHintsFlag;
			forceRepaint();
            break;
        case IDM_SHOW_SCORES:
            _showScoresFlag = !_showScoresFlag;
			forceRepaint();
            break;
        case IDM_SHOW_TILES:
            _showTilesFlag = !_showTilesFlag;
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

void TopWindow::play(bool passFlag) {
    Play pl;
    
    Tiles::const_iterator it;
    for (it = _handTiles.begin(); it != _handTiles.end(); it++) {
        Tile tile = *it;
        GridRef square;
        if (_board.findTile(tile, square)) {
           pl.add(tile, square);
        } else if (_swapTiles.contains(tile)) {
           pl.add(tile);
        }
    }
    
    if (pl.isPass() == passFlag && game->isValidPlay(pl)) {
        game->playTiles(pl);
        if (game->isOver()) {
            assert(false); // TODO
        }
        game->activateNextPlayer();
        Player activePlayer = game->getActivePlayer();
        _board = game->getBoard();
        _handTiles = activePlayer.getHand();
        _playedTileCount = 0;
        forceRepaint();
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
    HWND windowHandle = getHandle();
    PAINTSTRUCT paintStruct;    
    HDC contextHandle = ::BeginPaint(windowHandle, &paintStruct);
    assert(contextHandle != NULL);
    
    bool releaseMe = false;
    Canvas canvas(contextHandle, windowHandle, releaseMe, _clientAreaWidth, _clientAreaHeight);
    
    if (_pauseFlag) {
        int x = 0;
        int y = 0;
        int width = _clientAreaWidth;
        int height = _clientAreaHeight;
        COLORREF bgColor = BLACK_COLOR;
        COLORREF textColor = WHITE_COLOR;
        canvas.useColors(bgColor, textColor);
        canvas.drawText(x, y, width, height, "The game is paused.  Click here to resume.");
    } else {
        drawBoard(canvas);
        drawInactivePlayers(canvas);
        drawActivePlayer(canvas);
        drawHandTiles(canvas);
    }

    canvas.close();
    ::EndPaint(windowHandle, &paintStruct);
}

void TopWindow::resize(unsigned clientAreaWidth, unsigned clientAreaHeight) {
    unsigned oldHeight = _clientAreaHeight;
    unsigned oldWidth = _clientAreaWidth;
    setClientArea(clientAreaWidth, clientAreaHeight);
    recenter(oldHeight, oldWidth);
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
    unsigned count = _swapTiles.size() + _playedTileCount;
    bool playable = (count > 0);
    _playMenu->enableItems(_pauseFlag, playable);

    // "View" menu
    _viewMenu->showClocks(_showClocksFlag);
    _viewMenu->showGrid(_showGridFlag);
    _viewMenu->showHints(_showHintsFlag);
    _viewMenu->showScores(_showScoresFlag);
    _viewMenu->showTiles(_showTilesFlag);
    
	_viewMenu->enable(!_pauseFlag);
	
	// redraw
	HWND thisWindow = getHandle(); 
	BOOL success = ::DrawMenuBar(thisWindow);
	ASSERT(success);
}
#endif
