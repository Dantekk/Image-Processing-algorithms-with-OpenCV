Hough Circle Transform algorithm is very sensitive to the threshold values used. </br>
To get good results, you should try different threshold values for Canny and for Hough.

Hough Circle Transform application example using threshold 50,100 as min/max values for Canny step, threshold 10,110 as min/max values for radius, threshold 178 for algorithm and threshold 1 as step size Theta values.

To run this example, you can use :

`/hough_circle images/OC.png 50 100 10 110 178 1`

| Before Hough Circle |  After Hough Circle |
:---------:|:-----:|
![](https://github.com/Dantekk/Image-Processing-algorithms-with-OpenCV/blob/main/Hough%20Circle%20Transform/images/OC.png) | ![](https://github.com/Dantekk/Image-Processing-algorithms-with-OpenCV/blob/main/Hough%20Circle%20Transform/images/OC_out.png)

Another Hough Circle Transform application example using threshold 50,100 as min/max values for Canny step, threshold 10,190 as min/max values for radius, threshold 58 for algorithm and threshold 2 as step size Theta values.

To run this example, you can use :

`/hough_circle images/OC.png 50 100 10 110 178 1`

| Before Hough Circle |  After Hough Circle |
:---------:|:-----:|
![](https://github.com/Dantekk/Image-Processing-algorithms-with-OpenCV/blob/main/Hough%20Circle%20Transform/images/circles.png) | ![](https://github.com/Dantekk/Image-Processing-algorithms-with-OpenCV/blob/main/Hough%20Circle%20Transform/images/circles_out.png)
