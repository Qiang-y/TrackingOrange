#pragma once
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class myTrack{
private:
    string path;
    Mat img, imgHSV, mask;
    float fps = 15; // 设置视频播放的帧速率
    int delay = (int)(1000 / fps);   
    // int hmin=0, smin, vmin;
    // int hmax=360, smax = 255, vmax = 255;
    float speedX = 0, speedY = 0;
    float r =0;
    int timeIndex = 0;
    float mytime = (1000 / 15);
    float prespeedX = 0, prespeedY = 0;
    vector<vector<int>> prePoint;
    vector<vector<int>> newPoints;  //记录中心点路劲
    vector<vector<int>> mycolor = {{8, 360}, {140, 255}, {55, 255}};    //橘子颜色范围

public:
    myTrack(string str);
    Point getContours(Mat img_mask);
    void findOrange(Mat img);
};