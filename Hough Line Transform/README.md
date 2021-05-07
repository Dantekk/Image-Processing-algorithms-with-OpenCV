Hough Line Transform algorithm is very sensitive to the threshold values used. </br>
To get good results, you should try different threshold values for Canny and for Hough.

Hough Line Transform application example using threshold 10,100 as min/max values for Canny step, threshold 125,150 as min/max values for Hough step.

To run this example, you can use :

`/hough_line images/sudoku.png 10 100 125 150`

| Before Hough Line |  After Hough Line |
:---------:|:-----:|
![](https://github.com/Dantekk/Image-Processing-algorithms-with-OpenCV/blob/main/Hough%20Line%20Transform/images/sudoku.png) | ![](https://github.com/Dantekk/Image-Processing-algorithms-with-OpenCV/blob/main/Hough%20Line%20Transform/images/sudoku_out.png)
