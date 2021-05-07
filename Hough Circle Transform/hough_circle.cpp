#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct Hough{
    int minCanny;
    int maxCanny;
    int rMin;
    int rMax;
    int tr;
    int stepTheta;
};

struct Cerchi{
    int R;
    Point c;
};

vector<Cerchi> HoughCircles(Mat, struct Hough);
void DrawCicles(Mat, vector<Cerchi>);

int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> min threshold Canny
    argv[3] -> max threshold Canny
    argv[4] -> min radius Hough Circles
    argv[5] -> max radius Hough Circles
    argv[6] -> threshold Hough Circles
    argv[7] -> step size Theta Hough Circles
    */

    Mat imgIn = imread(argv[1]);
    if(imgIn.empty()){
        cerr<<"Input image failed!"<<endl;
        return -1;
    }

    struct Hough h;
    h.minCanny = atoi(argv[2]);
    h.maxCanny = atoi(argv[3]);
    h.rMin = atoi(argv[4]);
    h.rMax = atoi(argv[5]);
    h.tr = atoi(argv[6]);
    h.stepTheta = atoi(argv[7]);

    String win = "Original image";
    namedWindow(win, 0);
    imshow(win, imgIn);
    waitKey(0);

    Mat img;
    cvtColor(imgIn, img, CV_BGR2GRAY);
    Canny(img, img, h.minCanny, h.maxCanny, 3);

    String winCanny = "Image after Canny";
    namedWindow(winCanny, 0);
    imshow(winCanny, img);
    waitKey(0);

    vector<Cerchi> L = HoughCircles(img, h);
    DrawCicles(imgIn, L);

    return 0;
}

void DrawCicles(Mat img, vector<Cerchi> L){

    for(int i=0; i<L.size(); i++)
        circle(img, L[i].c, L[i].R, Scalar(0,0,255), 2, 4);

    String win = "Image after Hough Trasform";
    namedWindow(win, 0);
    imshow(win, img);
    waitKey(0);

}

vector<Cerchi> HoughCircles(Mat img, struct Hough h){

    vector<Cerchi> L;
    int sizeAcc[] = {img.rows, img.cols, (h.rMax-h.rMin)};
    Mat acc = Mat(3, sizeAcc, CV_16U, Scalar(0));

    for(int y=0; y<img.rows; y++)
        for(int x=0; x<img.cols; x++)
            if(img.at<uchar>(y,x)==255)
                for(int r=1; r<(h.rMax-h.rMin); r++){
                    int R = r+h.rMin;
                    for(int k=0; k<360; k=k+h.stepTheta){

                        int a = y - round(R*cos(k*(M_PI/180)));
                        int b = x - round(R*sin(k*(M_PI/180)));

                        if(a>0 && a<img.rows && b>0 && b<img.cols){
                            acc.at<uchar>(a,b,r)++;
                        }
                    }
                }

    for(int a=0; a<img.rows; a++)
        for(int b=0; b<img.cols; b++)
            for(int r=0; r<(h.rMax-h.rMin); r++)
                if(acc.at<uchar>(a,b,r)>h.tr){
                    struct Cerchi in;
                    in.R = r + h.rMin;
                    in.c.y = a;
                    in.c.x = b;
                    L.push_back(in);
                }

    return L;

}
