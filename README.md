修改自RapidOcrOnnx源码，支持PaddleOCR最新发布的PP-OCRv6模型二、下载本项目C++源码

目录结构如下

\OcrOnnx
├── onnxruntime-v1.29.0-static-mt\
│           ├── include\
│           └──  lib\
├──  opencv-5.0.0-minimal\
│           ├── include\opencv2\ 
│           └──x64\vc17\staticlib\
├── include\
├── src\
├── models\
└── CMakeLists.txt

编译

打开 VS 2022 的 x64 本机工具命令提示符并进入代码CMakeLists.txt目录

cmake -G "Visual Studio 17 2022" -A x64 -T v143 -S . -B build

cmake --build build --config Release

