#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>      

using namespace std;
using namespace cv;

/*
I decided to declare these variables in a global struct in order not to infer on 
the stack size as the algorithm it requires a lot of recursive calls.
*/
struct{
    Mat visited; //matrix that keeps track of the pixels classified in the various regions
    Mat img;      //image matrix
    float tr;
    vector<Vec3b> colors;//matrix containing the mean of all regions
}rg;

void RegionGrowing(int);
void DrawRG();

void DrawRG(){

    /*
    I scroll through the rg.visited matrix which contains for each pixel the value that 
    corresponds to its belonging class. I use this value as an index of the vector rg.colors, 
    which will contain the average of that class. Finally we assign this value to the output 
    matrix, that is rg.img.
    */
    for(int i=0; i<rg.visited.rows;i++)
        for(int j=0; j<rg.visited.cols;j++){
            int index = rg.visited.at<uchar>(i,j); 
            Vec3b mean = rg.colors[index-1];
            rg.img.at<Vec3b>(i,j)=mean;
        }

    String nameWinRG = "Image after Region Growing";
    namedWindow(nameWinRG,0);
    imshow(nameWinRG,rg.img);
    waitKey(0);
}


void RegionGrowing(int ID){
   
    srand (time(NULL));
    Point p;
    
    do{
    int NonVisited=0; 

    /*
    The next double-for is to find the seed:
    That is, it scrolls the rg.visited matrix that keeps track of the pixels considered. 
    Whenever it finds a pixel not yet considered, that is, whose value is equal to 0, 
    it inserts its coordinates in the vector tmp.
    */
    vector<Point> tmp;
    for(int i=0; i<rg.visited.rows; i++)
            for(int j=0; j<rg.visited.cols; j++){
                if(rg.visited.at<uchar>(i,j)==0){
                    Point p;
                    p.y=i;
                    p.x=j;
                    tmp.push_back(p);
                }
            }

    NonVisited = tmp.size();
    /*
    BASIC CASE OF THE APPEAL:
    If there are no more pixels to consider, i.e. in the rg matrix visited there 
    is not even a pixel equal to 0, then it means that all the pixels of the image 
    have been classified.
    */
    if(NonVisited==0) break;

    /*
    If there is at least one pixel not yet considered, 
    then we randomly take one that will become the new seed.
    */
    NonVisited = rand() % NonVisited;
    p = tmp[NonVisited]; //at this time p contains the coordinates of the new seed

    /*
    This queue is a usage data structure, which we will need to keep track of the region's growth arrest condition.
    */
    queue<Point> region_points;
    /*
    The variables sum and n keep track of the sum of the pixel values in the region 
    and the number of pixels in the region, respectively, as they grow. 
    They are variables of use for the calculation of the average.
    */
    Vec3f sum;
    int n;
    //Initialization sum to the value of the seed and n = 1
    sum = rg.img.at<Vec3b>(p.y,p.x); 
    n=1;

    rg.visited.at<uchar>(p.y,p.x)=ID; //I trace the pixel seed considered.

        do{
             //for each pixel I consider an 8-connected kernel
             for(int i=-1; i<2; i++)
                for(int j=-1; j<2; j++){
                    
                    //padding control
                    if(p.y+i <0 || p.y+i>rg.img.rows-1 || p.x+j<0 || p.y+j>rg.img.cols-1)
                            continue;

                    //if the pixel in question has not yet been classified to any image class
                    if(rg.visited.at<uchar>(p.y+i,p.x+j)==0){

                        //I calculate the rgb average of the region
                        Vec3f mean = sum;
                        for(int i=0;i<3;i++) mean[i]=mean[i]/n;

                        //I calculate the Euclidean distance between the considered pixel and the region average
                        float dist[3];
                        for(int i=0;i<3;i++) dist[i] = rg.img.at<Vec3b>(p.y+i,p.x+j)[i] - mean[i];

                        float val = sqrt(pow(dist[0],2)+pow(dist[1],2)+pow(dist[2],2));

                        /*
                        If the value of this pixel is part of the range [average-rg.trmin, average + rg.trmax], 
                        then it is part of the ID-th class of the image.
                        */
                        if(val < rg.tr){
                       
                            rg.visited.at<uchar>(p.y+i,p.x+j)=ID;
                            
                            //I update the variables of use of the mean
                            for(int i=0;i<3;i++) sum[i]+= rg.img.at<Vec3b>(p.y+i,p.x+j)[i];
                            n++;

                            Point in;
                            in.y=p.y+i;
                            in.x=p.x+j;
                            /*
                            I enter the coordinates of the newly assigned pixel in the region_points queue. 
                            In this way, we are able to consider the pixels in its surroundings as well.
                            */
                            region_points.push(in);
                        }

                    }

                }
            /*
            If the queue is not empty, it means that there are still pixels to consider to try 
            to grow the region. Otherwise, if the queue is empty, it means it cannot expand and 
            exits the while loop.
            */
            if(!region_points.empty()){
                p = region_points.front();//the new pixel to be considered will be the first element of the queue
                region_points.pop();
            }

        }while(!region_points.empty());

        
        /*
        I insert the rgb average of the newly created class in a vector. It will be used for image output.
        */

        Vec3b col;
        for(int i=0; i<3; i++) col[i] = int(sum[i]/n);
        rg.colors.push_back(col);
        ID++;

     }while(1);
}

int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> threshold RG
    */

    rg.img = imread(argv[1]);
    rg.tr = atof(argv[2]);

    /*
    In this type of algorithm, compared to edge detectors, we can use a Gaussian filter 
    of greater power, as it is useful for equidistributing the pixel values.
    */
    GaussianBlur(rg.img,rg.img, Size(11,11),1,1); 
    
    String nameWin = "Image after gaussian filter";
    namedWindow(nameWin,0);
    imshow(nameWin,rg.img);
    
    waitKey(0);
    
    Mat tmp = Mat(rg.img.rows,rg.img.cols,CV_16U,Scalar(0));
    rg.visited = tmp.clone();//initialize the matrix of the visited registers to 0

    RegionGrowing(1);//I start recursive calls.
    DrawRG();

    return 0;
}