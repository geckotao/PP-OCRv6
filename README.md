修改自RapidOcrOnnx源码，支持PaddleOCR最新发布的PP-OCRv6模型，取消GPU参数

输出结果为纯识别结果且不生成图片及文本，放便调用




一、下载onnxruntime与OpenCV静态编译包（也可自已折腾）

https://github.com/RapidAI/OnnxruntimeBuilder/releases/download/1.23.2/onnxruntime-v1.23.2-windows-vs2022-x64-static-mt.7z

https://github.com/RapidAI/OpenCVBuilder/releases/download/4.11.0/opencv-4.11.0-windows-vs2022-x64-mt.7z

二、下载本项目C++源码


目录结构如下

\OcrOnnx\
 ├── onnxruntime-v1.23.2-windows-vs2022-x64-static-mt\
 │    ├── include\
 │    └── lib\
 │         └── onnxruntime.lib
 ├── opencv-4.11.0-windows-vs2022-x64-mt\
 │    ├── include\
 │    └── x64\
 │         └── vc17\
 │              └── staticlib\   <-- (里面包含 opencv_core4110.lib 及 zlib, libjpeg 等所有 .lib 文件)
 ├── include\
 ├── src\
 └── CMakeLists.txt

 三、编译，如放在C盘

打开 VS 2022 的 x64 本机工具命令提示符

cd C:\OcrOnnx

cmake -G "Visual Studio 17 2022" -A x64 -T v143 -S . -B build

cmake --build build --config Release



