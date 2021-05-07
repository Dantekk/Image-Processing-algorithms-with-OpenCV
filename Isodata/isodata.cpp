#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>

using namespace std;
using namespace cv;

struct mean_tt{
    Vec3f sum;
    int nElem;
    Vec3b mean;
};

Mat Isodata(Mat, int , int, int);
float EuclideanDistance(Vec3b, Vec3b);

float EuclideanDistance(Vec3b p1, Vec3b p2){

    return sqrt( pow(p1[0]-p2[0],2) + 
                 pow(p1[1]-p2[1],2) +
                 pow(p1[2]-p2[2],2) );
}

/*
The function takes as input:
Mat img: the starting image
int n: the number of clusters to initially create
int sSud: the threshold for comparison on splitting a cluster
int sUni: The threshold for comparing the union of a cluster
*/
Mat Isodata(Mat img, int n, int sSudd, int sUni){
    
    srand(time(NULL));
    /*
    This matrix is used to keep track of which claster the pixels of the image belong to.
    */
    Mat tmp = Mat(img.rows, img.cols, CV_16U, Scalar(0));
    /*
    Vector of struct mean.
    It will contain information regarding clusters
    while the algorithm is running.
    */
    vector<mean_tt> t;

    /*
    It is used to create the n initial means.
    At each step I randomly choose a pixel, whose RGB value becomes a mean.
    I initialize a struct mean and insert it into the vector t.
    */
    for(int i=0; i<n; i++){
        int y = rand()%img.rows;
        int x = rand()%img.cols;
        
        struct mean_tt m;
        m.nElem = 0;
        m.sum = Vec3b(0,0,0);
        m.mean = img.at<Vec3b>(y,x);
        t.push_back(m);
        
    }

    /*
    1) Algorithm step
    I assign all the pixels of the image to the initial n clusters.
    Each pixel is assigned to the cluster that has the mean closest to the
    pixel considered.
    */
    for(int y=0; y<img.rows; y++)
            for(int x=0; x<img.cols; x++){
                float distMin=450;//max Euclidean distance in RGB
                int indexMin;
                /*
                For each pixel I calculate the Euclidean distance with all the cluster means
                and I find the minimum distance.
                */
                for(int i=0; i<t.size(); i++){
                    float dist = EuclideanDistance(img.at<Vec3b>(y,x), t[i].mean);

                    if(dist<distMin){
                        distMin = dist;
                        indexMin = i;//cluster index with minimum distance from the considered pixel
                    }
                }

                tmp.at<uchar>(y,x) = indexMin; //I assign the pixel to the cluster
                t[indexMin].nElem++;//I increase the number of pixel elements in that cluster.
            }

    int numIt=0;
 
    do{
  
    /*
    2) Algorithm step: subdivision
    Each cluster whose variance exceeds a certain threshold is divided into 2 clusters.
    This means that in the cluster there are pixels that have not quite similar colors, and
    therefore we divide them.
    */
    for(int i=0; i<t.size(); i++){//for each cluster
        int oneTime=0;
        /*
        If the cluster is empty, go to the next iteration and try
        another cluster. A cluster can be empty because it has been merged
        to other clusters in the merge phase, so it's as if it wasn't there.
        */
        if(t[i].nElem==0) continue; 
        
        for(int y=0; y<img.rows; y++){
            for(int x=0; x<img.cols; x++){
                int id = tmp.at<uchar>(y,x);
                if(id!=i) continue; 
                /*
                if they are here, it is because the selected pixel belongs
                to cluster t [i]. Then I calculate the Euclidean distance
                between the RGB value of the selected pixel and the mean of the cluster.
                */
                float dist = EuclideanDistance(img.at<Vec3b>(y,x), t[i].mean);
                //if this value is greater than the sSudd threshold, 
                //it is necessary to subdivide the cluster t [i] creating a new cluster
                if(dist>sSudd){
                    oneTime++;
                    /*
                    If it is the first pixel of the new cluster, I create a struct mean m,
                    I initialize it and insert it into the struct t.
                    */
                    if(oneTime==1){
                        n++;
                        struct mean_tt m;
                        m.sum = Vec3b(0,0,0);
                        m.nElem = 1;
                        m.mean = Vec3b(0,0,0);
                        t.push_back(m);
                    }
                    tmp.at<uchar>(y,x)=n-1; //new cludster id
                    t[n-1].nElem++;//increment the counter of the number of pixels of the new cluster
                    t[i].nElem--;//I decrease the counter of the number of pixels of the cluster from which I have removed the pixel
                }
            }
        }
    }

    /*
    3) Algorithm step: Union
    If you have created clusters with a smaller number of pixels
    at the sUni threshold, the cluster vanishes and all elements
    they must be merged with other clusters already present. In particular
    each pixel is assigned to the cluster at which the distance from
    mean is minimal.
    */

    for(int i=0; i<t.size(); i++){//scorro il vettore dei cluster
        /*
        If the number of pixels in the cluster is less than sUni and if
        the cluster in question is not one of those obtained in the previous step,
        of which we still have to calculate the mean:
        */
        if(t[i].nElem < sUni && t[i].mean[0]!=0){
            //scroll all the matrix until all the pixels of the cluster have been assigned.
            
            for(int y=0; y<tmp.rows && t[i].nElem!= 0; y++)
                for(int x=0; x<tmp.cols; x++){
                    if(tmp.at<uchar>(y,x)==i){
                        float minDist=455;
                        int index;
                        /*
                        I calculate the distance of the pixel from the mean of all the other clusters.
                        */
                        for(int k=0; k<t.size(); k++){
                            float dist = EuclideanDistance(img.at<Vec3b>(y,x), t[i].mean);
                            if(dist<minDist){
                                minDist = dist;
                                index=k;

                            }
                        }
                        tmp.at<uchar>(y,x)=index;//the pixel is assigned to the cluster from which it has the least distance from the mean.
                        t[index].nElem++; //I increase the number of pixels of the cluster to which I have just assigned the pixel
                        t[i].nElem--; //I decrease the number of elements of the cluster t [i].
                    }
                }
        }
    }

    /*
    4) Algorithm step: recalculation of the averages of the created / modified clusters.
    */

    /*
    I initialize the variables of the average calculation of all clusters.
    */
    for(int k=0; k<t.size(); k++){
        t[k].mean=Vec3b(0,0,0);
        t[k].sum=Vec3f(0,0,0);
        t[k].nElem=0;

    }

    /*
    Calculation for each cluster:
    1) The RGB sum of all pixels
    2) The number of pixels of which it is composed
    */
    for(int y=0; y<tmp.rows; y++){
        for(int x=0; x<tmp.cols; x++){
            int id=tmp.at<uchar>(y,x);
            t[id].sum[0]+=img.at<Vec3b>(y,x)[0];
            t[id].sum[1]+=img.at<Vec3b>(y,x)[1];
            t[id].sum[2]+=img.at<Vec3b>(y,x)[2];
            t[id].nElem++;
        }
    }

    /*
    I calculate the mean of all the pixels
    */
    for(int k=0; k<t.size(); k++){
        t[k].mean[0] = t[k].sum[0]/t[k].nElem;
        t[k].mean[1] = t[k].sum[1]/t[k].nElem;
        t[k].mean[2] = t[k].sum[2]/t[k].nElem;
    }

    numIt++;
    }while(numIt<10);
       
    Mat output = Mat(img.rows, img.cols, CV_8UC3);

    /*
    The final RGB value of each pixel of each cluster matches
    to the RGB value of the mean of its cluster.
    */
    for(int y=0; y<img.rows; y++)
            for(int x=0; x<img.cols; x++){
                int index = tmp.at<uchar>(y,x);
                output.at<Vec3b>(y,x)=t[index].mean;
            }

    return output;
}

int main(int argc, char **argv){

    /*
    argv[1] -> input image
    argv[2] -> number of initial clusters
    argv[3] -> threshold for division
    argv[4] -> threshold for union
    */

    Mat img = imread(argv[1]);

    if(img.empty()){
        cout<<"Image input failed!"<<endl;
        return -1;
    }

    int     n = atoi(argv[2]); 
    int sSudd = atoi(argv[3]); 
    int sUni  = atoi(argv[4]); 

    namedWindow("Original Image",0);
    imshow("Original Image",img);
    waitKey(0);

    Mat output;
    output = Isodata(img, n, sSudd, sUni);

    namedWindow("Image after Isodata",0);
    imshow("Image after Isodata",output);
    waitKey(0);

    return 0;

}