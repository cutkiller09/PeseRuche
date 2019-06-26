# Define CMAKE source directory
set(CMAKE_SOURCE_DIR /home/seloisel/01_Projets/02_PeseRuche/02_Logiciel)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/Arduino-Toolchain.cmake)

cmake_minimum_required(VERSION 3.8.2)

project(Module_Pesee)

# Define your Arduino board (below is for Arduino uno)
arduino_cmake_project(Module_Pesee BOARD_NAME uno)

# Define the source code
add_arduino_executable(Module_Pesee Module_Pesee.cpp)

# Define the port for uploading code to the Arduino
set_target_upload_port(Module_Pesee /dev/tty/USB0)
