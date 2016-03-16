/*
use this to compile
alias r='clear; export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig; c++ `Magick++-config --cxxflags --cppflags` -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -g -O2 -o binary main.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_stitching `Magick++-config --ldflags --libs`; ./binary; rm binary'
alias gdbIt='export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig; c++ `Magick++-config --cxxflags --cppflags` -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -g -o binary main.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_stitching `Magick++-config --ldflags --libs`; sudo gdb -tui binary; rm binary'
*/
//
//  main.cpp
//  BottomCaptcha
//
//  Created by localhost on 9/1/15.
//  Copyright (c) 2015 localhost. All rights reserved.
//

/**
* Error codes
* 1: getRGB has been given a nonexistent pixel
**/
#define root_Directory "/Users/localhost/Google Drive/I/Captchas/"
#define BACK_COLOR_REMOVE_CONST 1
#define COLOR_TOLERENCE 8
#define offsetTechniqueOffset 5
#define cropWhiteWeight 7
#define maxCropTilt .25

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <vector>
#include <Magick++.h> 
#include <array>
#include <cmath>
#include "neuralNet.cpp"
#include "rgb.cpp"
#include "lineClasses.cpp"


using namespace std;
using namespace cv;


/**
* Declaring methods
**/
rgb getRGB(Mat image, int col, int row);
bool isInImage(Mat image, int x, int y);

/**
* Attribtue elavluation methods
**/
void percentImageWhite(Mat image){
    int white = 0;
    int nonWhite = 0;
    for(int rows = 0; rows < image.rows; rows++){
        for(int cols = 0; cols < image.cols; cols++){
            if(getRGB(image,cols,rows).isEqual(rgb(255,255,255))){
                white++;
            }
            else{
                nonWhite++;
            }
        }
    }
    if(nonWhite != 0) cout << "Percent: " << (double)white/nonWhite << endl;
    else cout << "no nonwhite pixels" << endl;
}
// void testCharacteristic(Mat image){
//     char previous = 'y';
//     int addingDivider = 1;
//     char testVal = showAndWait(image);
//     cout << (int)testVal << ":" << (int)previous << endl;
//     if(testVal != previous){
//         addingDivider*=10;
//     }
//     if(testVal == 'y'){
//         cout << "Current is " << BACK_COLOR_REMOVE_CONST;
//         BACK_COLOR_REMOVE_CONST+=1.0/addingDivider;
//         cout << ". Now moving to " << BACK_COLOR_REMOVE_CONST << endl;
//         previous = 'y';
//     }
//     else{
//         cout << "Current is " << BACK_COLOR_REMOVE_CONST;
//         BACK_COLOR_REMOVE_CONST-=1.0/addingDivider;
//         cout << ". Now moving to " << BACK_COLOR_REMOVE_CONST;
//         previous = 'n';
//     }
// }

