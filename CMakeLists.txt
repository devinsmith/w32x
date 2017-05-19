PROJECT (w32x C)
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.12)

# We need X11 at least.
FIND_PACKAGE (X11)
INCLUDE_DIRECTORIES (${X11_INCLUDE_DIR})

# Add "include" directory.
INCLUDE_DIRECTORIES (include)

LIST(APPEND libw32x_src
  src/button.c
  src/defwnd.c
  src/graphics.c
  src/w32x.c
  src/winuser.c)

ADD_LIBRARY(w32x STATIC ${libw32x_src})

ADD_SUBDIRECTORY(test)

