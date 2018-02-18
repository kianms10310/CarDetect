
#include "opencv.hpp"
#include "Blob.h"
using namespace cv;
using namespace std;


#define SCALAR_WHITE Scalar(255,255,255)
#define SCALAR_BLACK Scalar(0,0,0)
#define SCALAR_RED Scalar(255,0,0)
#define SCALAR_GREEN Scalar(0,255,0)
#define SCALAR_BLUE Scalar(0,0,255)

void drawContours(Size imgSize, vector<vector<Point>> contours, string str);


int main()
{
	SimpleBlobDetector::Params params;
	vector<SJW> sjws;
	//params.filterByArea = true;
	params.minArea = 50;
	params.minArea = 500;
	params.filterByCircularity = true;
	// Change thresholds
	//params.minThreshold = 10;
	params.maxThreshold = 300;
	params.blobColor = 0;
	// Filter by Circularity
	//params.filterByCircularity = 10;
	//params.minCircularity = 5;

	Mat frame1, frame2;
	//VideoCapture capture("CarsDrivingUnderBridge.mp4");
	VideoCapture capture("20170613_082211t.mp4");
	if (!capture.isOpened())
	{
		cout << " Can not open capture !!!" << endl;
		return 0;
	}

	int fps = (int)(capture.get(CV_CAP_PROP_FPS));

	int delay = 1000 / fps;
	int frameNum = -1;

	capture.read(frame1);
	capture.read(frame2);

	Point linePoint[2];
	int lineHorPoint = (int)round((double)frame1.rows * 0.4);
	linePoint[0].x = 0; linePoint[1].x = frame1.cols - 1;
	linePoint[0].y = linePoint[1].y = lineHorPoint;
	int carCount = 0;
	for (;;)
	{

		vector<SJW> currentFrameBlobs;
		Mat img1, img2, imgDiff, imgThresh, imgOutPut;
		img1 = frame1.clone();
		img2 = frame2.clone();



		imshow("frame", frame1);

		cvtColor(img1, img1, CV_BGR2GRAY);
		cvtColor(img2, img2, CV_BGR2GRAY);

		GaussianBlur(img1, img1, Size(5, 5), 0);
		GaussianBlur(img2, img2, Size(5, 5), 0);

		absdiff(img1, img2, imgDiff);

		threshold(imgDiff, imgThresh, 30, 255.0, CV_THRESH_BINARY);

		imshow("imgDiff", imgDiff);
		Mat structuringElement5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));

		for (unsigned int i = 0; i < 2; i++) {
			dilate(imgThresh, imgThresh, structuringElement5x5);
			dilate(imgThresh, imgThresh, structuringElement5x5);
			erode(imgThresh, imgThresh, structuringElement5x5);
		}

		imshow("imgThresh", imgThresh);


		Mat imgThreshCopy = imgThresh.clone();

		vector<vector<Point>> contours;

		findContours(imgThreshCopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		drawContours(imgThresh.size(), contours, "imgContours");

		vector<vector<Point> > convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			convexHull(contours[i], convexHulls[i]);
		}

		Mat image(imgThreshCopy.size(), CV_8UC3, SCALAR_WHITE);
		drawContours(image, convexHulls, -1, SCALAR_BLACK, -1);;
		/*
		IplImage * IpImage_img = new IplImage(image);
		for (int i = linePoint[0].x; i < linePoint[1].x; i++) {
		Scalar s = cvGet2D(IpImage_img, i , linePoint[0].y);
		cout << s << endl;
		}
		//*/

		line(image, linePoint[0], linePoint[1], SCALAR_RED, 2);

		imshow("image", image);

		for (auto &convexHull : convexHulls) {
			SJW possible(convexHull);

			sjws.push_back(possible);
		}

		for (auto sjw : sjws) {
			int preFrame = (int)sjw.centerContours.size() - 2;
			int curFrame = (int)sjw.centerContours.size() - 1;

			if (sjw.centerContours[preFrame].y > lineHorPoint &&
				sjw.centerContours[curFrame].y <= lineHorPoint) {
				carCount++;
			}
		}
		cout << carCount << endl;

		vector<KeyPoint> keypoints;
#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

		// Set up detector with params
		SimpleBlobDetector detector(params);

		// Detect blobs
		detector.detect(im, keypoints);
#else 

		// Set up detector with params
		Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

		// Detect blobs
		detector->detect(image, keypoints);
#endif 
		Mat im_with_keypoints;
		drawKeypoints(image, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("keypoints", im_with_keypoints);


		frame1 = frame2.clone();

		if ((capture.get(CV_CAP_PROP_POS_FRAMES) + 1) < capture.get(CV_CAP_PROP_FRAME_COUNT)) {
			capture.read(frame2);
		}
		else {
			cout << "end of video\n";
			break;
		}

		frameNum++;
		int ckey = waitKey(delay);
		if (ckey == 27)
			break;
		else if (ckey == 32)
			waitKey();
	}
	return 0;
}


void drawContours(Size imgSize, vector<vector<Point>> contours, string str) {
	Mat image(imgSize, CV_8UC3, SCALAR_BLACK);

	drawContours(image, contours, -1, SCALAR_WHITE, -1);
	imshow(str, image);
}