/**
* Drawing Methods
**/
void drawLine(Mat &image, int x1, int y1, int x2, int y2, int thickness, Scalar theScalar){
    // if(!(isInImage(image, x1,y2) && isInImage(image, x2,y2))){
    //     cout << "LINE POINTS OUTSIDE IMAGE BOUNDS!!!!" << endl;
    //     return;
    // }
    int lineType = 8;
    CvPoint pt1;
    CvPoint pt2;
    pt1.x = x1;
    pt1.y = y1;
    pt2.x = x2;
    pt2.y = y2;
    line( image, pt1, pt2, theScalar, thickness, lineType, 0 );
}
void drawLine(Mat &image, int x1, int y1, int x2, int y2, int thickness){
    drawLine(image, x1, y1, x2, y2, 2, Scalar( 0, 0, 255 ));
}
void drawLine(Mat &image, int x1, int y1, int x2, int y2){
    drawLine(image, x1, y1, x2, y2, 2);
}
void drawLine(Mat &image, myLine line){
    drawLine(image, 0, line.b, image.cols-1,
        (int)(line.m*(image.cols-1)+line.b));
}
void drawLine(Mat &image, myLine line, Scalar color){
    drawLine(image, 0, line.b, image.cols-1,
        (int)(line.m*(image.cols-1)+line.b), 2, color);
}
/**
* Image file manipulation
**/
bool ifGif(char * fileurl) {
    if(fileurl[25] == 'f' && fileurl[24] == 'i' && fileurl[23] == 'g'){
        return true;
    }
    return false;
}
string getUrl(string urlValue){
    string url = root_Directory;
    url += urlValue;
    return url;
}
vector <string> loadImageUrls(string directory, string imageLocationWithinBase) {
    vector <string> files;
    // image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    struct dirent * ent;
    DIR * dir;
    directory+=imageLocationWithinBase;
    if ((dir = opendir(directory.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (string(ent -> d_name).length() == 26) {
                string fileName = "";
                for (int i = 0; i < 26; i++) {
                    fileName += ent -> d_name[i];
                }
                files.push_back(fileName);
            }
        }
        closedir(dir);
    }
    return files;
}
void convertGifsToPngs(){
    vector <string> fileUrls = loadImageUrls(root_Directory, "Gifs/");
    for (int i = 0; i < fileUrls.size(); i++) {
        string url = fileUrls[i];
        cout << url <<  endl;
        Magick::Image img;
        img.read(string(root_Directory) + string("Gifs/") + fileUrls[i]);
        img.write(string(root_Directory) + string("Pngs/") +fileUrls[i].substr(0,url.length()-4) + ".png");
    }
}
Mat basicThreshold(int thresh, Mat image){
    cvtColor(image, image, CV_BGR2GRAY);
    threshold(image, image, thresh, 255, 0);
    return image;
}
Mat readImage(string imageUrl){
    return imread(imageUrl, CV_LOAD_IMAGE_COLOR);
}

/**
* Helper methods
**/
rgb getRGB(Mat image, int col, int row){
    rgb returnRGB;
    if(row < image.rows && col < image.cols && row >= 0 && col >=0){
        returnRGB.r = image.at<cv::Vec3b>(row,col)[2];
        returnRGB.g = image.at<cv::Vec3b>(row,col)[1];
        returnRGB.b = image.at<cv::Vec3b>(row,col)[0];
    }
    else{
        cout << "getRGB has been given a nonexistent pixel, (" + to_string(col) + ", " + to_string(row) + ")!!!!!!!!!\n\n";
        return rgb();
    }
    return returnRGB;
}
//simple backgroundColor helper method. Checks if value is already in an array
int checkIfInArr(int Val, int arr[], int terminationVal){
    for(int i = 0; true; i++){
        if(arr[i] == terminationVal){
            return 0;
        }
        else if(arr[i] == Val){
            return i;
        }
    }
    return 0;
}
int checkIfInArr(rgb Val, rgb arr[], int terminationVal, int maxVal){
    for(int i = 0; i < maxVal; i++){
        if(arr[i].r == terminationVal && arr[i].g == terminationVal && arr[i].b == terminationVal){
            return -1;
        }
        else if(arr[i].isClose(Val)){
            return i;
        }
    }
    return -1;
}
bool isInImage(Mat image, int x, int y){
    bool test = x < image.cols && y < image.rows && x >= 0 && y>= 0;
    if(!test){
        cout << "PIXEL iS NOT IN IMAGE! ERROR IN ISINIMAGE" << endl;
    }
    return test;
}

/**
* Gui implementation
**/
char showAndWait(Mat image, string title){
    imshow(title, image);
    char key = waitKey();
    return key;
}
char showAndWait(Mat image){
    return showAndWait(image, "test");
}


/**
* Dealing with Background
**/
//Sort by changes with 
//List all colors in an image. Sort by changes how the reponse is ordered
// Sort by: 0 -> occurence counter; 1 -> intensity
listColorRet listColors(Mat image, bool justBorder, int sortBy){
    rgb *rawRGBValues;
    int maxPlace;
    if(justBorder){
        maxPlace = 4*image.cols+4;
        // create array for storing border pixel values and initialize all to -1
        rawRGBValues = new rgb[maxPlace];
        rawRGBValues[maxPlace].r = 1000;
        rawRGBValues[maxPlace].g = 1000;
        rawRGBValues[maxPlace].b = 1000;
        //List rawRGBValues into array where it goes, top, bottom, left, right, top, bottom, left, right...
        rgb topCornerPixel = getRGB(image,0,0);
        for(int i = 0; i < image.cols; i++){
            if(i < image.cols){
                //top
                rawRGBValues[4*i] = getRGB(image,i,0);
                //bottom
                rawRGBValues[4*i+1] = getRGB(image,i,image.rows-1);
            }
            if(i < image.rows){
                //left
                rawRGBValues[4*i+2] = getRGB(image,0,i);
                //right
                rawRGBValues[4*i+3] = getRGB(image,image.cols-1,i);
            }
        }
    }
    else{
        maxPlace = image.cols*image.cols;
        rawRGBValues = new rgb[maxPlace];
        rawRGBValues[maxPlace].r = 1000;
        rawRGBValues[maxPlace].g = 1000;
        rawRGBValues[maxPlace].b = 1000;
        for(int y = 0; y < image.rows; y++){
            for(int x = 0; x < image.cols; x++){
                rawRGBValues[y*image.cols+x] = getRGB(image,x,y);
            }
        }
    }
    //Turn array of values into value + counters for how many times used.
    rgb *rgbValues = new rgb[image.cols];
    int occurenceCounter[image.cols];
    for(int i = 0; i < image.cols; i++) occurenceCounter[i]=0;
    int sizeOfRgbValues = 0;
    for(int i = 0; i < maxPlace; i++){
        int isInArr = checkIfInArr(rawRGBValues[i],rgbValues,1000,sizeOfRgbValues);
        if(isInArr != -1){
            occurenceCounter[isInArr]++;
        }
        else{
            if(rawRGBValues[i].r != -1){
                rgbValues[sizeOfRgbValues]=rawRGBValues[i];
                sizeOfRgbValues++;
            }
        }
    }
    if(sortBy == 0){
        //Sorts by occurences
        for(int i = 0; i < sizeOfRgbValues-1; i++){
            if(occurenceCounter[i+1] > occurenceCounter[i]){
                rgb carryVal = rgbValues[i];
                rgbValues[i] = rgbValues[i+1];
                rgbValues[i+1] = carryVal;

                int carryInt = occurenceCounter[i];
                occurenceCounter[i] = occurenceCounter[i+1];
                occurenceCounter[i+1] = carryInt;
                i = -1;
            }
        }
    }
    else if(sortBy == 1){
        //Sorts by intensity
        for(int i = 0; i < sizeOfRgbValues-1; i++){
            if(rgbValues[i+1].b + rgbValues[i+1].g + rgbValues[i+1].r
             > rgbValues[i].r + rgbValues[i].g + rgbValues[i].b){
                rgb carryVal = rgbValues[i];
                rgbValues[i] = rgbValues[i+1];
                rgbValues[i+1] = carryVal;

                int carryInt = occurenceCounter[i];
                occurenceCounter[i] = occurenceCounter[i+1];
                occurenceCounter[i+1] = carryInt;
                i = -1;
            }
        }
    }
    return listColorRet(rgbValues, sizeOfRgbValues,occurenceCounter);
}
void removeSpecificColor(Mat &image, rgb color){
    for(int row = 0; row < image.rows; row++){
        for(int col = 0; col < image.cols; col++){
            if(color.r == (int)image.at<Vec3b>(row,col)[2]
                && color.g == (int)image.at<Vec3b>(row,col)[1]
                && color.b == (int)image.at<Vec3b>(row,col)[0]){
                image.at<Vec3b>(row,col) = Vec3b(255,255,255);
            }
        }
    }
}
void removeCloseColors(Mat &image, rgb color){
    for(int row = 0; row < image.rows; row++){
        for(int col = 0; col < image.cols; col++){
            if(color.isClose(rgb((int)image.at<Vec3b>(row,col)[2],
                (int)image.at<Vec3b>(row,col)[1],
                (int)image.at<Vec3b>(row,col)[0]))){
                image.at<Vec3b>(row,col) = Vec3b(255,255,255);
            }
        }
    }
}
int findDarkestColorIndex(rgb *arr, int maxSize){
    if(maxSize == 0){
        cout << "ERROR ERROR!!!! IN FUNCTION FINDDARKESTCOLORINDEX!!!!!" << endl;
        exit(0);
    }
    if(maxSize == 1) return 0;
    int lowestSum = arr[0].sum();
    int lowestSumIndex = 0;
    for(int i = 1; i < maxSize; i++){
        int sum = arr[i].sum();
        if(sum < lowestSum){
            lowestSum = sum;
            lowestSumIndex = i;
        }
    }
    return lowestSumIndex;
}
//Remove background color and surrounding objects
void removeBackgroundColor(Mat &image){
    listColorRet colors = listColors(image, true, 1);
    int lowestColorIndex = findDarkestColorIndex(colors.rgbValues,colors.rgbValuesSize);
    for(int i = 0; i < colors.rgbValuesSize; i++){
        if(i == lowestColorIndex){
            if(i == colors.rgbValuesSize-1){
                return;
            }
            else{
                i++;
            }
        }
        removeCloseColors(image,colors.rgbValues[i]);
    }
}

void removeUpperLeft(Mat &image){
            rectangle( image,
           Point( 0, 0 ),
           Point( 130, 17),
           Scalar( 255, 255, 255 ),
           -1,
           8 );
}
void removeAllEdges(Mat &image){
    Mat grayscale;
    cvtColor(image, grayscale, CV_BGR2GRAY);
    listColorRet colors = listColors(image, true, 1);
    for(int i = 0; i < colors.rgbValuesSize; i++){
        removeCloseColors(image, colors.rgbValues[i]);
    }
}

/**
* Crop using offset technique
**/
int valueOfLine(Mat image, myLine testLine){
    int sum = 0;
    for(int x = 0; x < image.cols; x++){
        int y = testLine.m*x+testLine.b;
        if((y >= 0 && y < image.rows) && !(getRGB(image,x,y).isEqual(rgb(255,255,255)))){
            sum++;
        }
    }
    return sum;
}
int valueOfVerticalLine(Mat image, int xIntercept){
    int sum = 0;
    for(int y = 0; y < image.rows; y++){
        if((y >= 0 && y < image.rows) && !(getRGB(image,xIntercept,y).isEqual(rgb(255,255,255)))){
            sum++;
        }
    }
    return sum;
}
int valueOfLineUsingSwitching(Mat image, myLine testLine){
    int sum = 0;
    bool previous = getRGB(image,0,testLine.b).isEqual(rgb(255,255,255));
    for(int x = 0; x < image.cols; x++){
        int y = testLine.m*x+testLine.b;
        bool currentTestBool = (y >= 0 && y < image.rows) &&
            !(getRGB(image,x,y).isEqual(rgb(255,255,255)));
        if(currentTestBool != previous){
            previous = currentTestBool;
            sum++;
        }
    }
    return sum;
}
myLine lineOfLeastResistanceOffset(Mat image, int offset){
    int bestScore = 0;
    int bestOffsetScore = 0;
    myLine bestLine;
    myLine bestOffsetLine;
    for(int b = 1; b < image.rows-1+(image.rows+maxCropTilt*image.cols); b++){
        for(double m = -.5; m < .5; m+=.01){
            myLine testLine =  myLine(b,m);
            myLine offsetLine = myLine(testLine.b+offset,
                testLine.m);
            int value = valueOfLine(image,testLine);
            int offsetValue = valueOfLine(image,offsetLine);
            int score = value - cropWhiteWeight*offsetValue;
            if(score > bestScore){
                bestLine = testLine;
                bestScore = value;
                bestOffsetScore = offsetValue;
                bestOffsetLine = offsetLine;
            }
        }
    }
    // drawLine(image, bestLine);
    // drawLine(image, bestOffsetLine, Scalar( 255, 0, 0 ));
    return bestOffsetLine;
}
void perspectiveCrop(Mat &image, CvPoint pt1, CvPoint pt2, CvPoint pt3, CvPoint pt4){
    // Define the destination image
    Mat quad = cv::Mat::zeros(pt4.y-pt1.y, pt4.x-pt1.x, CV_8UC3);

    // Corners of the destination image
    vector<cv::Point2f> quad_pts;
    quad_pts.push_back(Point2f(0, 0));
    quad_pts.push_back(Point2f(quad.cols, 0));
    quad_pts.push_back(Point2f(quad.cols, quad.rows));
    quad_pts.push_back(Point2f(0, quad.rows));

    vector<cv::Point2f> corners;
    corners.push_back(Point2f(pt1.x, pt1.y));
    corners.push_back(Point2f(pt3.x, pt3.y));
    corners.push_back(Point2f(pt4.x, pt4.y));
    corners.push_back(Point2f(pt2.x, pt2.y));

    // Get transformation matrix
    Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);

    // Apply perspective transformation
    warpPerspective(image, image, transmtx, quad.size());
}
void cropUsingOffsetTechnique(Mat &image){
    myLine topLine = lineOfLeastResistanceOffset(image, -offsetTechniqueOffset); 
    myLine bottomLine = lineOfLeastResistanceOffset(image, offsetTechniqueOffset);
    int leftBound = 0;
    for(; leftBound<image.cols && valueOfVerticalLine(image, leftBound) == 0; leftBound++);
    int rightBound = image.cols-1;
    for(; rightBound>0 && valueOfVerticalLine(image, rightBound) == 0; rightBound--);
    CvPoint upperLeft = cvPoint(leftBound, topLine.b+topLine.m*leftBound);
    CvPoint lowerLeft = cvPoint(leftBound, bottomLine.b+bottomLine.m*leftBound);
    CvPoint upperRight = cvPoint(rightBound, topLine.b+topLine.m*rightBound);
    CvPoint lowerRight = cvPoint(rightBound, bottomLine.b+bottomLine.m*rightBound);
    Mat imageWithLines = image.clone();
    // drawLine(imageWithLines, topLine, Scalar(0,0,255));
    // drawLine(imageWithLines, bottomLine, Scalar(0,0,255));
    // drawLine(imageWithLines, leftBound, 0, leftBound, image.cols, 2, Scalar( 0, 0, 255 ));
    // drawLine(imageWithLines, rightBound, 0, rightBound, image.rows, 2, Scalar( 0, 0, 255 ));
    imshow("Pre Perspective Crop",imageWithLines);

    perspectiveCrop(image,upperLeft,lowerLeft,upperRight,lowerRight);
    // image.copyTo(image(Rect(x1,y1,x1+x2,y1+y2));
}


