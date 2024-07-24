#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <direct.h>
#include <fstream>
#include <sstream>

cv::CascadeClassifier faceDetection;
std::string fileName;
std::string name;
int fileNumber = 0;

void detectAndDisplay(cv::Mat frame)
{
	std::vector<cv::Rect> faces;
	cv::Mat frame_gray;
	cv::Mat crop;
	cv::Mat res;
	cv::Mat gray;
	std::string text;
	std::stringstream sstm;

	cv::cvtColor(frame, frame_gray, cv::COLOR_BayerBGGR2GRAY);
	cv::equalizeHist(frame_gray, frame_gray);

	faceDetection.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

	cv::Rect roi_b;
	cv::Rect roi_c;

	size_t ib = 0;
	int ab = 0;

	size_t ic = 0;
	int ac = 0;

	for (ic; ic < faces.size(); ic++)
	{
		roi_c.x = faces[ic].x;
		roi_c.y = faces[ic].y;
		roi_c.width = (faces[ic].width);
		roi_c.height = (faces[ic].height);

		ac = roi_c.width * roi_c.height;

		roi_b.x = faces[ib].x;
		roi_b.y = faces[ib].y;
		roi_b.width = (faces[ib].width);
		roi_b.height = (faces[ib].height);

		crop = frame(roi_b);
		cv::resize(crop, res, cv::Size(128, 128), 0, 0, cv::INTER_LINEAR);
		cv::cvtColor(crop, gray, cv::COLOR_BGR2GRAY);

		std::stringstream ssfn;
		fileName = "C:/Users/Blain/Faces";
		ssfn << fileName.c_str() << name << fileNumber << ".jpg";
		fileName = ssfn.str();
		cv::imwrite(fileName, res);
		fileNumber++;
	}
}

void addFace()
{
	std::cout << "\nEnter your name:  ";
	std::cin >> name;

	cv::VideoCapture capture(0);

	if (!capture.isOpened())
		return;

	if (!faceDetection.load("C:/VisualCodeProjects/FaceDetection/haarcascade_frontalface_alt.xml"))
	{
		std::cerr << "ERROR::COULD NOT LOAD CASCADE FILE::LINE 27" << std::endl;
		exit(EXIT_FAILURE);
	}

	cv::Mat frame;
	std::cout << "\n Capturing your face, Press 'C' 10 times and keep your face in front of the camera.";
	char key;
	int i = 0;

	for (;;)
	{
		capture >> frame;

		detectAndDisplay(frame);
		i++;
		if (i == 10)
		{
			std::cout << "Face added.";
			break;
		}
		int c = cv::waitKey(10);

		if (27 == char(c))
		{
			break;
		}
	}
	return;
}

static void dbread(std::vector<cv::Mat>& images, std::vector<int>& labels)
{
	std::vector<cv::String> fn;
	fileName = "C:/Users/Blain/Faces";
	cv::glob(fileName, fn, false);

	size_t count = fn.size();

	for (size_t i = 0; i < count; i++)
	{
		std::string itsname = "";
		char sep = '/';
		size_t j = fn[i].rfind(sep, fn[i].length());
		if (j != std::string::npos)
		{
			itsname = (fn[i].substr(j + 1, fn[i].length() - j - 6));
		}
		images.push_back(cv::imread(fn[i], 0));
		labels.push_back(atoi(itsname.c_str()));
	}
}

void eigenFaceTrainer()
{
	std::vector<cv::Mat> images;
	std::vector<int> labels;

	dbread(images, labels);
	std::cout << "size of the images is " << images.size() << std::endl;
	std::cout << "size of the labels is " << labels.size() << std::endl;
	std::cout << "Training....." << std::endl;


	// create algorithm recognizer
	cv::Ptr<cv::face::EigenFaceRecognizer> model = cv::face::EigenFaceRecognizer::create();

	// train data
	model->train(images, labels);
	model->save("C:/Users/Blain/Faces/eigenface.yml");

	std::cout << "Training finished..." << std::endl;
	cv::waitKey(10000);
}

void FaceRecognition()
{
	std::cout << "Starting facial recognition..." << std::endl;

	cv::Ptr<cv::face::FaceRecognizer> model = cv::face::FisherFaceRecognizer::create();
	model->read("C:/Users/Blain/Faces/eigenface.yml");

	cv::Mat testSample = cv::imread("C:/Users/Blain/Faces/0.jpg", 0);

	int img_width = testSample.cols;
	int img_height = testSample.rows;

	std::string window = "Capture - Face Recognition";

	if (!faceDetection.load("C:/VisualCodeProjects/FaceDetection/haarcascade_frontalface_alt.xml"))
	{
		std::cerr << "ERROR::FACE_RECOGNITION::LINE 164::COULD_NOT_LOAD_FILE" << std::endl;
		exit(EXIT_FAILURE);
	}

	cv::VideoCapture cap(0);

	if (!cap.isOpened())
	{
		std::cout << "EXITING..." << std::endl;
		return;
	}

	cv::namedWindow(window, 1);
	long count = 0;
	std::string Pname = "";

	while (true)
	{
		std::vector<cv::Rect> faces;
		cv::Mat frame;
		cv::Mat grayFrame;
		cv::Mat original;

		cap >> frame;
		count++;

		if (!frame.empty())
		{
			original = frame.clone();

			cv::cvtColor(original, grayFrame, cv::COLOR_BayerBG2GRAY);

			faceDetection.detectMultiScale(grayFrame, faces, 1.1, 3, 0, cv::Size(90, 90));
			std::string frameset = std::to_string(count);
			std::string faceset = std::to_string(faces.size());

			int width = 0, height = 0;

			for (int i = 0; i < faces.size(); i++)
			{
				cv::Rect face_i = faces[i];
				cv::Mat face = grayFrame(face_i);

				cv::Mat face_resized;
				cv::resize(face, face_resized, cv::Size(img_width, img_height), 1.0, 1.0, cv::INTER_CUBIC);

				int label = -1;
				double confidence = 0;
				model->predict(face_resized, label, confidence);

				std::cout << " confidence " << confidence << " label: " << label << std::endl;

				Pname = std::to_string(label);

				cv::rectangle(original, face_i, cv::Scalar(0, 255, 0), 1);
				std::string text = Pname;

				int pos_x = std::max(face_i.tl().x - 10, 0);
				int pos_y = std::max(face_i.tl().y - 10, 0);

				cv::putText(original, text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(0, 255, 0), 1.0);
			}

			cv::putText(original, "Frames: " + frameset, cv::Point(30, 60), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(0, 255, 0), 1.0);
			cv::putText(original, "No. of people detected: " + std::to_string(faces.size()), cv::Point(30, 90), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(0, 255, 0), 1.0);

			cv::imshow(window, original);
		}
		if (cv::waitKey(30) >= 0) break;
	}
}