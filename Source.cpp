#include <cstdio>
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace cv;
using namespace std;
#define PI 3.14159

void drawHistImg(const Mat &src, Mat &dst);
void rotateImage(const Point center, const Mat &src, Mat &dst, double degree, double scale);

int main() {
	Mat original = imread("test_file.bmp",IMREAD_COLOR);
	int rows = original.rows;
	int cols = original.cols;
	//imshow("original", original);
	Mat target_origin = imread("target.bmp", IMREAD_COLOR);
	Mat target;
	cvtColor(target_origin, target, COLOR_BGR2GRAY);

	//imshow("Target", target);
	Mat displayImg = original.clone();
	Mat src;
	cvtColor(original, src, COLOR_BGR2GRAY);
	Mat result;
	
	Mat contoursImg = original.clone();
	Mat pyrImg;
	pyrImg = src.clone();
	threshold(pyrImg, pyrImg, 230, 255, THRESH_TOZERO_INV);
	Mat repyrImg;
	int times = 1;
	for (int i = 1; i <= times; i++) {
		pyrDown(pyrImg, pyrImg);
		repyrImg = pyrImg.clone();

		for (int j = 0; j < i; j++) {
			pyrUp(repyrImg, repyrImg);
		}
		char s[5];
		//sprintf_s(s, "%03d", i);
		//imshow(s, repyrImg);
	}
	//imshow("PRY", repyrImg);
	//imwrite("After 1 times to test file.bmp", repyrImg);
	threshold(repyrImg, repyrImg, 150, 255, THRESH_BINARY);
	//imshow("PRY+THR", repyrImg);
	//imwrite("Apply Threshold to PyrImg (test file).bmp", repyrImg);
	Mat pryEDGE;
	Mat PRY_contoursImg = original.clone();
	Canny(repyrImg, pryEDGE, 50, 150, 3);
	//imshow("CANNY1", pryEDGE);
	vector<vector<Point>> contoursPRY;
	vector<Vec4i> hierarchyPRY;
	RNG rngPRY(12345);

	findContours(pryEDGE, contoursPRY, hierarchyPRY, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<Point2f> mc(contoursPRY.size());
	vector<Moments> mu(contoursPRY.size());
	vector<Point2f> area_number(contoursPRY.size());
	vector<Point2f> top3_by_x(3);
	for (int i = 0; i < contoursPRY.size(); i++)
	{
		area_number[i].x = i;
		area_number[i].y = contourArea(contoursPRY[i]);
		mu[i] = moments(contoursPRY[i], false);
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		Scalar color = Scalar(rngPRY.uniform(0, 255), rngPRY.uniform(0, 255), 255);
		if (contourArea(contoursPRY[i]) > 10) {
			drawContours(PRY_contoursImg, contoursPRY, i, color, 1, 8, hierarchyPRY);
			circle(PRY_contoursImg, mc[i], 4, color, -1, 8, 0);
		}
		//printf(" * Contour[%02d]( %.5f, %.5f ) Area = %.5f - Length: %.5f \n", i,mc[i].x,mc[i].y,  contourArea(contoursPRY[i]), arcLength(contoursPRY[i], true));
	}
	//imshow("PRY_Canny", PRY_contoursImg);
	//imwrite("Find Canny of Image (test file).bmp", PRY_contoursImg);


	for (int m = 0; m < contoursPRY.size(); m++) {
		for (int n = m; n < contoursPRY.size(); n++) {
			if (area_number[m].y < area_number[n].y) {
				Point temp = area_number[m];
				area_number[m] = area_number[n];
				area_number[n] = temp;
			}
		}
	}
	for (int l = 0; l < 3; l++) {
		top3_by_x[l] = mc[area_number[l].x];
	}

	for (int j = 0; j < 3; j++) {
		for (int k = j; k < 3; k++) {
			if (top3_by_x[j].x < top3_by_x[k].x) {
				Point temp = top3_by_x[j];
				top3_by_x[j] = top3_by_x[k];
				top3_by_x[k] = temp;
			}
		}
	}
	for (int i = 0; i < contoursPRY.size(); i++) {
		//printf("area_number[%02d] = ( %.2f, %.2f )\n", i, area_number[i].x, area_number[i].y);
	}
	for (int i = 0; i < 3; i++) {
		//printf("BY_X area_number[%02d] = ( %.2f, %.2f )\n", i, top3_by_x[i].x, top3_by_x[i].y);
	}






	Mat tar_pyrImg;
	tar_pyrImg = target.clone();
	threshold(tar_pyrImg, tar_pyrImg, 230, 255, THRESH_TOZERO_INV);
	//threshold(pyrImg, pyrImg, 125, 255, THRESH_BINARY);
	Mat tar_repyrImg;
	for (int i = 1; i <= times; i++) {
		pyrDown(tar_pyrImg, tar_pyrImg);
		tar_repyrImg = tar_pyrImg.clone();

		for (int j = 0; j < i; j++) {
			pyrUp(tar_repyrImg, tar_repyrImg);
		}
		char s[5];
		//sprintf_s(s, "%03d", i);
		//imshow(s, repyrImg);
	}
	//imshow("Target PRY", tar_repyrImg);
	//imwrite("After Pyr Down and Up for 1 times (target).bmp", tar_repyrImg);

	threshold(tar_repyrImg, tar_repyrImg, 150, 255, THRESH_BINARY);
	//imwrite("Apply Threshold to PyrImg (target).bmp", tar_repyrImg);
	//imshow("Target PRY+THR", tar_repyrImg);

	Mat tar_pryEDGE;
	Mat tar_PRY_contoursImg = target.clone();
	Canny(tar_repyrImg, tar_pryEDGE, 50, 150, 3);
	//imshow("Target CANNY", tar_pryEDGE);
	vector<vector<Point>> tar_contoursPRY;
	vector<Vec4i> tar_hierarchyPRY;
	RNG tar_rngPRY(12345);

	findContours(tar_pryEDGE, tar_contoursPRY, tar_hierarchyPRY, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<Point2f> tar_mc(tar_contoursPRY.size());
	vector<Moments> tar_mu(tar_contoursPRY.size());
	vector<Point2f> tar_area_number(tar_contoursPRY.size());
	vector<Point2f> target_top3_x(3);
	for (int i = 0; i < tar_contoursPRY.size(); i++)
	{
		tar_area_number[i].x = i;
		tar_area_number[i].y = contourArea(tar_contoursPRY[i]);
		tar_mu[i] = moments(tar_contoursPRY[i], false);
		tar_mc[i] = Point2f(tar_mu[i].m10 / tar_mu[i].m00, tar_mu[i].m01 / tar_mu[i].m00);
		Scalar color = Scalar(tar_rngPRY.uniform(0, 255), tar_rngPRY.uniform(0, 255), 255);
		if (contourArea(tar_contoursPRY[i]) > 10) {
			drawContours(target_origin, tar_contoursPRY, i, color, 1, 8, tar_hierarchyPRY);
			circle(target_origin, tar_mc[i], 4, color, -1, 8, 0);
		}
		//printf("Target Contour[%02d]( %.5f, %.5f ) Area = %.5f - Length: %.5f \n", i, tar_mc[i].x, tar_mc[i].y, contourArea(tar_contoursPRY[i]), arcLength(tar_contoursPRY[i], true));
	}
	//imshow("Target PRY_Canny", target_origin);
	//imwrite("Find Canny of Image (target).bmp", target_origin);



	for (int m = 0; m < tar_contoursPRY.size(); m++) {
		for (int n = m; n < tar_contoursPRY.size(); n++) {
			if (tar_area_number[m].y < tar_area_number[n].y) {
				Point temp = tar_area_number[m];
				tar_area_number[m] = tar_area_number[n];
				tar_area_number[n] = temp;
			}
		}
	}
	for (int i = 0; i < tar_contoursPRY.size(); i++) {
		//printf("Target area_number[%02d] = ( %.2f, %.2f )\n", i, tar_area_number[i].x, tar_area_number[i].y);
	}

	for (int l = 0; l < 3; l++) {
		target_top3_x[l] = tar_mc[tar_area_number[l].x];
	}

	for (int j = 0; j < 3; j++) {
		for (int k = j; k < 3; k++) {
			if (target_top3_x[j].x < target_top3_x[k].x) {
				Point temp = target_top3_x[j];
				target_top3_x[j] = target_top3_x[k];
				target_top3_x[k] = temp;
			}
		}
	}
	for (int i = 0; i < 3; i++) {
		//printf("Target BY_X area_number[%02d] = ( %.2f, %.2f )\n", i, target_top3_x[i].x, target_top3_x[i].y);
	}

	Point rotateEND;
	Point center = top3_by_x[1];
	Point V_mc = top3_by_x[0];
	Point A_mc = top3_by_x[2];
	Point target_center = target_top3_x[1];
	double sample_length = pow(V_mc.x - A_mc.x, 2) + pow(V_mc.y - A_mc.y, 2);
	double target_length = pow(target_top3_x[0].x - target_top3_x[2].x, 2) + pow(target_top3_x[0].y - target_top3_x[2].y, 2);
	double cal_scale = sqrt(target_length) / sqrt(sample_length);
	double sample_angle = atan(fabs(V_mc.y - A_mc.y) / (fabs(V_mc.x - A_mc.x))) * 180 / PI;
	double target_angle = atan(fabs(target_top3_x[0].y - target_top3_x[2].y) / (fabs(target_top3_x[0].x - target_top3_x[2].x))) * 180 / PI;;
	double rotation_angle = sample_angle - target_angle;
	printf("Rotation Angle = %.2f Scale = %.2f\n", rotation_angle, cal_scale);
	//printf("Sample Angle = %.2f, Target Angle = %.2f\n", sample_angle, target_angle);
	Mat rota;
	rotateImage(center,original, rota, rotation_angle, cal_scale);
	//imshow("After Calibrate", rota);
	Mat rota_write;
	rota_write = rota.clone();
	char S[50];
	sprintf_s(S, "Rotation Angle = %.2f", rotation_angle);
	putText(rota_write, string(S), Point(10, 50), 0, 1, Scalar(0, 0, 255), 1,LINE_AA);
	sprintf_s(S, "Scale = %.2f", cal_scale);
	putText(rota_write, string(S), Point(10, 90), 0, 1, Scalar(0, 0, 255), 1, LINE_AA);
	imwrite("After Calibrate.bmp", rota_write);
	Mat after_cal;
	cvtColor(rota, after_cal, COLOR_BGR2GRAY);
	threshold(after_cal, after_cal, 140, 255, THRESH_BINARY);
	threshold(target, target, 140, 255, THRESH_BINARY);
	
	matchTemplate(after_cal, target, result, TM_SQDIFF_NORMED);
	double minVal;
	Point minLoc;
	minMaxLoc(result, &minVal, 0, &minLoc, 0);
	rectangle(rota, minLoc, Point(minLoc.x + target.cols, minLoc.y + target.rows), Scalar(255,50,50), 2);
	//printf("minVal = %.2f\n", minVal);
	//imshow("INPUT", after_cal);
	//imshow("Target", target);
	//imshow("result", rota);
	Mat rota_write2;
	rota_write2 = rota.clone();
	char F[30];
	sprintf_s(F, "Simular Ratio = %.2f", minVal);
	putText(rota_write2, string(F), Point(10, 50), 0, 1, Scalar(0, 255, 255), 1, LINE_AA);

	if (minVal >= 0.2) {
		putText(rota_write2, "BAD Image", Point(10, 90), 0, 1, Scalar(0, 0, 255), 1, LINE_AA);
	}
	else {
		putText(rota_write2, "GOOD Image", Point(10, 90), 0, 1, Scalar(0, 255, 0), 1, LINE_AA);
	}

	imwrite("Result.bmp", rota_write2);
	imshow("Result", rota_write2);

	if (minVal >= 0.2) {
		printf("This is a BAD Image\n");
	}
	else {
		printf("This is a GOOD Image\n");
	}

	waitKey(0);

	return 0;
}


void drawHistImg(const Mat &src, Mat &dst) {
	int histSize = 256;
	float histMaxValue = 0;
	for (int i = 0; i < histSize; i++) {
		float tempValue = src.at<float>(i);
		if (histMaxValue < tempValue) {
			histMaxValue = tempValue;
		}
	}

	float scale = (0.9 * 256) / histMaxValue;
	for (int i = 0; i < histSize; i++) {
		int intensity = static_cast<int>(src.at<float>(i)*scale);
		line(dst, Point(i, 255), Point(i, 255 - intensity), Scalar(0));
	}
}


void rotateImage(const Point center,const Mat &src, Mat &dst, double degree, double scale)
{
	Mat rot_mat = getRotationMatrix2D(center, degree, scale);
	warpAffine(src, dst, rot_mat, dst.size());
	//imshow("Affine_2", dst);
}