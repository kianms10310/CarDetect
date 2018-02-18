#include "Blob.h"

SJW::SJW(vector<Point> _contours) {
	contours = _contours;

	Rect rect = boundingRect(contours);

	Point a;
	a.x = (rect.x + rect.x + rect.width) / 2;
	a.y = (rect.y + rect.y + rect.height) / 2;
	centerContours.push_back(a);

}