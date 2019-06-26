
# Define CMAKE source directory
set(CMAKE_SOURCE_DIR /home/seloisel/01_Projets/02_PeseRuche/02_Logiciel)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/Arduino-Toolchain.cmake)

cmake_minimum_required(VERSION 3.8.2)

project(mcp2515)

# Define your Arduino board (below is for Arduino uno)
arduino_cmake_project(mcp2515 BOARD_NAME uno)

# Define the source code
add_arduino_executable(mcp2515 mcp2515.cpp)

# Define the port for uploading code to the Arduino
set_target_upload_port(mcp2515 /dev/tty/USB0)

find_arduino_library(spi_lib SPI ${board_id})

link_platform_library(${PROJECT_NAME} SPI "${board_id}")

