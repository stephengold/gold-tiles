## Builds ##

There is just one production build of the Gold Tiles Game:

  * gold-tile-Win32 - The main product, targeting the Microsoft Windows(tm) platform.

This build implements a graphical user interface (GUI) which assumes the presence of a mouse or other pointing device.  With regard to network play, it acts as both a client (initiator) and a server (remote player).  The graphics are based on GDI, and the networking is based on WinSock2.  The '_CLIENT', '_SERVER', 'WIN32', and '_WINDOWS' preprocessor macros are defined._

A port to the Qt platform is in progress.

There are several builds used for unit testing, including:

  * gold-tile-client - implements a console user interface for
multiple platforms, including MS-Windows, Qt, and Posix.  With regard
to network play, it acts as both a client (initiator) only.  The '_CLIENT' and '_NETWORK\_TEST' preprocessor macros are defined.

  * gold-tile-server - implements a console user interface for
multiple platforms, including MS-Windows, Qt, and Posix.  With regard
to network play, it acts as both a server (remote player) only.  The '_NETWORK\_TEST' and '_SERVER' preprocessor macros are defined.

To build a debugging version (with assertions enabled) define the '_DEBUG' preprocessor macro._


## Coding standards ##

### Source files ###

Source files needed only for GUI builds belong in src/gui.

Every header file should use #ifndef to guard against multiple inclusions.

For every class Foo there should ideally be a declaration header file "foo.hpp" and an implementation file "foo.cpp".

Project-wide definitions belong in "project.hpp"

### Indentation ###

  * Indent the contents of each block (except labels) by four spaces.
  * Use Unix-style indentation when the open brace fits on the same line with the preceding keyword. If it doesn't fit, align the open brace with the keyword.

### Identifiers ###

#### Prefixes ####

  * g = global
  * i = iterator over
  * m = member data (i.e. attribute)
  * p = pointer
  * r = reference
  * s = static

#### Suffixes ####

  * s = collection of

For instance, if "Foo" is the name of a class, then the container
class for Foo objects would be named "Foos".

#### Capitalization ####

  * attributes - `mMixedCasePrefixedByLowercaseM (this->mWidth)`
  * auto variables - `all_lowercase_with_underscores (int all_done;)`
  * classes - `MixedCaseStartingWithUppercase (class MenuItem)`
  * enum values - `ALL_UPPER_CASE_WITH_UNDERSCORES (COLOR_BLACK)`
  * formal parameters - `mixedCaseStartingWithLowercase  (bool goFlag)`
  * global variables - `gMixedCasePrefixedByLowercaseG (gData)`
  * methods - `MixedCaseStartingWithUppercase (this->HideAll())`
  * preproc macros - `ALL_UPPER_CASE_WITH_UNDERSCORES (ASSERT(e))`
  * types - `MixedCaseSuffixedWithType (typedef int IdType;)`

See http://www.possibility.com/Cpp/CppCodingStandard.html for rationale.

#### Abbreviations ####

Abbreviations in identifiers are used sparingly.

  * attr = attribute
  * cnt = count
  * const = constant
  * id = identifier
  * ind = index
  * opt = options
  * max = maximum
  * min = minimum
  * msec = millisecond
  * poly = polygon
  * rect = rectangle
  * ulc = upper left corner
  * um = user message