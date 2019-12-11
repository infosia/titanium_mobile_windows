include(Compiler/XL)
__compiler_xl(CXX)
string(APPEND CMAKE_CXX_FLAGS_RELEASE_INIT " -DNDEBUG")
string(APPEND CMAKE_CXX_FLAGS_MINSIZEREL_INIT " -DNDEBUG")

# -qthreaded = Ensures that all optimizations will be thread-safe
string(APPEND CMAKE_CXX_FLAGS_INIT " -qthreaded")

if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.1)
  if(CMAKE_SYSTEM MATCHES "Linux")
    set(CMAKE_CXX98_STANDARD_COMPILE_OPTION "")
    set(CMAKE_CXX98_EXTENSION_COMPILE_OPTION "")
  else()
    set(CMAKE_CXX98_STANDARD_COMPILE_OPTION "-qlanglvl=strict98")
    set(CMAKE_CXX98_EXTENSION_COMPILE_OPTION "-qlanglvl=extended")
  endif()
  set(CMAKE_CXX11_STANDARD_COMPILE_OPTION "-qlanglvl=extended0x")
  set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "-qlanglvl=extended0x")
endif ()

__compiler_check_default_language_standard(CXX 10.1 98)

set(CMAKE_CXX_COMPILE_OBJECT
  "<CMAKE_CXX_COMPILER> -+ <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
