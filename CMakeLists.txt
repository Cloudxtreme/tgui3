cmake_minimum_required(VERSION 2.8)
project(tgui3 CXX)

include_directories("include")

set(USER_INCLUDE_PATH "" CACHE FILEPATH "Any extra include directories you want")
set(USER_LIBRARY_PATH "" CACHE FILEPATH "Any extra library directories you want")

include_directories(${USER_INCLUDE_PATH})
link_directories(${USER_LIBRARY_PATH})

set(SRC
	src/tgui3.cpp
)

add_library(tgui3 SHARED ${SRC})

if(WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DTGUI_WINDOWS /DWITH_SDL /DTGUI_LIB_BUILD")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DWITH_SDL /DTGUI_DEBUG /DTGUI_WINDOWS /DTGUI_LIB_BUILD")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_SDL -DTGUI_LIB_BUILD")
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTGUI_DEBUG")
	endif()
	if(APPLE)
		set_target_properties(tgui3 PROPERTIES MACOSX_RPATH on)
		include_directories("/Library/Frameworks/SDL2.framework/Headers")
	endif()
endif()

