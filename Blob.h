#pragma once
#include "opencv.hpp"

using namespace std;
using namespace cv;


class SJW {
public:
	vector<Point> contours;
	vector<Point> centerContours;
	SJW(vector<Point> _contours);

};