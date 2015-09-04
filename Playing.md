This page explains the Gold Tiles game to players.

## Quick start (Release 0.91) ##

1. Download the latest ZIPped release from http://code.google.com/p/gold-tiles/downloads/list

2. (optional) Extract the MS-Windows executable file "gold-tile-Win32.exe"

3. Double-click on the "gold-tile-Win32" icon to start the program.  A Gold Tile game window should open, and it should contain a "Create New Game" dialog box with "Practice" and "Standard Rules" pre-selected.

3a. If you get an error dialog about the a missing DLL, download and install MSVCP100.DLL from Microsoft's website at
http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=5555  and then try step 3 again.

3b. If anti-virus software removes or quarantines the file, restore it and check the SHA1 checksum.  If it matches, try step 3 again.

3c. If you get a system dialog warning you that the file's publisher is unknown, click on the "Run" button.

4. Click the "Next >" button.  You should see a "Hand Information" dialog box for the first hand, with "someone at this computer" pre-selected.

5. Click the "Next >" button.  You should see a "Hand Information" dialog box for the last hand, with "someone at this computer" pre-selected.

6. Click to select "this computer (automatic play)".

7. Click "Start Game".  A "Opening Bids" dialog box should appear.  Note who will play first (you or the computer) and how many tiles must be played.


## Definitions and rules ##

### Players and users ###

Gold Tile is a turn-based computer game for one or more **players**.
You can play it solitaire, play against a computer, or
play against other **users** (people) on the same computer.
You can even watch a computer play against itself.

The players and their names are configured at the start
of each game.  Players can resign, but they
cannot join a game after it has started.

### Tiles ###

The playing pieces in Gold Tile are two-sided **tiles**.
Each tile has **markings** on one side, which is call the **face**
Tiles may be square or hexagonal, depending on the type of
grid chosen at the start of the game.

Most tiles have a white or grey background.  However, a
fraction of the tiles may be **bonus** tiles, distinguished
from normal tiles by their gold or yellow background color.
Bonuses affect scoring, but not how tiles may be played.
The frequency of bonus tiles (0-25%) is configured at the
start of each game.

#### Tile attributes ####

The markings on a tile's face indicate the **values** of
its **attributes**.  Within a particular game, all
tiles have the same number of attributes, typically two.
The number of attributes per tile is configured at the
start of each game.

Each attribute takes at least two values, but no more than nine.
The number of values taken by each attribute is configured
at the start of each game.

Tiles commonly have two attributes, one indicated by a geometric
shape and the other denoted by the color of that shape.
For instance, in a five-color game the color attribute might
take the values _black_, _red_, _blue_, _green_, and _purple_.
In a four-shape game, the shape attribute might take the
values _circle_, _square_, _triangle_, and _star_.
However, tiles can have up to five attributes, and attributes
may also be indicated by letters or numbers.

During play, each user can customize how each attribute is
displayed.  (This may be especially helpful for players
who have difficulty distinguishing symbols and/or colors.)

In each game, the available tiles include at least one with
each possible **combination** of attribute values.
A game may also include **clones**--tiles with identical
markings (which therefore match in every attribute).
For instance, a game with four clones per combination would
include a total of five tiles per combination.
The number of clones per combination is configured at
the start of each game.

#### Tile compatibility ####

Two tiles are **compatible** if exactly one of their attributes
matches (has the same value).  If none of their attributes
match, then the tiles are not compatible.  Nor are tiles
compatible if two or more of their attributes match.
(Consequently, clones are never compatible.)

For instance, a tile having red markings is compatible
with any tile having black markings, provided their markings
are otherwise identical.  A tile marked with a square
is compatible with any tile marked with a circle, provided
their markings are otherwise identical.  And so on.

### Locations for tiles ###

The game is played by moving tiles from one **location**
to another.

Each player controls one or more **hands** which hold
tiles owned by that player.  A hand becomes **playable**
during its turn.  No more than one hand is playable
at any time.

The players share a board and a stock bag, which may
also contain tiles, and there is also a swap area,
used to perform special moves called swaps.
A player can access these locations only when
one of her/his/its tiles is playable.

#### Hands ####

Hands can only hold a limited number of tiles.
This limit is configured at the start of each game.

At the start of each game, all hands are full.
Toward the end, however hands may hold fewer tiles.
The number of tiles held by each hand is visible to all players,
but the markings on those tiles are visible only
to the player controlling that hand.
In normal games, players cannot see the markings on tiles in
other player's hands.

#### The board ####

The **board** is a two-dimensional grid of **cells**.

Each cell starts the game empty.  An **empty** cell becomes
**used** when a hand **places** one of its tiles face-up on the cell.
Once used, a cell remains used until the end of the game.
Once placed on a cell, a tile remains there until the end
of the game.

The cells are typically arranged in a 4-way square grid
with two **scoring axes**:  horizontal and vertical.
Alternatively, a hexagonal grid with three scoring
axes may be used, or a square grid with four
scoring axes:  horizontal, vertical and two diagonals.
The type of grid (and thus the number of scoring axes)
is configured at the start of each game.

