#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat DistanceTrasform(Mat, int);
Mat tresholding(Mat, int);

int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> threshold
    */

    Mat img = imread(argv[1], 0);
    if(img.empty()){
        cerr<<"Input image failed!"<<endl;
        return -1;
    }

    int tr = atoi(argv[2]);

    String win = "Original Image";
    namedWindow(win, 0);
    imshow(win, img);
    waitKey(0);

    Mat out;
    out = DistanceTrasform(img, tr);

    String winDT = "Original after DT";
    namedWindow(winDT, 0);
    imshow(winDT, out);
    waitKey(0);

    return 0;
}

Mat DistanceTrasform(Mat img, int tr){

    Mat out;
    out = tresholding(img, tr);

    String winTr = "Image after tresholding";
    namedWindow(winTr, 0);
    imshow(winTr, out);
    waitKey(0);

    for(int y=1; y<img.rows-1; y++)
        for(int x=1; x<img.cols-1; x++)
            if(out.at<uchar>(y,x)==255){

                int tmp[4];
                tmp[0] = out.at<uchar>(y, x-1);
                tmp[1] = out.at<uchar>(y-1, x-1);
                tmp[2] = out.at<uchar>(y-1, x);
                tmp[3] = out.at<uchar>(y-1, x+1);

                int minVal = tmp[0];
                for(int k=1; k<4; k++)
                    if(tmp[k]<minVal) minVal = tmp[k];

                out.at<uchar>(y,x) = minVal+1;
            }

    for(int y=img.rows-1; y>0; y--)
        for(int x=img.cols-1; x>0; x--)
            if(out.at<uchar>(y,x)==255){

                int tmp[5];
                tmp[0] = out.at<uchar>(y, x);
                tmp[1] = out.at<uchar>(y, x+1)   +1;
                tmp[2] = out.at<uchar>(y+1, x+1) +1;
                tmp[3] = out.at<uchar>(y+1, x)   +1;
                tmp[4] = out.at<uchar>(y+1, x-1) +1;

                int minVal = tmp[0];
                for(int k=1; k<5; k++)
                    if(tmp[k]<minVal) minVal = tmp[k];

                out.at<uchar>(y,x) = minVal;
            }


    return out;
}

Mat tresholding(Mat img, int tr){

    Mat out = Mat(img.rows, img.cols, CV_8U, Scalar(0));

    for(int y=0; y<img.rows; y++)
        for(int x=0; x<img.cols; x++)
            if(img.at<uchar>(y,x)<tr) out.at<uchar>(y,x) = 255;

    return out;
}
