# ======================================================================
#  lareventdisplay main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/lareventdisplay/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

project(lareventdisplay VERSION 08.12.04 LANGUAGES CXX)

find_package(cetmodules REQUIRED)
find_package(canvas_root_io)
find_package(art)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcoreobj )
find_ups_product( larcorealg )
find_ups_product( larcore )
find_ups_product( lardataobj )
find_ups_product( lardataalg )
find_ups_product( lardata )
find_ups_product( larevt )
find_ups_product( larsim )
find_ups_product( larreco )
find_ups_product( eigen )
find_ups_product( nuevdb )
find_ups_product( nug4 )
find_ups_product( nugen )
find_ups_product( art )
find_ups_product(art_root_io)
find_package( postgresql )

# use the system zlib
find_package( ZLIB REQUIRED )
include_directories( ZLIB_INCLUDE_DIRS )

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(lareventdisplay)
add_subdirectory(cmakeConfig)
# ups - table and config files
if(WANT_UPS)
add_subdirectory(ups)
endif()

# packaging utility
#include(UseCPack)
