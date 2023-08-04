#include"TRACK.h"

myTrack::myTrack(string str){
    path = str;
    VideoCapture cap(path);

    /*
    namedWindow("track", (640, 480));
    createTrackbar("H min", "track", &hmin, 360);
    createTrackbar("H max", "track", &hmax, 360);
    createTrackbar("S min", "track", &smin, 255);
    createTrackbar("S max", "track", &smax, 255);
    createTrackbar("V min", "track", &vmin, 255);
    createTrackbar("V max", "track", &vmax, 255);
    */

    while(waitKey(0) != 27){
        cap >> img;
        if(img.empty()) break;
        
    

        //flip(img, img, 1);
        //cvtColor(img, imgHSV, COLOR_BGR2HSV);
        //GaussianBlur(img, imgGuass, Size(3,3), 3,0);
        //Scalar lower(hmin, smin, vmin);
        //Scalar upper(hmax, smax, vmax);
        //inRange(imgHSV, lower, upper, mask);
         
        findOrange(img);


        imshow("img",img);
        //imshow("hsv", imgHSV);
        imshow("mask", mask);
        timeIndex++;
    }

}

Point myTrack::getContours(Mat img_mask){
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    //得到轮廓的点
    findContours(img_mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    //画出轮廓(画出所有的，可能会有噪音)
    //drawContours(img, contours, -1, Scalar(255, 0, 255), 4);
    //第三个参数为要绘制的第几个轮廓，-1为轮廓

    //vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());
    Point myPoint(0, 0);    //用来保存中心点


    //删除噪音
    for(int i = 0; i < contours.size(); i++){
        int area = contourArea(contours[i]);    //得到轮廓面积
        //过滤掉噪音  只有面积大于1000的轮廓才会被画出
        if(area > 1000){
            cout <<area << endl;//显示每个的面积
            float peri = arcLength(contours[i], true);  //得到轮廓周长，true表明闭合
            //得到相近曲线，即用特殊点几个点（n顶点之类）代替之前
            //approxPolyDP(contours[i], conPoly[i], 0.02*peri,true);
            //画出轮廓
            drawContours(img, contours, i, Scalar(255, 0, 0), 4);
            //画出相近曲线，减少性能消耗
            //drawContours(img, conPoly, i, Scalar(255, 0, 255), 4);
            //cout<< conPoly[i].size() << endl;   //面积会小很多

            //画一个矩形框柱他们
            boundRect[i] = boundingRect(contours[i]);
            //rectangle(img, boundRect[i].tl(),boundRect[i].br(), Scalar(0, 255, 0), 5);
            //tl和br分别得到左上和右下角坐标

            //得到中n心点
            if(area > 7700 || (area > 7799 && area < 8850)){
                myPoint.x = boundRect[i].x + boundRect[i].width / 2;
                myPoint.y = boundRect[i].y + boundRect[i].height / 2;

                r = max(boundRect[i].width, boundRect[i].height) / 2;
                circle(img, myPoint, r, Scalar(255,255,0),3);
                circle(img, myPoint, 6, Scalar(0,0,255),3);
            }
        }  
         

    }
        //画圆心历史路径
    int len = newPoints.size();
    for(int i = min(len, 7); i > 0; i--){
        circle(img, Point(newPoints[len - i][0], newPoints[len - i][1]), 6, Scalar(165,0,255),3);

    }
    return myPoint;
}

void myTrack::findOrange(Mat img){
        cvtColor(img, imgHSV, COLOR_BGR2HSV);
    Scalar lower(mycolor[0][0], mycolor[1][0], mycolor[2][0]);
    Scalar upper(mycolor[0][1], mycolor[1][1], mycolor[2][1]);
    inRange(imgHSV, lower, upper, mask);
    //GaussianBlur(mask, mask, Size(3,3), 3,0);
    
    Point myPoint = getContours(mask);

    //没检测到
    if(myPoint == Point(NULL, NULL)){
        newPoints.push_back({(int)state(0,0), (int)state(1,0)});
    }
    //检测到了
    else{
        kalmanUpd(start, myPoint.x, myPoint.y);
        start = true;
        newPoints.push_back({myPoint.x, myPoint.y});
    }
    kalmanPred(start);
    //画预测
    circle(img, Point(state(0,0), state(1,0)), 6, Scalar(0,255,164),3);
    circle(img, Point(state(0,0), state(1,0)), r, Scalar(0,255,164),3);

    //imshow("mask", mask);

}

void myTrack::kalmanUpd(bool start, float measX, float measY){
    //状态转移矩阵
    A << 1, 0, 1, 0,
         0, 1, 0, 1,
         0, 0, 1, 0,
         0, 0, 0, 1;
       
    H << 1, 0, 0, 0,
         0, 1, 0, 0;

    Q << 0.1, 0, 0, 0,
         0, 0.1, 0, 0,
         0, 0, 0.1, 0,
         0, 0, 0, 0.1;

    I << 1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1;
    R << 1, 1,
         1, 1;

    y << measX,
        measY;

    if (!start){
        state << 0,
                0,
                0,
                0;

        P << 1000, 0, 0, 0,
            0, 1000, 0, 0, 
            0, 0, 1000, 0,
            0, 0, 0, 1000;
        
    //   start = true;
    }
      
    S = H * P * H.transpose() + R; 
    
    cout << "S: " << endl << S << endl << endl;

    S1(0,0) = S(0,0);
    S1(1,0) = S(1,0);
    S1(0,1) = S(0,1);
    S1(1,1) = S(1,1);
        cout << "S1: " << endl << S1 << endl << endl;

     S2 = S1.inverse();
     Si(0,0) = S2(0,0);
     Si(0,1) = S2(0,1);
     Si(1,0) = S2(1,0);
     Si(1,1) = S2(1,1);
    cout << "S2: " << endl << S2 << endl << endl;
     
      K = P * H.transpose() * Si;
      cout << "P: " << endl << P << endl << endl;
      cout << "H_t: " << endl << H.transpose() << endl << endl;
      cout << "Si: " << endl << Si << endl << endl;
      cout << "k: " << endl << K << endl << endl;

      state = state + K * (y - H * state);
      P = (I - K * H) * P;
      cout << state << endl << endl;
    
    // cout << measX << endl;
}

void myTrack::kalmanPred(bool start){

    A << 1, 0, 1, 0,
         0, 1, 0, 1,
         0, 0, 1, 0,
         0, 0, 0, 1;

    Q << 100, 0, 0, 0,
         0, 100, 0, 0,
         0, 0, 100, 0,
         0, 0, 0, 100;

    state = A * state;
    P = A * P * A.transpose() + Q;
}


