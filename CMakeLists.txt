cmake_minimum_required(VERSION 3.15)
project(OcrOnnx)

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
# 2. 配置 ONNX Runtime 静态库 (使用相对路径)
# ==========================================
set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime-v1.23.2-windows-vs2022-x64-static-mt")
set(OnnxRuntime_INCLUDE_DIRS "${ONNXRUNTIME_DIR}/include")
set(OnnxRuntime_LIBS "${ONNXRUNTIME_DIR}/lib/onnxruntime.lib")

include_directories(${OnnxRuntime_INCLUDE_DIRS})

# Windows 静态链接 ONNX Runtime 必需的系统底层库
set(ONNX_SYSTEM_LIBS ws2_32.lib bcrypt.lib advapi32.lib userenv.lib ntdll.lib ole32.lib oleaut32.lib)

# ==========================================
# 3. 配置 OpenCV 静态库 
# ==========================================
set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv-4.11.0-windows-vs2022-x64-mt")
set(OPENCV_INCLUDE_DIRS "${OPENCV_DIR}/include")
# RapidAI 的包将静态库统一放在此目录下
set(OPENCV_LIB_DIR "${OPENCV_DIR}/x64/vc17/staticlib")

include_directories(${OPENCV_INCLUDE_DIRS})

# 自动收集 staticlib 目录下的所有 .lib 文件 
# (一次性链接 opencv 核心库以及内置的 zlib, libjpeg, libpng 等所有静态依赖，避免手动逐个指定)
file(GLOB OPENCV_STATIC_LIBS "${OPENCV_LIB_DIR}/*.lib")

if (NOT OPENCV_STATIC_LIBS)
    message(FATAL_ERROR "未找到 OpenCV 静态库文件，请检查路径是否正确: ${OPENCV_LIB_DIR}")
else()
    message(STATUS "成功找到 ${CMAKE_MATCH_COUNT} 个 OpenCV 静态库文件")
endif()

# ==========================================
# 4. 项目源码
# ==========================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB OCR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# ==========================================
# 5. 构建目标 (独立 EXE)
# ==========================================
add_executable(OcrOnnx ${OCR_SRC})
target_compile_definitions(OcrOnnx PRIVATE __EXEC__)

# 链接所有库
target_link_libraries(OcrOnnx 
    ${OnnxRuntime_LIBS} 
    ${OPENCV_STATIC_LIBS}  # 链接所有 OpenCV 及其第三方静态依赖
    ${ONNX_SYSTEM_LIBS}    # 链接 ONNX 所需的 Windows 系统库
)
