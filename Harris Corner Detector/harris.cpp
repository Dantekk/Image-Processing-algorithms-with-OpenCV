#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct Harris{
    Mat dx2;
    Mat dy2;
    Mat dxdy;
};

vector<tuple<float, int, int>> HarrisFunc(Mat, float);
void GradientCompute(Mat, struct Harris *);
void DrawCorner(Mat , vector<tuple<float, int, int>>, String );
void NonMaxSupp(vector<tuple<float, int, int>>& );

int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> threshold
    */
    Mat imgIn = imread(argv[1]);
    float tr = atoi(argv[2]);

    if(imgIn.empty()){
        cerr<<"Input image failed!"<<endl;
        return -1;
    }

    String win = "Original Image";
    namedWindow(win, 0);
    imshow(win, imgIn);
    waitKey(0);

    vector<tuple<float, int, int>> L;
    L = HarrisFunc(imgIn, tr);
    DrawCorner(imgIn, L, "Image after non max supp");

    return 0;
}

vector<tuple<float, int, int>> HarrisFunc(Mat imgIn, float tr){

    vector<tuple<float, int, int>> L;
    struct Harris h;
    Mat img;
    cvtColor(imgIn, img, CV_BGR2GRAY);
    GradientCompute(img, &h);
    GaussianBlur(h.dx2, h.dx2, Size(3,3), 1.4);
    GaussianBlur(h.dy2, h.dy2, Size(3,3), 1.4);
    GaussianBlur(h.dxdy, h.dxdy, Size(3,3), 1.4);

    for(int y=1; y<img.rows-1; y++)
        for(int x=1; x<img.cols-1; x++){

            Mat cov = Mat(2, 2, CV_32F, Scalar(0));
            for(int i=-1; i<2; i++)
                for(int j=-1; j<2; j++){
                    cov.at<float>(0,0)+= h.dx2.at<float>(y+i, x+j);
                    cov.at<float>(0,1)+= h.dxdy.at<float>(y+i, x+j);
                    cov.at<float>(1,0)+= h.dxdy.at<float>(y+i, x+j);
                    cov.at<float>(1,1)+= h.dy2.at<float>(y+i, x+j);
                }

            Mat av;
            eigen(cov, av);
            float R, detM, k, traceM;
            detM = av.at<float>(0,0) * av.at<float>(0,1);
            k = 0.04;
            traceM = av.at<float>(0,0) + av.at<float>(0,1);
            R = detM - k *(traceM * traceM);
            if(R > tr) L.push_back(make_tuple(R, y, x));

        }

    DrawCorner(imgIn, L, "Image after Corner Response");
    NonMaxSupp(L);
    return L;

}

void NonMaxSupp(vector<tuple<float, int, int>>& L){

    sort(L.begin(), L.end(), greater<tuple<float, int, int>>());

    for(int i=0; i<L.size(); i++){
        int y1 = get<1>(L[i]);
        int x1 = get<2>(L[i]);
        for(int j=i+1; j<L.size(); j++){
            int y2 = get<1>(L[j]);
            int x2 = get<2>(L[j]);
            if(abs(y1-y2)<=4 && abs(x1-x2)<=4){
                L.erase(L.begin()+j);
                j--;
            }
        }
    }

}

void DrawCorner(Mat imgIn, vector<tuple<float, int, int>> L, String winMex){

    Mat img = imgIn.clone();

    for(int i=0; i<L.size(); i++){
        Point p;
        p.y = get<1>(L[i]);
        p.x = get<2>(L[i]);
        rectangle(img, p, p, Scalar(0,0,255), 3, 4);
        //circle(img, Point(p.x,p.y), 2, Scalar(0,0,255),1,4);
    }

    namedWindow(winMex, 0);
    imshow(winMex, img);
    waitKey(0);

}

void GradientCompute(Mat img, struct Harris *h){

    Mat dx, dy;
    Sobel(img, dx, CV_32F, 1, 0);
    Sobel(img, dy, CV_32F, 0, 1);
    multiply(dx, dx, h->dx2);
    multiply(dy, dy, h->dy2);
    multiply(dx, dy, h->dxdy);

}
