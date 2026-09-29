# 修改自RapidOcrOnnx，支持最新PP-OCRv6模型。


一、下载onnxruntime静态编译包（也可自已折腾）

https://github.com/csukuangfj/onnxruntime-libs/releases/download/v1.29.0/onnxruntime-win-x64-static_lib-MT-Release-1.29.0.tar.bz2

解压到onnxruntime-v1.29.0-static-mt目录

二、下载OpenCV  https://github.com/opencv/opencv/archive/refs/tags/5.0.0.zip    进行静态编译包制作
<pre>
1、解压到 opencv-5.0.0目录

2、用VS 2022 的 x64 本机工具命令静态编译
cd opencv-5.0.0
md opencv-5.0.0-minimal
  
3、CMake Configure 配置生成 VS 工程

cmake -G "Visual Studio 17 2022" -A x64 -T v143 -S.  -B build ^
-DCMAKE_INSTALL_PREFIX=%OPENCV_INSTALL% ^
-DBUILD_SHARED_LIBS=OFF ^
-DBUILD_WITH_STATIC_CRT=ON ^
-DBUILD_opencv_world=OFF ^
-DBUILD_TESTS=OFF ^
-DBUILD_PERF_TESTS=OFF ^
-DBUILD_EXAMPLES=OFF ^
-DBUILD_opencv_apps=OFF ^
-DBUILD_opencv_python2=OFF ^
-DBUILD_opencv_python3=OFF ^
-DBUILD_opencv_java=OFF ^
-DBUILD_opencv_js=OFF ^
-DBUILD_opencv_dnn=OFF ^
-DBUILD_opencv_ml=OFF ^
-DBUILD_opencv_objdetect=OFF ^
-DBUILD_opencv_photo=OFF ^
-DBUILD_opencv_stitching=OFF ^
-DBUILD_opencv_video=OFF ^
-DBUILD_opencv_features2d=OFF ^
-DBUILD_opencv_calib3d=OFF ^
-DBUILD_opencv_gapi=OFF ^
-DBUILD_opencv_flann=ON ^
-DWITH_IPP=OFF ^
-DWITH_ITT=OFF ^
-DWITH_ADE=OFF ^
-DWITH_FFMPEG=OFF ^
-DWITH_GSTREAMER=OFF ^
-DWITH_MSMF=OFF ^
-DWITH_DIRECTX=OFF ^
-DWITH_TIFF=OFF ^
-DWITH_WEBP=OFF ^
-DWITH_OPENJPEG=OFF ^
-DWITH_JASPER=OFF ^
-DWITH_OPENEXR=OFF ^
-DWITH_VTK=OFF ^
-DBUILD_ZLIB=ON ^
-DBUILD_JPEG=ON ^
-DBUILD_PNG=ON

4、编译 Release 版本（-j8 使用 8 线程编译）
cmake --build build --config Release -j 8
  
5、执行 install，输出头文件 + 静态库到 指定目录opencv-5.0.0-minimal
cmake --install build --config Release --prefix opencv-5.0.0-minimal

</pre>

三、下载本项目C++源码

目录结构如下
#
<pre>
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
</pre>
#
四、编译

打开 VS 2022 的 x64 本机工具命令提示符并进入代码CMakeLists.txt目录


cmake -G "Visual Studio 17 2022" -A x64 -T v143 -S . -B build

cmake --build build --config Release

