﻿#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
vector<vector<int>> newPoints;



// Project Virtual Paint //

vector<vector<int>> myColors{ {79, 57, 87, 134, 255, 255}	// blue
							  ,{24, 94, 132, 49, 255, 246}		// yellow
							  ,{50, 42, 35, 69, 255, 189} // green
};

vector<Scalar> myColorValues{ {255,0,0},		// blue
							  {0, 255, 255},		// yellow
							  {0, 255, 0} };	// green

Point getContours(Mat imgDil) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;


	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	string objectType;

	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++) {

		int area = contourArea(contours[i]);
		cout << area << endl;

		if (area > 1000) {
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;


			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);


		}
	}

	return myPoint;
}


vector<vector<int>> findColor(Mat img) {

	Mat imgHSV;

	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++) {

		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);		// h = Hue, s = Saturation, v = Value (Тон, Насыщенность, Значение)
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);		// h = Hue, s = Saturation, v = Value (Тон, Насыщенность, Значение)
		Mat mask;
		inRange(imgHSV, lower, upper, mask);

		//imshow(to_string(i), mask);

		Point myPoint = getContours(mask);

		if (myPoint.x != 0 && myPoint.y != 0) {
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}
	}

	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues) {

	for (int i = 0; i < newPoints.size(); i++) {

		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
	}
}


void main() {

	VideoCapture cap(0);


	while (true) {

		cap.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, myColorValues);

		imshow("Image", img);
		waitKey(1);
	}

}