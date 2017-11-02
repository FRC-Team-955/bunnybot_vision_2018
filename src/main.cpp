#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <thread>

#include <tinysplinecpp.h>

#include <renderable.h>
#include <renderer.h>
#include <path_calculator.h>
#include <opencv2/opencv.hpp>

int main() {
	PathCalculator *calc = new PathCalculator(5.0, 0.5, 0.2, 0.01);
	Renderer::init();
	Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	calc->calculate_path(PathCalculator::Goal(cv::Point2f(5.0, 5.0), pi / 4.0));
	Renderer::update(true);
	while (true) {
		for (float i = 0; i < 2*pi; i+= pi/512.0) {
			auto path = calc->calculate_path(PathCalculator::Goal(cv::Point2f(5.0, 5.0), i));
			Renderer::objects.push_back(&path);
			Renderer::update(false);
			Renderer::objects.pop_back();
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}
}