/**
* Seperate individual characters
**/
//Seperates all of the non touching elements, within tolerance
Mat edges(Mat image){
    int edgeThresh = 1;
    int lowThreshold = 1;
    int const max_lowThreshold = 100;
    int ratio = 3;
    int kernel_size = 3;
    Mat detected_edges;
    Mat dst;

    /// Reduce noise with a kernel 3x3
    blur( image, detected_edges, Size(3,3) );

    /// Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

    /// Using Canny's output as a mask, we display our result
    dst = Scalar::all(0);
    image.copyTo( dst, detected_edges);
    return dst;
}
bool colClear(Mat image, int col, int tolerance){
    int counter = 0;
    for(int i = 0; i < image.rows; i++){
        if(!getRGB(image,col,i).isEqual(rgb(255,255,255))) counter++;
    }
    return counter < tolerance;
}
vector<Mat> seperateVertical(Mat &image){

    vector<Mat> retArr;
    for(int i = 0; i < image.cols; i++){
        if(colClear(image,i,1)){
            drawLine(image, i, 0,i, image.rows-1);
            i+=5;
        }
    }
    seperateVertical(image);
    return retArr;
}


int main(int argc, const char * argv[]) {
    // convertGifsToPngs();
    vector <string> fileUrls = loadImageUrls(root_Directory, "Pngs/");
    for(int i = 0; i < fileUrls.size(); i++){
        Mat image;
        string imageUrl = string(root_Directory) + string("Pngs/") + fileUrls[i];
        image = readImage(imageUrl);
        removeUpperLeft(image);
        removeBackgroundColor(image);
        removeAllEdges(image);
        cropUsingOffsetTechnique(image);
        trainSeperationNet(image);
        Letters *imageLetters = new Letters(Mat image);

        // image = edges(image);


        // for(int i = 0; i < image.cols; i++){
        //     cout << image.cols << endl;
        //     Mat testImage;
        //     image.copyTo(testImage);
        //     drawLine(testImage, i, 0, i, image.rows, 2);
        //     char response = showAndWait(testImage, "Post Perspective Crop");
        //     if(response == ' '){
        //         cout << i << endl;
        //     }
        // }
    }
}
