#include <stdio.h>
#include <string>

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include "SRegion.h"

#define WIDTH_OF_IMAGE 1280
#define HEIGHT_OF_IMAGE 720

using namespace std;
using namespace cv;

void moveDown(int row,
			  int column,
			  int tempArray[][WIDTH_OF_IMAGE],
			  int label,
			  struct SRegion* pSRegion)
{
	//checking and mark cells in the down direction
	int neighbourDown = row + 1;
	while(tempArray[neighbourDown][column] == 1 && neighbourDown < HEIGHT_OF_IMAGE)
	{
		tempArray[neighbourDown][column] = label;
		pSRegion->Area++;
		neighbourDown++;
	}
}

void moveRight( int row,
				int column,
				int tempArray[][WIDTH_OF_IMAGE],
				int label,
				struct SRegion* pSRegion)
{
	int neighbourRight = column + 1;
		while(tempArray[row][neighbourRight] == 1	&& neighbourRight < WIDTH_OF_IMAGE)
		{
			tempArray[row][neighbourRight] = label;
			pSRegion->Area++;
			moveDown(row, neighbourRight, tempArray, label, pSRegion);
			neighbourRight++;
		}
}

void moveLeft( int row,
			   int column, 
			   int tempArray[][WIDTH_OF_IMAGE], 
			   int label, 
			   struct SRegion* pSRegion)
{
	int neighbourLeft = column - 1;
		while(tempArray[row][neighbourLeft] == 1	&& neighbourLeft >= 0 )
		{
			tempArray[row][neighbourLeft] = label;
			pSRegion->Area++;
			moveDown(row, neighbourLeft, tempArray, label, pSRegion);
			neighbourLeft--;
		}
}


void moveDownRight( int row,
					int column,
					int tempArray[][WIDTH_OF_IMAGE],
					int label,
					struct SRegion* pSRegion)
{
	int neighbourDown = row + 1;
	int neighbourRight = column + 1;
	while( tempArray[neighbourDown][neighbourRight] == 1 &&
		   neighbourDown < HEIGHT_OF_IMAGE &&
		   neighbourRight < WIDTH_OF_IMAGE)
		{
			tempArray[neighbourDown][neighbourRight] = label;
			pSRegion->Area++;
			moveDown(neighbourDown, neighbourRight, tempArray, label, pSRegion);
			moveRight(neighbourDown, neighbourRight, tempArray, label, pSRegion);
			moveLeft(neighbourDown, neighbourRight, tempArray, label, pSRegion);
			neighbourDown++;
			neighbourRight++;
		}
}

void moveDownLeft( int row,
				   int column,
				   int tempArray[][WIDTH_OF_IMAGE],
				   int label,
				   struct SRegion* pSRegion)
{
	int neighbourDown = row + 1;
		int neighbourLeft = column - 1;
		while(tempArray[neighbourDown][neighbourLeft] == 1 &&
			  neighbourDown < HEIGHT_OF_IMAGE &&
			  neighbourLeft >= 0)
		{
			tempArray[neighbourDown][neighbourLeft] = label;
			pSRegion->Area++;
			moveDown(neighbourDown, neighbourLeft, tempArray, label, pSRegion);
			moveLeft(neighbourDown, neighbourLeft, tempArray, label, pSRegion);
			moveRight(neighbourDown, neighbourLeft, tempArray, label, pSRegion);
			moveDownRight(neighbourDown, neighbourLeft, tempArray, label, pSRegion);
			neighbourDown++;
			neighbourLeft--;
		}
}

int setRegionColor()
	{
		return rand()%255;
	}
	

void detectRegions( int intBitArray[][WIDTH_OF_IMAGE],
					struct SRegion* pStructArray[])
{
	int intRegionCounter = 0,
		intTemp = 0;
	for(int i = 0; i < HEIGHT_OF_IMAGE; i++)
	{
		for(int j = 0; j < WIDTH_OF_IMAGE; j++)
		{
			if(intBitArray[i][j] == 1)
			{
				if(intBitArray[i][j + 1] > 1) // check the right neighbour
				{
					intBitArray[i][j] = intBitArray[i][j + 1];
					intTemp = intBitArray[i][j];
					moveDown(i, j, intBitArray, intTemp, pStructArray[intTemp]);
				}
				else if(intBitArray[i + 1][j + 1] > 1) // check the down-right neighbour
				{
					intBitArray[i][j] = intBitArray[i + 1][j + 1];
					intTemp = intBitArray[i][j];
					moveDownLeft(i, j, intBitArray, intTemp, pStructArray[intTemp]);
				}
				else if(intBitArray[i + 1][j - 1] > 1) // check the down-left neighbour
				{
					intBitArray[i][j] = intBitArray[i + 1][j - 1];
					intTemp = intBitArray[i][j];
					moveDownLeft(i, j, intBitArray, intTemp, pStructArray[intTemp]);
				}
				else
				{
					intRegionCounter++;
					pStructArray[intRegionCounter] = new struct SRegion;
					pStructArray[intRegionCounter]->Area = 1;
					intBitArray[i][j] = intRegionCounter;
					pStructArray[intRegionCounter]->ColorRed = setRegionColor();
					pStructArray[intRegionCounter]->ColorGreen = setRegionColor(); 
					pStructArray[intRegionCounter]->ColorBlue = setRegionColor();
					moveDown(i, j, intBitArray, intRegionCounter, pStructArray[intRegionCounter]);
					moveRight(i, j, intBitArray, intRegionCounter, pStructArray[intRegionCounter]);
					moveDownLeft(i, j, intBitArray, intRegionCounter, pStructArray[intRegionCounter]);
					moveDownRight(i, j, intBitArray, intRegionCounter, pStructArray[intRegionCounter]);
				}
			}
			else if(intBitArray[i][j] > 1) // if the cell is marked, just check neighbours
			{
				intTemp = intBitArray[i][j];
				moveDownLeft(i, j, intBitArray, intTemp, pStructArray[intTemp]);
				moveDownRight(i, j, intBitArray, intTemp, pStructArray[intTemp]);
			}
		}
	}
}


