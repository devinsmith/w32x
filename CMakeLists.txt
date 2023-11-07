cmake_minimum_required(VERSION 3.7.2)
project(w32x C)

# We need X11 at least.
find_package(X11 REQUIRED)
find_package(Freetype)

include_directories(${X11_INCLUDE_DIR})

if(FREETYPE_FOUND)
  include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

# Add "include" directory.
include_directories(include)

list(APPEND libw32x_src
  src/button.c
  src/defwnd.c
  src/graphics.c
  src/menu.c
  src/rect.c
  src/w32x.c
  src/winuser.c)

add_library(w32x STATIC ${libw32x_src})

add_subdirectory(test)