A cell is **adjacent** to two neighboring cells
(or **neighbors**) along each scoring axis, one in
each **direction**.  For instance, on a hexagonal grid
each cell is adjacent to six neighbors.

A **rank** consists of the cells which can be reached from
a specific cell by moving (in either direction) along a
specific scoring axis.  For example, on a square grid
with two scoring directions, each row forms a rank
and each column also forms a rank.

Each board has an **start cell**, which is where the first
tile must be placed.  A board has no edges or other
features, it grows as needed in every direction.

#### The stock bag ####

The **stock bag** contains all the unplayed tiles which don't
belong to any hand.
The number of tiles in the bag is visible to all players,
but the markings on those tiles are not.
Players acquire tiles by **drawing** them blindly from the
stock bag -- without seeing the tiles' markings.

#### The swap area ####

A hand may return tiles to the stock bag on its turn.
To avoid drawing back returned tiles, the hand
temporarily places the returning tiles in a private
**swap area** while drawing replacements.
The swap area can hold as many tiles as a hand can.

### Setup ###

At the start of the game, all cells on the board are
empty and all tiles reside in the stock bag.

Each hand blindly **draws** tiles from the stock bag until
the hand is full, then **bids** for the first turn by
announcing the size of its largest set of mutually-compatible
tiles held by that hand.

The hand with the largest set of mutually-compatible
tiles takes the first turn.  On that turn, it may not
pass and must play exactly the number of tiles it bid.

### Taking turns ###

A hand's **turn** consists of making a **move** and then
(usually) drawing replacement tiles from the stock bag
until either the hand is full or the stock bag is empty.

There are four legal moves:
  * The hand may **play** one or more of its tiles to empty cells (on the board) in order to score points.
  * The hand may **swap** -- return one or more of its tiles to the stock bag by way of the swap area.
> > A swap does not score any points.
  * The hand may **pass** -- do nothing.
> > The hand does not score any points, nor does it draw replacement tiles.
  * The and may **resign** and drop out of the game.
> > The hand does not score any points.  Its tiles return to the stock bag, and it doesn't draw replacements.

At the end of a hand's turn, if it has played all its
tiles and is unable to draw any replacements (because the
stock bag is empty), the hand **goes out**, and the
game is over.  Otherwise, the hand on its
left which hasn't yet resigned gets the next turn.

In **challenge** games, each hand gets a limited amount of time
in which to make all its moves.  If a hand uses up its
allotted time, it is forced to resign.

#### Play restrictions ####

Every tile played must use an empty cell on the board.

Except for the start cell (used on the first turn), each
cell used in a play must be a neighbor of a previously used
cell.  In other words, all used cells must connect back
to the start cell.

The cells used in a play must all lie in a single rank,
with no  intervening empty cells.

If two or more tiles are placed in single rank without any
intervening empty cells, the tiles must all be mutually compatible.
(Consequently, tiles placed in adjacent cells must be compatible.)

#### Swapping restrictions ####

The number of tiles returned during a swap may not exceed
the number of tiles remaining in the swap bag.

### Game over ###

A game ends normally when the stock bag is empty and one
hand has gone out.

Alternatively, a game may end when all hands have resigned.

### Scoring ###

Each hand starts the game with zero points.
A hand can earn points, but points are never taken away
from a hand.

#### For making a play ####

After a hand makes a play, it earns points as follows:

For each rank where two or more tiles were connected by the play, the **base score** is the number of connected tiles in that rank at the end of the turn.

Each base score gets adjusted as follows:
  * If the connected tiles all share a single common attribute and the number of connected tiles equals the number of values for the attribute, then the base score is doubled.
  * For each bonus tile among the connected tiles, the base score is doubled yet again.

The adjusted base scores get added to the playable hand's points.

#### For going out ####

After a hand goes out, it earns a point for each tile
held by any of the remaining hands.

## Graphical interface ##

The game window consists of a menu bar and a game view.

![http://sgold.members.sonic.net/images/scap1.png](http://sgold.members.sonic.net/images/scap1.png)

The **menu bar** across the top of the window provides
pull-down menus of **commands** you can invoke in order
to control the game.
Many of these commands have keyboard shortcuts.

The **game view** portion of the game window illustrates
the game in progress, including tiles, hands, the board,
and so on.

The board fills the background of the game view area.
You can grab the board and drag it with your mouse.

The start cell is marked with the word START (or the letter S).

The playable hand floats above the board in the upper
left corner of the game view.  Other hands float
in the upper right of the game view.

Non-movable tiles have gray (or dark yellow) backgrounds.
Movable tiles are shown with white (or bright yellow) backgrounds.

There are several ways to move tiles:
  * You can grab a movable tile and drag it with the mouse.
  * You can click on a movable tile to **activate** it, then click a second time on its destination.
  * You can click on an empty cell to make it the **target**, then click on a tile to place it on the target.  (The target is indicated by a thick, gray arrow.)