/*  this fucntion turns a color frame to the binary array
 *  using filtration based on a color of a pixel
 *	tmp_frame store each frame from initial video stream
 *  frameArray - result binary array */
 
void binaryzationToArray( Mat& tmp_frame,
						  int frameArray[][WIDTH_OF_IMAGE])
{
	Vec3b brgPixel;
	for(int row = HEIGHT_OF_IMAGE - 1; row >= 0; row--)
	{
		for(int cols = 0; cols < WIDTH_OF_IMAGE; cols++)
		{
			brgPixel = tmp_frame.at<Vec3b>(row,cols);
			if(brgPixel[0]*255*255 < 10500000) // MAGIC NUMBER from the color histogram
			{
				frameArray[row][cols] = 0;
			}else{
				frameArray[row][cols] = 1;
			}
		}      
	}
}


void copingArray( int tempArray[][WIDTH_OF_IMAGE], 
				  int cumulativeArray[][WIDTH_OF_IMAGE])
{
	for(int row = 0; row < HEIGHT_OF_IMAGE; row++)
	{
		for(int cols = 0; cols < WIDTH_OF_IMAGE; cols++)
		{
			cumulativeArray[row][cols] = tempArray[row][cols];
		}      
	}
}

void comparingArrays( int tempArray[][WIDTH_OF_IMAGE], 
					  int cumulativeArray[][WIDTH_OF_IMAGE])
{
	for(int row = 0; row < HEIGHT_OF_IMAGE; row++)
	{
		for(int cols = 0; cols < WIDTH_OF_IMAGE; cols++)
		{
			if(tempArray[row][cols] == 1)
			{
				//look around
				if(cumulativeArray[row][cols - 1] !=0){
				   tempArray[row][cols] = cumulativeArray[row][cols - 1];
				}
				if(cumulativeArray[row][cols] !=0){
				   tempArray[row][cols] = cumulativeArray[row][cols];
				}
				if(cumulativeArray[row][cols + 1] !=0){
				   tempArray[row][cols] = cumulativeArray[row][cols + 1];
				}
				if(cumulativeArray[row + 1][cols - 1] !=0){
				   tempArray[row][cols] = cumulativeArray[row + 1][cols - 1];
				}
				if(cumulativeArray[row + 1][cols] !=0){
				   tempArray[row][cols] = cumulativeArray[row + 1][cols];
				}
				if(cumulativeArray[row + 1][cols + 1] !=0){
				   tempArray[row][cols] = cumulativeArray[row + 1][cols + 1];   
				}
			}
		}      
	}
}

void turningArrayToMat( int frameArray[][WIDTH_OF_IMAGE],
						Mat& out_frame,
						struct SRegion* pStructArray[])
{
	Vec3b color;
	int label = 0;
    for(int row = 0; row < HEIGHT_OF_IMAGE; row++)
	{
		for(int cols = 0; cols < WIDTH_OF_IMAGE; cols++)
		{
			if(frameArray[row][cols] >= 1)
			{
				label = frameArray[row][cols];
				if(pStructArray[label]->Area > 30)
				{
					color[0] = pStructArray[label]->ColorBlue;
					color[1] = pStructArray[label]->ColorGreen;
					color[2] = pStructArray[label]->ColorRed;
				}
				else
				{
					color[0] = 0;
					color[1] = 0;
					color[2] = 0;
				}
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
    int frameArray[HEIGHT_OF_IMAGE][WIDTH_OF_IMAGE] = {0},
		// array to store the hisory of a frame
		cumulativeArray[HEIGHT_OF_IMAGE][WIDTH_OF_IMAGE] = {0};

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
    namedWindow("segmented", 1);
    bool firstFrame = true;
    for(;;)
    {
        cap >> tmp_frame;
        if( tmp_frame.empty() )
            break;
        
        binaryzationToArray(tmp_frame, frameArray);
        
        if (firstFrame){
			detectRegions(frameArray, pStructArray);
			copingArray(frameArray, cumulativeArray);
			firstFrame = false;
		}
        comparingArrays(frameArray, cumulativeArray);
        
        turningArrayToMat(frameArray, out_frame, pStructArray);
        //show image in window(namedWindow)
        imshow("segmented", out_frame);
        copingArray(cumulativeArray, frameArray);
        char keycode = (char)waitKey(30);
                
        /*Exit on Ecs*/
        if( keycode == 27 )
			break;
    }
    // free the used memory from structures
	for(int j = 0; j < 1000; j++)delete pStructArray[j];
    return 0;
}
