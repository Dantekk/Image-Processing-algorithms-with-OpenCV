#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct Cluster{
    Vec3b mean;
    Vec3f sum;
    int n;
};

Mat Heng(Mat);
float EuclideanDistance(Vec3b, Vec3b);
vector<Vec3b> FindDistMax(Mat);
void AssignPixel(Mat, Mat, vector<Cluster> &);
tuple<float,Point> ComputeDeX(Mat, Mat, vector<Cluster> );
void ReCoumputeMedie(vector<Cluster> &);

float EuclideanDistance(Vec3b p1, Vec3b p2){

return sqrt(pow(p1[0]-p2[0],2) +
            pow(p1[1]-p2[1],2) +
            pow(p1[2]-p2[2],2) );
}

vector<Vec3b> FindDistMax(Mat img){

    vector<Vec3b> pointsMax;
    Point p1;
    Point p2;

    for(int y=0; y<img.rows; y++)
    for(int x=0; x<img.cols; x++)
    {
            float distMax=0;
            for(int k=0; k<img.cols; k++){
                float dist = EuclideanDistance(img.at<Vec3b>(y,x), img.at<Vec3b>(y,k));
                if(dist>distMax){
                    distMax = dist;
                    p1.y = y;
                    p1.x = x;
                    p2.y = y;
                    p2.x = k;
                }
            }
            for(int y1=y+1; y1<img.rows; y1++)
            for(int x1=0; x1<img.cols; x1++){
                float dist = EuclideanDistance(img.at<Vec3b>(y,x), img.at<Vec3b>(y1,x1));
                if(dist>distMax){
                    distMax = dist;
                    p1.y = y;
                    p1.x = x;
                    p2.y = y1;
                    p2.x = x1;
                }
            }
    
    }

    Vec3b pMax1 = img.at<Vec3b>(p1.y,p1.x);
    Vec3b pMax2 = img.at<Vec3b>(p2.y,p2.x);

    pointsMax.push_back(pMax1);
    pointsMax.push_back(pMax2);
    return pointsMax;
}

void AssignPixel(Mat img, Mat out, vector<Cluster> &c){

    for(int y=0; y<img.rows; y++)
    for(int x=0; x<img.cols; x++){
        float distMin=455;
        int indexMin;
        for(int k=0; k<c.size(); k++){
            float dist = EuclideanDistance(img.at<Vec3b>(y,x), c[k].mean);
            if(dist<distMin){
                distMin = dist;
                indexMin = k;
            }
        }
        out.at<uchar>(y,x) = indexMin;
        for(int m=0; m<3; m++) c[indexMin].sum[m]+= img.at<Vec3b>(y,x)[m];
        c[indexMin].n++;
    }

}

tuple<float,Point> ComputeDeX(Mat img, Mat out, vector<Cluster> c){

    tuple<float,Point> DX;
    Point p;
    float distMax=0;
    for(int y=0; y<img.rows; y++)
    for(int x=0; x<img.cols; x++){
            int index = out.at<uchar>(y,x);
            float dist = EuclideanDistance(img.at<Vec3b>(y,x), c[index].mean);
            if(dist>distMax){
                distMax = dist;
                p.y = y;
                p.x = x;
            }
        }


    get<0>(DX) = distMax;
    get<1>(DX) = p;

    return DX;

}


void ReCoumputeMedie(vector<Cluster> &c){

for(int i=0; i<c.size(); i++){
    for(int j=0; j<3; j++) c[i].mean[j] = c[i].sum[j]/c[i].n;
    Vec3f new_vec(0.0,0.0,0.0);
    c[i].sum = new_vec;
    c[i].n = 0;
}
}

Mat Heng(Mat img){

vector<Vec3b> pointsMax;
pointsMax = FindDistMax(img);

Mat out = Mat(img.rows, img.cols, CV_8U, Scalar(0));
vector<Cluster> c;

for(int i=0; i<2; i++){
    struct Cluster cluster;
    cluster.mean = pointsMax[i];
    Vec3f new_vec(0.0,0.0,0.0);
    cluster.sum = new_vec;
    cluster.n=0;
    c.push_back(cluster);
}
int it=0;
bool check;

do
{
    AssignPixel(img, out, c);
    ReCoumputeMedie(c);
    tuple<float,Point> DX;
    DX = ComputeDeX(img, out, c);
    float d = get<0>(DX);
    Point X = get<1>(DX);

    float distMedie=0;
    int n=0;
    for(int i=0; i<c.size(); i++)
        for(int j=i+1; j<c.size(); j++){
            distMedie+= EuclideanDistance(c[i].mean, c[j].mean);
            n++;
        }
    float q = distMedie/n;

    if(d>q/2){
            struct Cluster cluster;
            cluster.mean = img.at<Vec3b>(X.y,X.x);
            Vec3f new_vec(0.0,0.0,0.0);
            cluster.sum = new_vec;
            cluster.n=0;
            c.push_back(cluster);
            check = true;

    }else check = false;

}while(check==true && it<12);

Mat outRGB = Mat(img.rows, img.cols, CV_8UC3);

for(int y=0; y<img.rows; y++)
    for(int x=0; x<img.cols; x++){
        int index = out.at<uchar>(y,x);
        outRGB.at<Vec3b>(y,x) = c[index].mean;
    }

return outRGB;

}

int main(int argc, char **argv){

    //argv[1] -> input image

    Mat img = imread(argv[1]);
    if(img.empty()){
        cerr<<"Input image failed!"<<endl;
        return -1;
    }

    String winIm = "Original Image";
    namedWindow(winIm, 0);
    imshow(winIm, img);
    waitKey(0);

    Mat output;
    output = Heng(img);

    String winKmeans = "Image after Heng clustering";
    namedWindow(winKmeans, 0);
    imshow(winKmeans, output);
    waitKey(0);

    return 0;
}
