cmake_minimum_required (VERSION 2.6)
project (HelloSpine)

add_subdirectory(../../../../oxygine-framework/ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})


add_subdirectory(../../../ oxygine-spine)
add_definitions(${OXYGINE_SPINE_DEFINITIONS})
include_directories(${OXYGINE_SPINE_INCLUDE_DIRS})
link_directories(${OXYGINE_SPINE_LIBRARY_DIRS})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

add_executable(HelloSpine ../src/example.cpp ../src/main.cpp  ../src/example.h  ../src/test.h  ../src/test.cpp )
target_link_libraries(HelloSpine ${OXYGINE_CORE_LIBS}  ${OXYGINE_SPINE_LIBS})



if (WIN32) #disable console mode for VC++
	set_target_properties(HelloSpine PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)



if (EMSCRIPTEN)
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WASM=0 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648 -s FORCE_FILESYSTEM=1")
	em_link_pre_js(HelloSpine  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)