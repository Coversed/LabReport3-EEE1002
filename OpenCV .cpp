// Include files for required libraries
#include <thread>
#include "opencv_aee.hpp"
#include "main.hpp"     // You can use this file for declaring defined values and functions
#include "pi2c.h"
Pi2c arduino(4);



void setup(void)
{
    setupCamera(320, 240);  // Enable the camera for OpenCV

}

int main( int argc, char** argv )
{

    char receive[16]; //Create a buffer of char (single bytes) for the data
    int HueSym =72;
    int SaturationSym =50;
    int ValueSym =50;
    int Hue2Sym =92;
    int Saturation2Sym =255;
    int Value2Sym =255;

    int Hue =0;
    int Saturation =0;
    int Value =0;
    int Hue2 =255;
    int Saturation2 =255;
    int Value2 =60;

    float LMS = 0;
    float RMS = 0;
    float SA = 0;
    int loop = 0;
    setup();    // Call a setup function to prepare IO and devices
    float error;
    //cv::namedWindow("Photo");   // Create a GUI window called photo
    /*
    Mat CircleRed;
    CircleRed = readImage("/home/pi/Desktop/OpenCV-Template_RPi/OpenCV-Template/Circle(Red Line).png");
    Mat CircleRed_HSV;
    cvtColor(CircleRed, CircleRed_HSV, COLOR_BGR2HSV);
    Mat CircleRed_HSVP;
    inRange(CircleRed_HSV,Scalar(HueSym, SaturationSym, ValueSym), Scalar(Hue2Sym, Saturation2Sym, Value2Sym), CircleRed_HSVP);
    Mat StarGreen;
    StarGreen = readImage("/home/pi/Desktop/OpenCV-Template_RPi/OpenCV-Template/Star(Green Line).png");
    Mat StarGreen_HSV;
    cvtColor(StarGreen, StarGreen_HSV, COLOR_BGR2HSV);
    Mat StarGreen_HSVP;
    inRange(StarGreen_HSV,Scalar(HueSym, SaturationSym, ValueSym), Scalar(Hue2Sym, Saturation2Sym, Value2Sym), StarGreen_HSVP);
    Mat UmbrellaYellow;
    UmbrellaYellow = readImage("/home/pi/Desktop/OpenCV-Template_RPi/OpenCV-Template/Umbrella(Yellow Line).png");
    Mat UmbrellaYellow_HSV;
    cvtColor(UmbrellaYellow, UmbrellaYellow_HSV, COLOR_BGR2HSV);
    Mat UmbrellaYellow_HSVP;
    inRange(UmbrellaYellow_HSV,Scalar(HueSym, SaturationSym, ValueSym), Scalar(Hue2Sym, Saturation2Sym, Value2Sym), UmbrellaYellow_HSVP);
    Mat TriangleBlue;
    TriangleBlue = readImage("/home/pi/Desktop/OpenCV-Template_RPi/OpenCV-Template/Triangle(Blue Line).png");
    Mat TriangleBlue_HSV;
    cvtColor(TriangleBlue, TriangleBlue_HSV, COLOR_BGR2HSV);
    Mat TriangleBlue_HSVP;
    inRange(TriangleBlue_HSV,Scalar(HueSym, SaturationSym, ValueSym), Scalar(Hue2Sym, Saturation2Sym, Value2Sym), TriangleBlue_HSVP);
    */

    while(1)    // Main loop to perform image processing
    {
        Mat frame;
        while(frame.empty())
            frame = captureFrame(); // Capture a frame from the camera and store in a new matrix variable

        //cv::imshow("Photo", frame); //Display the image in the window



        Mat image_HSV;
        cvtColor(frame, image_HSV, COLOR_BGR2HSV); // Convert the image to HSV
       /* std::vector< std::vector<cv::Point> > contours; // Variable for list of contours
        std::vector<Vec4i> hierarchy;
        Mat image_HSVP;
        inRange(image_HSV,Scalar(HueSym, SaturationSym, ValueSym), Scalar(Hue2Sym, Saturation2Sym, Value2Sym), image_HSVP);
        cv::findContours(image_HSVP, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE,Point(0, 0)); // Calculate the contours and store them


        Mat transformed = transformPerspective(contours, frame, 350, 350);
        float matchCircleRed = compareImages(transformed, CircleRed_HSVP);
        float matchStarGreen = compareImages(transformed, StarGreen_HSVP );
        float matchUmbrellaYellow = compareImages(transformed, UmbrellaYellow_HSVP);
        float matchTriangleBlue = compareImages(transformed, TriangleBlue_HSVP);
        if (matchCircleRed > matchStarGreen && matchCircleRed > matchUmbrellaYellow && matchCircleRed > matchTriangleBlue && matchCircleRed > 25)
           {
//            Hue = 0;
//            Saturation = 110;
//            Value = 75;
//            Hue2 = 4;
//            Saturation2 = 255;
//            Value2 = 255;
                printf("Circle Red");
            }
//        if(matchStarGreen > matchCircleRed && matchStarGreen > matchUmbrellaYellow && matchStarGreen > matchTriangleBlue && matchStarGreen > 25)
//            {
//            Hue = 23;
//            Saturation = 110;
//            Value = 110;
//            Hue2 = 39;
//            Saturation2 = 255;
//            Value2 = 255;
//            }
//        if(matchUmbrellaYellow > matchCircleRed && matchUmbrellaYellow > matchStarGreen && matchUmbrellaYellow > matchTriangleBlue && matchUmbrellaYellow > 25)
//            {
//            Hue = 11;
//            Saturation = 110;
//            Value = 110;
//            Hue2 = 17;
//            Saturation2 = 255;
//            Value2 = 255;
//            }
//        if(matchTriangleBlue > matchCircleRed && matchTriangleBlue > matchUmbrellaYellow && matchTriangleBlue > matchStarGreen && matchTriangleBlue > 25)
//            {
//            Hue = 46;
//            Saturation = 110;
//            Value = 110;
//            Hue2 = 72;
//            Saturation2 = 255;
//            Value2 = 255;
//            }
//        else
//            {
//            int Hue =0;
//            int Saturation =0;
//            int Value =0;
//            int Hue2 =100;
//            int Saturation2 =255;
//            int Value2 =4;
//            }
*/
        Mat image_LF;
        inRange(image_HSV, Scalar(Hue, Saturation, Value), Scalar(Hue2, Saturation2, Value2), image_LF);
        float Kp = 0.3;
        float Ki = 0;
        float Kd = 0.06;
        float cumulativeerror = 0;
        float previouserror = 0;
        float servoAngle = 0;
        float u = 0;
        float error = 0;
        float K = 0.5;
        int largest = 0;
        int temp = 0;
        Mat croppedImageBL; // Create a matrix to store the cropped image
        croppedImageBL = image_LF(Rect(0,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner
        Mat croppedImageBML; // Create a matrix to store the cropped image
        croppedImageBML = image_LF(Rect(40,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner
        Mat croppedImageBMML; // Create a matrix to store the cropped image
        croppedImageBMML = image_LF(Rect(80,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner
        Mat croppedImageBMMML; // Create a matrix to store the cropped image
        croppedImageBMMML = image_LF(Rect(120,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner
        Mat croppedImageBMMMR; // Create a matrix to store the cropped image
        croppedImageBMMMR = image_LF(Rect(160,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner
        Mat croppedImageBMR; // Create a matrix to store the cropped image
        croppedImageBMR = image_LF(Rect(200,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner
        Mat croppedImageBMMR; // Create a matrix to store the cropped image
        croppedImageBMMR = image_LF(Rect(240,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner
        Mat croppedImageBR; // Create a matrix to store the cropped image
        croppedImageBR = image_LF(Rect(280,100,40,140)); // Crop image and store the result in croppedImage. X,Y is the pixel coordinate of the top left corner


        int PixBL,PixBML, PixBMML, PixBMMML, PixBMMMR, PixBMMR, PixBMR,PixBR;


        PixBL = countNonZero(croppedImageBL); // Count the number of non-zero pixels
        PixBML= countNonZero(croppedImageBML); // Count the number of non-zero pixels
        PixBMML= countNonZero(croppedImageBMML); // Count the number of non-zero pixels
        PixBMMML = countNonZero(croppedImageBMMML);
        PixBMMMR = countNonZero(croppedImageBMMMR); // Count the number of non-zero pixels
        PixBMMR = countNonZero(croppedImageBMMR);
        PixBMR = countNonZero(croppedImageBMR); // Count the number of non-zero pixels
        PixBR = countNonZero(croppedImageBR); // Count the number of non-zero pixels
        int maximum[] ={PixBL, PixBML, PixBMML, PixBMMML, PixBMMMR, PixBMMR, PixBMR, PixBR};
        for (int loop=0; loop<7; loop=loop+1)
        {
            if (largest <= maximum[loop])
            {
                largest = maximum[loop];
                temp = loop;
                printf("\n%d",temp);


            }
        }
        arduino.i2cWriteArduinoInt(temp);

        //cv::imshow("BNW",image_LF);



        int key = cv::waitKey(10);   // Wait 1ms for a keypress (required to update windows)

        key = (key==255) ? -1 : key;    // Check if the ESC key has been pressed
        if (key == 27)
        {
            break;
        }

    }
	closeCV();  // Disable the camera and close any windows
	return 0;
	}


