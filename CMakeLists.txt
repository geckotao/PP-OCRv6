cmake_minimum_required(VERSION 3.15)
project(OcrOnnx)

# ==========================================
# 0. 强制使用 UTF-8 编码编译 (解决 warning C4819 和中文注释报错)
# ==========================================
if (MSVC)
    add_compile_options("/utf-8")
endif()

# ==========================================
# 1. 强制使用静态 C++ 运行时 (/MT)，与下载的 -mt 包严格匹配
# ==========================================
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # 清理 CMake 默认可能带入的 /MD 标志，防止 LNK2038 运行时库不匹配错误
    foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        string(REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
    endforeach()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DUNICODE -D_UNICODE)

# ==========================================
# 2. 配置 ONNX Runtime 静态库
# ==========================================
set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime-v1.29.0-static-mt")
set(OnnxRuntime_INCLUDE_DIRS "${ONNXRUNTIME_DIR}/include")
set(OnnxRuntime_LIBS "${ONNXRUNTIME_DIR}/lib/onnxruntime.lib")
include_directories(${OnnxRuntime_INCLUDE_DIRS})

# Windows 静态链接 ONNX Runtime 必需的系统底层库
set(ONNX_SYSTEM_LIBS ws2_32.lib bcrypt.lib advapi32.lib userenv.lib ntdll.lib ole32.lib oleaut32.lib)

# ==========================================
# 3. 配置 OpenCV 静态库
# ==========================================
set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv-5.0.0-minimal")
set(OPENCV_INCLUDE_DIRS "${OPENCV_DIR}/include")
set(OPENCV_LIB_DIR "${OPENCV_DIR}/x64/vc17/staticlib")
include_directories(${OPENCV_INCLUDE_DIRS})

# 明确指定 OpenCV 核心模块 (请确保你的 staticlib 目录下这些文件名完全匹配)
set(OPENCV_CORE_LIBS
    "${OPENCV_LIB_DIR}/opencv_core500.lib"
    "${OPENCV_LIB_DIR}/opencv_imgproc500.lib"
    "${OPENCV_LIB_DIR}/opencv_imgcodecs500.lib"
    "${OPENCV_LIB_DIR}/opencv_videoio500.lib"
    "${OPENCV_LIB_DIR}/opencv_geometry500.lib"
    "${OPENCV_LIB_DIR}/opencv_flann500.lib"
)

# 自动收集所有第三方依赖库 (zlib, jpeg, png, tiff, openjp2, ittnotify 等)
file(GLOB OPENCV_3RD_LIBS "${OPENCV_LIB_DIR}/*.lib")
list(FILTER OPENCV_3RD_LIBS EXCLUDE REGEX "opencv_.*\\.lib$")

# OpenCV 静态链接额外需要的 Windows 系统库
set(OPENCV_SYSTEM_LIBS vfw32.lib comctl32.lib gdi32.lib uuid.lib shell32.lib user32.lib kernel32.lib)

# ==========================================
# 4. 项目源码
# ==========================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB OCR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# ⚠️ 如果你自己写了 main.cpp 放在 src 下，请取消下面这行的注释，防止与官方 main.cpp 冲突
# list(FILTER OCR_SRC EXCLUDE REGEX ".*main\\.cpp$")

# ==========================================
# 5. 构建目标 (独立 EXE)
# ==========================================
add_executable(OcrOnnx ${OCR_SRC})
target_compile_definitions(OcrOnnx PRIVATE __EXEC__)

# 链接所有库 (顺序很重要：核心库 -> 依赖库 -> 系统库)
target_link_libraries(OcrOnnx
    ${OnnxRuntime_LIBS}
    ${OPENCV_CORE_LIBS}       
    ${OPENCV_3RD_LIBS}        
    ${ONNX_SYSTEM_LIBS}       
    ${OPENCV_SYSTEM_LIBS}     
)