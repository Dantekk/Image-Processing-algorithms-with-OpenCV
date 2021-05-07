#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct Dati{

    Mat mag;
    Mat dir;
};

Mat CannyDetector(Mat, int, int);
void DirModCalculate(Mat, struct Dati *);
Mat NonMaxSupp(Mat, int, struct Dati *);
void Hysteresis(Mat, Mat, int, struct Dati);

int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> min threshold
    argv[3] -> max threshold
    */

    Mat img = imread(argv[1], 0);
    if(img.empty()){
        cerr<<"Input image failed!"<<endl;
        return -1;
    }
    int sMin = atoi(argv[2]);
    int sMax = atoi(argv[3]);

    String win = "Original image";
    namedWindow(win, 0);
    imshow(win, img);

    Mat out;
    out = CannyDetector(img, sMin, sMax);
    return 0;

}

Mat CannyDetector(Mat img, int sMin, int sMax){

    Mat out = Mat(img.rows, img.cols, CV_8U, Scalar(0));
   // GaussianBlur(img, img, Size(3,3), 1.4);

    struct Dati c;
    DirModCalculate(img, &c);
    out = NonMaxSupp(img, sMax, &c);

    String winNonMax = "Image after non max supp";
    namedWindow(winNonMax, 0);
    imshow(winNonMax, out);
    waitKey(0);

    Hysteresis(img, out, sMin, c);

    String winHysteresis = "Image after Hysteresis";
    namedWindow(winHysteresis, 0);
    imshow(winHysteresis, out);
    waitKey(0);

    return out;
}

void Hysteresis(Mat img, Mat out, int sMin, struct Dati c){

    bool imgChanged = false;
    int it=0;

    while(imgChanged==false){
        for(int y=0; y<img.rows; y++)
            for(int x=0; x<img.cols; x++){
                if(y<2 || y>img.rows-2 || x<2 || x>img.cols-2) continue;
                if(out.at<uchar>(y,x)==0) continue;

                float currDir = c.dir.at<float>(y,x);
                out.at<uchar>(y,x) = 30;

                if(currDir>22.5 && currDir<=67.5){

                    if(out.at<uchar>(y-1,x+1)!=30 &&
                       c.mag.at<float>(y-1,x+1) > sMin &&
                       c.dir.at<float>(y-1,x+1) > 22.5 &&
                       c.dir.at<float>(y-1,x+1) <= 67.5 &&
                       c.mag.at<float>(y-1,x+1) > c.mag.at<float>(y-2,x) &&
                       c.mag.at<float>(y-1,x+1) > c.mag.at<float>(y,x+2)){
                            imgChanged = true;
                            out.at<uchar>(y-1,x+1) = 255;

                      }

                    if(out.at<uchar>(y+1,x-1)!=30 &&
                       c.mag.at<float>(y+1,x-1) > sMin &&
                       c.dir.at<float>(y+1,x-1) > 22.5 &&
                       c.dir.at<float>(y+1,x-1) <= 67.5 &&
                       c.mag.at<float>(y+1,x-1) > c.mag.at<float>(y,x-2) &&
                       c.mag.at<float>(y+1,x-1) > c.mag.at<float>(y+2,x)){
                            imgChanged = true;
                            out.at<uchar>(y+1,x-1) = 255;

                      }

                }

                else if(currDir>67.5 && currDir<=112.5){

                    if(out.at<uchar>(y,x-1)!=30 &&
                       c.dir.at<float>(y,x-1) > 67.5 &&
                       c.dir.at<float>(y,x-1) <= 112.5 &&
                       c.mag.at<float>(y,x-1) > sMin &&
                       c.mag.at<float>(y,x-1) > c.mag.at<float>(y-1,x-1) &&
                       c.mag.at<float>(y,x-1) > c.mag.at<float>(y+1,x-1)
                      ){
                        imgChanged = true;
                        out.at<uchar>(y,x-1) = 255;

                      }

                    if(out.at<uchar>(y,x+1)!=30 &&
                       c.dir.at<float>(y,x+1) > 67.5 &&
                       c.dir.at<float>(y,x+1) <= 112.5 &&
                       c.mag.at<float>(y,x+1) > sMin &&
                       c.mag.at<float>(y,x+1) > c.mag.at<float>(y-1,x+1) &&
                       c.mag.at<float>(y,x+1) > c.mag.at<float>(y+1,x+1)
                      ){
                        imgChanged = true;
                        out.at<uchar>(y,x+1) = 255;

                      }

                }

                else if(currDir>112.5 && currDir<=157.5){

                    if(out.at<uchar>(y-1,x-1)!=30 &&
                       c.dir.at<float>(y-1,x-1) > 112.5 &&
                       c.dir.at<float>(y-1,x-1) <= 157.5 &&
                       c.mag.at<float>(y-1,x-1) > sMin &&
                       c.mag.at<float>(y-1,x-1) > c.mag.at<float>(y-2,x) &&
                       c.mag.at<float>(y-1,x-1) > c.mag.at<float>(y,x-2)
                      ){
                        imgChanged = true;
                        out.at<uchar>(y-1,x-1) = 255;

                      }

                    if(out.at<uchar>(y+1,x+1)!=30 &&
                       c.dir.at<float>(y+1,x+1) > 112.5 &&
                       c.dir.at<float>(y+1,x+1) <= 157.5 &&
                       c.mag.at<float>(y+1,x+1) > sMin &&
                       c.mag.at<float>(y+1,x+1) > c.mag.at<float>(y,x+2) &&
                       c.mag.at<float>(y+1,x+1) > c.mag.at<float>(y+2,x)
                      ){
                        imgChanged = true;
                        out.at<uchar>(y+1,x+1) = 255;

                      }

                }

                else{

                    if(out.at<uchar>(y-1,x)!=30 &&
                       (c.dir.at<float>(y-1,x) < 22.5 ||
                       c.dir.at<float>(y-1,x) > 157.5) &&
                       c.mag.at<float>(y-1,x) > sMin &&
                       c.mag.at<float>(y-1,x) > c.mag.at<float>(y-1,x-1) &&
                       c.mag.at<float>(y-1,x) > c.mag.at<float>(y-1,x+1)
                      ){
                        imgChanged = true;
                        out.at<uchar>(y-1,x) = 255;

                      }

                    if(out.at<uchar>(y+1,x)!=30 &&
                       (c.dir.at<float>(y+1,x) <22.5 ||
                       c.dir.at<float>(y+1,x) > 157.5) &&
                       c.mag.at<float>(y+1,x) > sMin &&
                       c.mag.at<float>(y+1,x) > c.mag.at<float>(y+1,x-1) &&
                       c.mag.at<float>(y+1,x) > c.mag.at<float>(y+1,x+1)
                      ){
                        imgChanged = true;
                        out.at<uchar>(y+1,x) = 255;

                      }

                }


            }
    }

    for(int y=0; y<img.rows; y++)
            for(int x=0; x<img.cols; x++)
                if(out.at<uchar>(y,x) == 30) out.at<uchar>(y,x) = 255;

}


