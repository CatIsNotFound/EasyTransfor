cmake_minimum_required(VERSION 3.16)

project(QEasyTransfor LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "E:/Qt/6.9.1/mingw_64")
list(APPEND CMAKE_PREFIX_PATH  "D:/CPPLibs/nlohnman_json")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(nlohmann_json REQUIRED)

add_executable(QEasyTransfor
  main.cpp
  src/Transfor.h src/Transfor.cpp
  src/Server.h src/Server.cpp
  src/CLI.h

)
target_link_libraries(QEasyTransfor
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Network
  nlohmann_json::nlohmann_json
)

include(GNUInstallDirs)
install(TARGETS QEasyTransfor
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
