if (WIN32)
    cmake_minimum_required(VERSION 3.12)
    add_compile_options(/std:c++17)
elseif (APPLE)
    cmake_minimum_required(VERSION 3.17)
elseif (UNIX)
    cmake_minimum_required(VERSION 3.17)
endif ()

project(OcrOnnx)

# Output BIN JNI CLIB
if (NOT DEFINED OCR_OUTPUT)
    set(OCR_OUTPUT "BIN")
    message(STATUS "No OCR_OUTPUT, defaulting to BIN")
endif ()

add_definitions(-DUNICODE -D_UNICODE)

if (MSVC)
    add_compile_options(/W3)  # MSVC 使用 /W3 (常规警告)
else()
    add_compile_options(-Wall) # GCC/Clang 使用 -Wall
endif()

# OnnxRuntime (CPU only)
set(OnnxRuntime_FOUND TRUE)
set(OnnxRuntime_LIBS "C:/onnxruntime/lib/onnxruntime.lib")
set(OnnxRuntime_INCLUDE_DIRS "C:/onnxruntime/include")
include_directories(${OnnxRuntime_INCLUDE_DIRS})

if (OnnxRuntime_FOUND)
    message(STATUS "OnnxRuntime_LIBS: ${OnnxRuntime_LIBS}")
    message(STATUS "OnnxRuntime_INCLUDE_DIRS: ${OnnxRuntime_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "onnxruntime Not Found!")
endif ()

# OpenCV
set(BUILD_SHARED_LIBS false)
find_package(OpenCV REQUIRED)

if (OpenCV_FOUND)
    message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "opencv Not Found!")
endif ()

# project include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# source
file(GLOB OCR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(OCR_COMPILE_CODE ${OCR_SRC})

# JNI
if (OCR_OUTPUT STREQUAL "JNI")
    find_package(JNI REQUIRED)
    if (JNI_FOUND)
        message("JNI FOUND")
        message(STATUS "JNI_LIBS: ${JNI_LIBS}")
        message(STATUS "JNI_INCLUDE_DIRS: ${JNI_INCLUDE_DIRS}")
        include_directories(${JNI_INCLUDE_DIRS})
    else ()
        message(FATAL_ERROR "JNI Not Found!")
    endif ()
endif ()

# Build targets
if (OCR_OUTPUT STREQUAL "JNI")
    add_library(OcrOnnx SHARED ${OCR_COMPILE_CODE})
    target_compile_definitions(OcrOnnx PRIVATE __JNI__)
    target_link_libraries(OcrOnnx ${OnnxRuntime_LIBS} ${OpenCV_LIBS} ${JNI_LIBS})
elseif (OCR_OUTPUT STREQUAL "CLIB")
    add_library(OcrOnnx SHARED ${OCR_COMPILE_CODE})
    target_compile_definitions(OcrOnnx PRIVATE __CLIB__)
    target_link_libraries(OcrOnnx ${OnnxRuntime_LIBS} ${OpenCV_LIBS})
elseif (OCR_OUTPUT STREQUAL "BIN")
    add_executable(OcrOnnx ${OCR_COMPILE_CODE})
    target_compile_definitions(OcrOnnx PRIVATE __EXEC__)
    target_link_libraries(OcrOnnx ${OnnxRuntime_LIBS} ${OpenCV_LIBS})
endif ()

install(TARGETS OcrOnnx EXPORT OcrOnnx)

if (OCR_OUTPUT STREQUAL "CLIB")
    file(GLOB OCR_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
    install(FILES ${OCR_INCLUDE} DESTINATION include)
endif ()

# Windows Link CRT
if (OCR_BUILD_CRT STREQUAL "True")
    include(${CMAKE_CURRENT_SOURCE_DIR}/OcrCRTLinkage.cmake)
endif ()
