set(CMAKE_SYSTEM_VERSION 10.0.19041.0)
set(WINDOWS TRUE)
set(PLATFORM_NAME "Windows")

add_definitions(-DAE_WINDOWS -D_WINDOWS -DWIN32 -D_WIN32 -D_WIN64 -D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_CONFIGURATION_TYPES Debug Profile Release)
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)

set(MSVC_COMMON_FLAGS 
	/nologo     # Don't show version info
	/W3         # Enable warning level 3
	/fp:fast    # Use fast floating point precision model
	/Zc:wchar_t # Parse wchar_t as internal type
	/GF         # Eliminate Duplicate Strings
	/Gy         # Enable function level linking
	/utf-8      # Set source and execution charset to utf-8
	/Wv:18      # Disable warnings until SDK depedencies switch to UTF-8/ASCII.
	/MP         # Build with multiple processes
	/bigobj     # Allow larger .obj files

	/WX         # Treat warnings as errors
	/wd4653     # Ignore PCH for any individual file that has different optimization settings
	/wd4275     # non dll-interface class '' used as base for dll-interface class ''
	/wd4961     # No profile data was merged into '', profile-guided optimizations disabled
	/wd4068     # 'Unknown pragma' - sometimes need these in code to prevent global suppression.
)
string(REPLACE ";" " " MSVC_COMMON_FLAGS "${MSVC_COMMON_FLAGS}")

set(CMAKE_RC_FLAGS /nologo)

# Override cxx flags
set(CMAKE_CXX_FLAGS "${MSVC_COMMON_FLAGS}" CACHE STRING "C++ Common Flags" FORCE)

set(CMAKE_C_FLAGS_DEBUG "/MDd /Zi /Zo /Od /Ob0 /Oy- /RTC1 /GS /DDEBUG /D_DEBUG" CACHE STRING "C Flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Zo /Od /Ob0 /Oy- /RTC1 /GS /DDEBUG /D_DEBUG" CACHE STRING "C++ Flags" FORCE)

# Create PDBs (/Zi)
# Create better debug info (/Zo)
# Enable full optimization (/Ox) Same as /Ob2 /Oi /Ot /Oy
# Don't omit frame pointer (/Oy-)
# Disable buffer security checks (/GS-)
set(CMAKE_C_FLAGS_PROFILE "/Zi /Zo /MD /Ox /Oy- /GS- /DNDEBUG /D_PROFILE" CACHE STRING "C Flags" FORCE)
set(CMAKE_CXX_FLAGS_PROFILE "/Zi /Zo /MD /Ox /Oy- /GS- /DNDEBUG /D_PROFILE" CACHE STRING "C++ Flags" FORCE)

set(CMAKE_C_FLAGS_RELEASE "/Zi /Zo /MD /Ox /GS- /DNDEBUG /D_RELEASE" CACHE STRING "C Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "/Zi /Zo /MD /Ox /GS- /DNDEBUG /D_RELEASE" CACHE STRING "C++ Flags" FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "/debug" CACHE STRING "Profile link flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PROFILE    "/debug" CACHE STRING "Profile link flags" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_PROFILE "/debug /INCREMENTAL" CACHE STRING "Profile link flags" FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/debug" CACHE STRING "Release link flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "/debug" CACHE STRING "Release link flags" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "/debug  /OPT:REF /OPT:ICF" CACHE STRING "Release link flags" FORCE)