# Export all of the compile commands so I can use them in vim
SET( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)

# Revision1 project
PROJECT(Vision)

FIND_PACKAGE(OpenCV REQUIRED)

FIND_PACKAGE(Threads REQUIRED)

FIND_PACKAGE(GLUT)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})

FIND_PACKAGE(OpenGL)
include_directories(${OPENGL_INCLUDE_DIRS})
link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

# Manually the librealsense libraries and headers
FILE(GLOB Realsense_LIBS "/usr/local/lib/librealsense.so")
INCLUDE_DIRECTORIES("/usr/local/include/librealsense/")
FILE(GLOB TinySpline_LIBS "/usr/local/lib/libtinysplinecpp.so")
INCLUDE_DIRECTORIES("/usr/local/include/tinysplinecpp.h")

# Compile source files
FILE(GLOB SOURCES "src/*.cpp")
INCLUDE_DIRECTORIES("./include")

# Use c++11 and clang
SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

# Make all of the sources executable
ADD_EXECUTABLE(Vision ${SOURCES})

# Link libraries into the executable
TARGET_LINK_LIBRARIES(Vision ${OpenCV_LIBS} ${Realsense_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${TinySpline_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
