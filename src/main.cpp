#include <renderer.h>
#include <goal_path_calculator.h>
#include <opencv2/opencv.hpp>

int main() {
	GoalPathCalculator *calc = new GoalPathCalculator(0.5, 0.1);
	Renderer::init();
	Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	calc->calculate_path(cv::Point2f(5.0, 5.0), pi / 4.0);
	Renderer::update(true);
	while (true) {
		//for (float i = -pi / 2.0; i < (pi / 7.0); i+= pi/512.0) {
		for (float i = 0; i < (pi * 2.0); i+= pi/512.0) {
			auto path = calc->calculate_path(cv::Point2f(5.0, 5.0), i);
			//auto path = calc->calculate_path(cv::Point2f(5.0, 5.0), pi / 3);
			Renderer::objects.push_back(&path);
			Renderer::update(false);
			Renderer::objects.pop_back();
		}
	}
}
