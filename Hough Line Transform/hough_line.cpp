#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct Hough{
    int minCanny;
    int maxCanny;
    int tr;
    int theta_step;
};

struct Lines{

    Point p1;
    Point p2;
};

vector<Lines> HoughLines(Mat , struct Hough);
void DrawLines(Mat, vector<Lines>);


int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> min threshold Canny
    argv[3] -> max threshold Canny
    argv[4] -> min threshold Hough Lines
    argv[5] -> max threshold Hough Lines
    */

    Mat imgIn = imread(argv[1]);
    if(imgIn.empty()){
        cerr<<"Input image failed!"<<endl;
        return -1;
    }
    struct Hough h;
    h.minCanny = atoi(argv[2]);
    h.maxCanny = atoi(argv[3]);
    h.tr = atoi(argv[4]);
    h.theta_step = atoi(argv[5]);

    String win = "Original image";
    namedWindow(win, 0);
    imshow(win, imgIn);
    waitKey(0);

    Mat img = imgIn.clone();
    cvtColor(imgIn, img, CV_BGR2GRAY);
    Canny(img, img, h.minCanny, h.maxCanny, 3);

    String winCanny = "Image after Canny";
    namedWindow(winCanny, 0);
    imshow(winCanny, img);
    waitKey(0);

    vector<Lines> L = HoughLines(img, h);
    DrawLines(imgIn, L);

    return 0;

}

void DrawLines(Mat img, vector<Lines> lines){

    for(int i=0; i<lines.size(); i++){

        line(img, lines[i].p1, lines[i].p2, Scalar(0,0,255), 1, 8);
    }

    String win = "Image after Hough Trasform";
    namedWindow(win, 0);
    imshow(win, img);
    waitKey(0);
}

vector<Lines> HoughLines(Mat img, struct Hough h){

    vector<Lines> L;

    int pMax = ceil(sqrt(img.rows*img.rows + img.cols*img.cols));
    int acc[2*pMax][180] = {{0}};

    for(int y=0; y<img.rows; y++)
        for(int x=0; x<img.cols; x++)
            if(img.at<uchar>(y,x)==255)
                for(int k=0; k<180; k++){

                    int p = round(x*cos(k*(M_PI/180)) + y*sin(k*(M_PI/180)) ) + pMax;
                    acc[p][k]++;
                }


    for(int p=0; p<2*pMax; p++)
        for(int k=0; k<180; k++)
            if(acc[p][k]>h.tr){
                float rho = p-pMax, theta = k * (M_PI/180);
                Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = round(x0 + 1000*(-b));
                pt1.y = round(y0 + 1000*(a));
                pt2.x = round(x0 - 1000*(-b));
                pt2.y = round(y0 - 1000*(a));
                struct Lines in;
                in.p1 = pt1;
                in.p2 = pt2;
                L.push_back(in);
            }
    return L;


}
