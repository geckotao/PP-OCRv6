#include "OcrLite.h"
#include "OcrUtils.h"
#include "getopt.h"
#include "version.h"
#ifdef _WIN32
#include <windows.h>
#endif

static struct option long_options[] = {
        {"models",       required_argument, nullptr, 'd'},
        {"det",          required_argument, nullptr, '1'},
        {"cls",          required_argument, nullptr, '2'},
        {"rec",          required_argument, nullptr, '3'},
        {"keys",         required_argument, nullptr, '4'},
        {"image",        required_argument, nullptr, 'i'},
        {"numThread",    required_argument, nullptr, 't'},
        {"padding",      required_argument, nullptr, 'p'},
        {"maxSideLen",   required_argument, nullptr, 's'},
        {"boxScoreThresh", required_argument, nullptr, 'b'},
        {"boxThresh",    required_argument, nullptr, 'o'},
        {"unClipRatio",  required_argument, nullptr, 'u'},
        {"doAngle",      required_argument, nullptr, 'a'},
        {"mostAngle",    required_argument, nullptr, 'A'},
        {"version",      no_argument,       nullptr, 'v'},
        {"help",         no_argument,       nullptr, 'h'},
        {nullptr, 0,                       nullptr, 0}
};

void printHelp(FILE *file, const char *programName) {
    fprintf(file, " ------- Required Parameters -------\n");
    fprintf(file, "-d --models: models directory.\n");
    fprintf(file, "-1 --det: model file name of det.\n");
    fprintf(file, "-2 --cls: model file name of cls.\n");
    fprintf(file, "-3 --rec: model file name of rec.\n");
    fprintf(file, "-4 --keys: keys file name.\n");
    fprintf(file, "-i --image: path of target image.\n\n");

    fprintf(file, " ------- Optional Parameters -------\n");
    fprintf(file, "-t --numThread: value of numThread(int), default: 4\n");
    fprintf(file, "-p --padding: value of padding(int), default: 50\n");
    fprintf(file, "-s --maxSideLen: Long side of picture for resize(int), default: 1024\n");
    fprintf(file, "-b --boxScoreThresh: value of boxScoreThresh(float), default: 0.5\n");
    fprintf(file, "-o --boxThresh: value of boxThresh(float), default: 0.3\n");
    fprintf(file, "-u --unClipRatio: value of unClipRatio(float), default: 1.6\n");
    fprintf(file, "-a --doAngle: Enable(1)/Disable(0) Angle Net, default: Enable\n");
    fprintf(file, "-A --mostAngle: Enable(1)/Disable(0) Most Possible AngleIndex, default: Enable\n");
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printHelp(stderr, argv[0]);
        return -1;
    }
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::string modelsDir, modelDetPath, modelClsPath, modelRecPath, keysPath;
    std::string imgPath, imgDir, imgName;
    int numThread = 4;
    int padding = 50;
    int maxSideLen = 1024;
    float boxScoreThresh = 0.5f;
    float boxThresh = 0.3f;
    float unClipRatio = 1.6f;
    bool doAngle = true;
    int flagDoAngle = 1;
    bool mostAngle = true;
    int flagMostAngle = 1;

    int opt;
    int optionIndex = 0;
  
    while ((opt = getopt_long(argc, argv, "d:1:2:3:4:i:t:p:s:b:o:u:a:A:vh", long_options, &optionIndex)) != -1) {
        switch (opt) {
            case 'd':
                modelsDir = optarg;
                break;
            case '1':
                modelDetPath = modelsDir + "/" + optarg;
                break;
            case '2':
                modelClsPath = modelsDir + "/" + optarg;
                break;
            case '3':
                modelRecPath = modelsDir + "/" + optarg;
                break;
            case '4':
                keysPath = modelsDir + "/" + optarg;
                break;
            case 'i':
                imgPath = optarg;
                break;
            case 't':
                numThread = (int) strtol(optarg, NULL, 10);
                break;
            case 'p':
                padding = (int) strtol(optarg, NULL, 10);
                break;
            case 's':
                maxSideLen = (int) strtol(optarg, NULL, 10);
                break;
            case 'b':
                boxScoreThresh = (float) atof(optarg);
                break;
            case 'o':
                boxThresh = (float) atof(optarg);
                break;
            case 'u':
                unClipRatio = (float) atof(optarg);
                break;
            case 'a':
                flagDoAngle = (int) strtol(optarg, NULL, 10);
                doAngle = flagDoAngle == 1;
                break;
            case 'A':
                flagMostAngle = (int) strtol(optarg, NULL, 10);
                mostAngle = flagMostAngle == 1;
                break;
            case 'v':
                printf("%s\n", VERSION);
                return 0;
            case 'h':
                printHelp(stdout, argv[0]);
                return 0;
            default:
                printf("other option %c :%s\n", opt, optarg);
        }
    }

    // 优化路径拆分：确保 imgDir 末尾带有斜杠，防止底层拼接时丢失斜杠
    imgDir = ".";
    imgName = imgPath;
    size_t pos = imgPath.find_last_of("/\\");
    if (pos != std::string::npos) {
        imgDir = imgPath.substr(0, pos + 1); // 保留末尾的斜杠
        imgName = imgPath.substr(pos + 1);
    }

    // 在调用 OCR 前，先测试图片是否能被 OpenCV 正常读取
    cv::Mat testImg = cv::imread(imgPath, cv::IMREAD_COLOR);
    if (testImg.empty()) {
        fprintf(stderr, "Error: 无法读取图片！请检查图片路径是否正确。\n");
        fprintf(stderr, "尝试读取的路径: %s\n", imgPath.c_str());
        return -1;
    }
    testImg.release(); // 释放测试用的内存

    OcrLite ocrLite;
    ocrLite.setNumThread(numThread);

    if (!ocrLite.initModels(modelDetPath, modelClsPath, modelRecPath, keysPath)) {
        fprintf(stderr, "Error: 模型加载失败！请检查路径和文件名是否正确。\n");
        return -1;
    }

    OcrResult result = ocrLite.detect(imgDir.c_str(), imgName.c_str(), padding, maxSideLen,
                                      boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);

    if (result.textBlocks.empty()) {
        fprintf(stderr, "Warning: 未识别到任何文字。请检查图片中是否包含文字。\n");
    } else {
        // 成功时仅输出纯识别结果
        for (const auto &block: result.textBlocks) {
            printf("%s\n", block.text.c_str());
        }
    }

    return 0;
}