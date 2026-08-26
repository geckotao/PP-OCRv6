#include "OcrLite.h"
#include "OcrUtils.h"

OcrLite::OcrLite() {}

OcrLite::~OcrLite() {}

void OcrLite::setNumThread(int numOfThread) {
    dbNet.setNumThread(numOfThread);
    angleNet.setNumThread(numOfThread);
    crnnNet.setNumThread(numOfThread);
}

bool OcrLite::initModels(const std::string &detPath, const std::string &clsPath,
                         const std::string &recPath, const std::string &keysPath) {
    dbNet.initModel(detPath);
    angleNet.initModel(clsPath);
    crnnNet.initModel(recPath, keysPath);
    return true;
}

cv::Mat makePadding(cv::Mat &src, const int padding) {
    if (padding <= 0) return src;
    cv::Scalar paddingScalar = {255, 255, 255};
    cv::Mat paddingSrc;
    cv::copyMakeBorder(src, paddingSrc, padding, padding, padding, padding, cv::BORDER_ISOLATED, paddingScalar);
    return paddingSrc;
}

OcrResult OcrLite::detect(const char *path, const char *imgName,
                          const int padding, const int maxSideLen,
                          float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle) {
    std::string imgFile = getSrcImgFilePath(path, imgName);
    cv::Mat originSrc = imread(imgFile, cv::IMREAD_COLOR);
    int originMaxSide = (std::max)(originSrc.cols, originSrc.rows);
    int resize = (maxSideLen <= 0 || maxSideLen > originMaxSide) ? originMaxSide : maxSideLen;
    resize += 2 * padding;
    cv::Rect paddingRect(padding, padding, originSrc.cols, originSrc.rows);
    cv::Mat paddingSrc = makePadding(originSrc, padding);
    ScaleParam scale = getScaleParam(paddingSrc, resize);
    
    return detect(path, imgName, paddingSrc, paddingRect, scale,
                  boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
}

OcrResult OcrLite::detect(const cv::Mat &mat, int padding, int maxSideLen, float boxScoreThresh, float boxThresh,
                          float unClipRatio, bool doAngle, bool mostAngle) {
    cv::Mat originSrc = mat;
    int originMaxSide = (std::max)(originSrc.cols, originSrc.rows);
    int resize = (maxSideLen <= 0 || maxSideLen > originMaxSide) ? originMaxSide : maxSideLen;
    resize += 2 * padding;
    cv::Rect paddingRect(padding, padding, originSrc.cols, originSrc.rows);
    cv::Mat paddingSrc = makePadding(originSrc, padding);
    ScaleParam scale = getScaleParam(paddingSrc, resize);
    
    return detect(NULL, NULL, paddingSrc, paddingRect, scale,
                  boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
}

std::vector<cv::Mat> OcrLite::getPartImages(cv::Mat &src, std::vector<TextBox> &textBoxes) {
    std::vector<cv::Mat> partImages;
    for (size_t i = 0; i < textBoxes.size(); ++i) {
        cv::Mat partImg = getRotateCropImage(src, textBoxes[i].boxPoint);
        partImages.emplace_back(partImg);
    }
    return partImages;
}

OcrResult OcrLite::detect(const char *path, const char *imgName,
                          cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                          float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle) {
    double startTime = getCurrentTime();
    
    std::vector<TextBox> textBoxes = dbNet.getTextBoxes(src, scale, boxScoreThresh, boxThresh, unClipRatio);
    double endDbNetTime = getCurrentTime();
    double dbNetTime = endDbNetTime - startTime;

    std::vector<cv::Mat> partImages = getPartImages(src, textBoxes);
    std::vector<Angle> angles = angleNet.getAngles(partImages, path, imgName, doAngle, mostAngle);

    for (size_t i = 0; i < partImages.size(); ++i) {
        if (angles[i].index == 1) {
            partImages.at(i) = matRotateClockWise180(partImages[i]);
        }
    }

    std::vector<TextLine> textLines = crnnNet.getTextLines(partImages, path, imgName);

    std::vector<TextBlock> textBlocks;
    for (size_t i = 0; i < textLines.size(); ++i) {
        std::vector<cv::Point> boxPoint = std::vector<cv::Point>(4);
        int padding = originRect.x;
        boxPoint[0] = cv::Point(textBoxes[i].boxPoint[0].x - padding, textBoxes[i].boxPoint[0].y - padding);
        boxPoint[1] = cv::Point(textBoxes[i].boxPoint[1].x - padding, textBoxes[i].boxPoint[1].y - padding);
        boxPoint[2] = cv::Point(textBoxes[i].boxPoint[2].x - padding, textBoxes[i].boxPoint[2].y - padding);
        boxPoint[3] = cv::Point(textBoxes[i].boxPoint[3].x - padding, textBoxes[i].boxPoint[3].y - padding);
        
        TextBlock textBlock{boxPoint, textBoxes[i].score, angles[i].index, angles[i].score,
                            angles[i].time, textLines[i].text, textLines[i].charScores, textLines[i].time,
                            angles[i].time + textLines[i].time};
        textBlocks.emplace_back(textBlock);
    }

    double fullTime = getCurrentTime() - startTime;

    // 仅拼接纯文本结果
    std::string strRes;
    for (auto &textBlock : textBlocks) {
        strRes.append(textBlock.text);
        strRes.append("\n");
    }

    return OcrResult{dbNetTime, textBlocks, cv::Mat(), fullTime, strRes};
}