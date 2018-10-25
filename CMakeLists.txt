cmake_minimum_required (VERSION 2.6)
project (OXYGINE_SPINE)

set(OXYGINE_SPINE_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spine/include
	)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(OXYGINE_SPINE_DEFINITIONS	
	-DOXYGINE_SPINE=1)


file(GLOB OX_SPINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/oxygine-spine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/oxygine-spine/*.h)
#source_group(lib FILES ${OX_SPINE_SRC})


file(GLOB SPINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spine/src/spine/*.cpp)


source_group(lib FILES ${SPINE_SRC})

add_library(oxygine-spine STATIC ${OX_SPINE_SRC} ${SPINE_SRC})

set(OXYGINE_SPINE_LIBS
		oxygine-spine)	

if(EMSCRIPTEN)
	set(linkFlags "-Wno-warn-absolute-paths")
	set_target_properties(oxygine-spine PROPERTIES LINK_FLAGS "${linkFlags}")
endif(EMSCRIPTEN)

include_directories(${OXYGINE_SPINE_INCLUDE_DIRS})
include_directories(../oxygine-framework/oxygine/src)



set(OXYGINE_SPINE_INCLUDE_DIRS ${OXYGINE_SPINE_INCLUDE_DIRS} PARENT_SCOPE)
set(OXYGINE_SPINE_LIBS ${OXYGINE_SPINE_LIBS} PARENT_SCOPE)
set(OXYGINE_SPINE_DEFINITIONS ${OXYGINE_SPINE_DEFINITIONS} PARENT_SCOPE)
