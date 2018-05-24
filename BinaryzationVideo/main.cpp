#include <stdio.h>
#include <string>

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#define WIDTH_OF_IMAGE 1280
#define HEIGHT_OF_IMAGE 720

using namespace std;
using namespace cv;



/*  fucntion turns a color frame to the binary array
 *  using filtration based on a color of a pixel
 *	tmp_frame store each frame from initial video stream
 *  frameArray - the result binary array */
 
void binaryzationToArray( Mat& tmp_frame,
						  int frameArray[][WIDTH_OF_IMAGE])
{
	Vec3b brgPixel;
	for(int row = HEIGHT_OF_IMAGE - 1; row >= 0; row--)
	{
		for(int cols = 0; cols < WIDTH_OF_IMAGE; cols++)
		{
			brgPixel = tmp_frame.at<Vec3b>(row,cols);
			if(brgPixel[0] < 161) // MAGIC NUMBER from the color histogram
			{
				frameArray[row][cols] = 0;
			}else{
				frameArray[row][cols] = 1;
			}
		}      
	}
}

/*  fucntion turns back the binary array to Mat-class 
 *  to write the binary video file*/

void turningArrayToBitMat( int frameArray[][WIDTH_OF_IMAGE],
						   Mat& out_frame)
{
	Vec3b color;
    for(int row = 0; row < HEIGHT_OF_IMAGE; row++)
	{
		for(int cols = 0; cols < WIDTH_OF_IMAGE; cols++)
		{
			if(frameArray[row][cols] == 1)
			{
				color[0] = 255;
				color[1] = 255;
				color[2] = 255;
			}
			else
			{
				color[0] = 0;
				color[1] = 0;
				color[2] = 0;
			}
			out_frame.at<Vec3b>(Point(cols,row)) = color;
		}
	}
}

int main(int argc, char** argv)
{
    VideoCapture cap;
    
    struct SRegion* pStructArray[1000];
    
  		//array to reflect tmp_frame    
    int frameArray[HEIGHT_OF_IMAGE][WIDTH_OF_IMAGE] = {0};
		
    CommandLineParser parser(argc, argv, "{help h||}{@input||}");
    
    string input = parser.get<std::string>("@input");
    if (input.empty())
        cap.open(0);
    else
        cap.open(input);
    if( !cap.isOpened() )
    {
        printf("\nCan not open video file\n");
        return -1;
    }
    Mat tmp_frame,
		out_frame(HEIGHT_OF_IMAGE, WIDTH_OF_IMAGE, CV_8UC3);
		
    cap >> tmp_frame;
    if(tmp_frame.empty())
    {
        printf("can not read data from the video source\n");
        return -1;
    }
    namedWindow("Binary Video", 1);// UI window to see a video stream
    
    VideoWriter writer;
    int codec = VideoWriter::fourcc('X','2','6','4');
    string filename = "BinaryVideo.avi";
    Size S = Size(WIDTH_OF_IMAGE, HEIGHT_OF_IMAGE);
    writer.open(filename, codec, cap.get(cv::CAP_PROP_FPS), S, true);
    for(;;)
    {
        cap >> tmp_frame;
        if( tmp_frame.empty() )
            break;
        //first turns frame to a binary array
        binaryzationToArray(tmp_frame, frameArray);
		
		//turns back to a binary frame
        turningArrayToBitMat(frameArray, out_frame);
        
        /*uncomment imshow to see out_frame example in window*/
        //imshow("segmented", out_frame);
        
		writer.write(out_frame);
        char keycode = (char)waitKey(30);
                
        /*Exit on Ecs*/
        if( keycode == 27 )
			break;
    }
    return 0;
}

