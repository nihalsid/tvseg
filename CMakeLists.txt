cmake_minimum_required(VERSION 3.0.0)

project(tvseg_windows VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED CMAKE_CUDA_STANDARD)
    set(CMAKE_CUDA_STANDARD 11)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

list(APPEND CMAKE_PREFIX_PATH "D:/nihalsid/opencv-3.4.4/install")
set (QT_UIC_EXECUTABLE "C:/Qt/5.12.0/msvc2017_64/bin/uic.exe")

find_package(Qt5 COMPONENTS Core Gui Widgets Concurrent REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CUDA 10.0 REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS regex system filesystem thread REQUIRED)

set (UIS
    "${PROJECT_SOURCE_DIR}/src/consoledockwidget.ui"
    "${PROJECT_SOURCE_DIR}/src/feedbackdockwidget.ui"
    "${PROJECT_SOURCE_DIR}/src/feedbackwidget.ui"
    "${PROJECT_SOURCE_DIR}/src/tvsegwindow.ui")

file(GLOB SOURCES
                "${PROJECT_SOURCE_DIR}/lib/tvseg/*.cpp"
                "${PROJECT_SOURCE_DIR}/lib/tvseg/cuda/*.cpp"
                "${PROJECT_SOURCE_DIR}/lib/tvseg/settings/*.cpp"
                "${PROJECT_SOURCE_DIR}/lib/tvseg/util/*.cpp"
                "${PROJECT_SOURCE_DIR}/lib/tvseg/3rdparty/*.cc"
                "${PROJECT_SOURCE_DIR}/lib/tvseg/cuda/*.cpp"
                "${PROJECT_SOURCE_DIR}/lib/tvseg/cuda/*.cu"
                "${PROJECT_SOURCE_DIR}/src/*.cpp"
                "${PROJECT_SOURCE_DIR}/src/3rdparty/qtpropertybrowser/*.cpp"
)

include_directories(
        ${PROJECT_SOURCE_DIR}/lib
	${PROJECT_SOURCE_DIR}/lib/cuda_samples/common/inc
	${PROJECT_SOURCE_DIR}/lib/boost
        ${PROJECT_SOURCE_DIR}/src/util
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/3rdparty/qtpropertybrowser
        ${Boost_INCLUDE_DIRS}
)

message (STATUS "UIHEADERS: ${UI_HEADERS}")

cuda_add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPERABLE_COMPILATION ON)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent ${OpenCV_LIBS} ${Boost_LIBRARIES} ${CUDA_npp_LIBRARY})