Mat NonMaxSupp(Mat img, int sMax, struct Dati *c){

    Mat out = Mat(img.rows, img.cols, CV_8U, Scalar(0));

    for(int y=1; y<img.rows-1; y++)
        for(int x=1; x<img.cols-1; x++){
            float currDir = atan(c->dir.at<float>(y,x)) * (180/M_PI);
            while(currDir<0) currDir+=180;
            c->dir.at<float>(y,x) = currDir;

            if(c->mag.at<float>(y,x) < sMax) continue;
            bool check = true;
            if(currDir>22.5 && currDir<=67.5){
                if(c->mag.at<float>(y,x) < c->mag.at<float>(y-1,x-1) ||
                   c->mag.at<float>(y,x) < c->mag.at<float>(y+1,x+1)) check = false;

            }

            else if(currDir>67.5 && currDir<=112.5){
                if(c->mag.at<float>(y,x) < c->mag.at<float>(y-1,x) ||
                   c->mag.at<float>(y,x) < c->mag.at<float>(y+1,x)) check = false;

            }

            else if(currDir>112.5 && currDir<=157.5){
                if(c->mag.at<float>(y,x) < c->mag.at<float>(y+1,x-1) ||
                   c->mag.at<float>(y,x) < c->mag.at<float>(y-1,x+1)) check = false;

            }

            else{
                if(c->mag.at<float>(y,x) < c->mag.at<float>(y,x-1) ||
                   c->mag.at<float>(y,x) < c->mag.at<float>(y,x+1)) check = false;
            }

            if(check) out.at<uchar>(y,x) = 255;


        }

    return out;

}

void DirModCalculate(Mat img, struct Dati *c){

    Mat dx, dy, dx2, dy2;
    Sobel(img, dx, CV_32F, 1, 0);
    Sobel(img, dy, CV_32F, 0, 1);
    divide(dy, dx, c->dir);
    multiply(dx, dx, dx2);
    multiply(dy, dy, dy2);
    sqrt(dx2+dy2, c->mag);

}
