#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>

using namespace std;
using namespace cv;

struct Cluster{
    int n;
    Vec3b mean;
    Vec3f sum;
};

Mat Kmeans(Mat, int);
int AssignPixel(Mat, Mat, vector<Cluster>&);
float EuclideanDistance(Vec3b, Vec3b);
void MeanRecompute(vector<Cluster>&);

int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> number of clusters
    */

    Mat img = imread(argv[1]);
    int k   = atoi(argv[2]);

    if(img.empty()){
        cerr<<"Input image failed!"<<endl;
        return -1;
    }

    String win = "Original Image";
    namedWindow(win, 0);
    imshow(win, img);
    waitKey(0);

    Mat out;
    out = Kmeans(img, k);

    String winKmeans = "Image after KMEANS";
    namedWindow(winKmeans, 0);
    imshow(winKmeans, out);
    waitKey(0);

    return 0;
}

float EuclideanDistance(Vec3b p1, Vec3b p2){

    return sqrt( pow(p1[0]-p2[0],2) +
                 pow(p1[1]-p2[1],2) +
                 pow(p1[2]-p2[2],2) );
}

int AssignPixel(Mat img, Mat out, vector<Cluster>& c){

    int cont = 0;

    for(int y=0; y<img.rows; y++)
        for(int x=0; x<img.cols; x++){
            float distMin = 450;
            int indexMin;
            for(int k=0; k<c.size(); k++){
                float dist = EuclideanDistance(img.at<Vec3b>(y,x), c[k].mean);
                if(dist<distMin){
                    distMin=dist;
                    indexMin = k;
                }
            }
            if(indexMin!= out.at<uchar>(y,x)){
                cont++;
            }
            out.at<uchar>(y,x) = indexMin;
            c[indexMin].n++;
            for(int m=0; m<3; m++) c[indexMin].sum[m]+= img.at<Vec3b>(y,x)[m];

        }

    return cont;
}


void MeanRecompute(vector<Cluster>& c){

    for(int i=0; i<c.size(); i++){
        for(int m=0; m<3; m++) c[i].mean[m] = c[i].sum[m]/c[i].n;
        for(int m=0; m<3; m++) c[i].sum[m] = 0;
        c[i].n = 0;

    }

}

Mat Kmeans(Mat img, int k){

    srand(time(NULL));
    vector<Cluster> c;
    Mat out = Mat(img.rows, img.cols, CV_8U, Scalar(k+1));

    for(int i=0; i<k; i++){
        int y = rand()%img.rows;
        int x = rand()%img.cols;

        Cluster in;
        in.mean = img.at<Vec3b>(y,x);
        in.n=0;
        in.sum = Vec3f(0,0,0);
        c.push_back(in);
        }

    AssignPixel(img, out, c);
    MeanRecompute(c);

    int conta = 0;
    do{
    conta = AssignPixel(img, out, c);
    MeanRecompute(c);
    
    }while(conta > (img.rows * img.cols) * 0.001 );

    Mat outRGB = Mat(img.rows, img.cols, CV_8UC3);

    for(int y=0; y<img.rows; y++)
        for(int x=0; x<img.cols; x++){
            int indice = out.at<uchar>(y,x);
            outRGB.at<Vec3b>(y,x) = c[indice].mean;
        }

    return outRGB;
}





