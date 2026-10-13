cmake_minimum_required(VERSION 3.16)

project(seasmoke VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network)

qt_standard_project_setup(REQUIRES 6.10)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

set(SEASMOKE_SOURCES
    src/app/main.cpp
    src/data/network/ApiClient.cpp
    src/data/network/ApiClient.h
    src/data/JsonMappers.cpp
    src/data/JsonMappers.h
    src/data/ApiService.cpp
    src/data/ApiService.h
    src/domain/Entities.h
    src/domain/IApiService.h
    src/presentation/models/JobListModel.cpp
    src/presentation/models/JobListModel.h
    src/presentation/models/RequestListModel.cpp
    src/presentation/models/RequestListModel.h
    src/presentation/MainViewModel.cpp
    src/presentation/MainViewModel.h
)

qt_add_executable(appseasmoke
    ${SEASMOKE_SOURCES}
)

qt_add_qml_module(appseasmoke
    URI seasmoke
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

set_target_properties(appseasmoke PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appseasmoke
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(appseasmoke
    PRIVATE
        Qt6::Quick
        Qt6::Network
)

install(TARGETS appseasmoke
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
