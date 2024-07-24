#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "FaceRec.h"

int main()
{
	int choice;
	std::cout << "1. Recognise Face\n";
	std::cout << "2. Add Face\n";
	std::cout << "Enter an Answer: ";
	std::cin >> choice;

	switch (choice)
	{
	case 1:
		FaceRecognition();

		break;
	case 2:
		addFace();
		eigenFaceTrainer();

		break;
	default:
		return 0;
	}

	//system("pause");
	return 0;
}