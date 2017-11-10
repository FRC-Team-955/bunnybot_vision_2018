#include <renderer.h>
#include <goal_path_calculator.h>
#include <opencv2/opencv.hpp>

typedef GoalPathCalculator GPC;
int main() {
	GPC *calc = new GPC(0.5, 0.01);
	Renderer::init();
	Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	calc->calculate_path(pi / 2.0, cv::Point2f(5.0, 5.0));
	Renderer::update(true);

	size_t iter = 0;
	while (true) {
		for (float i = 0; i < (pi * 2.0); i+= pi/512.0) {
			auto path = calc->calculate_path(i, cv::Point2f(5.0, 5.0));
			iter++;
			Renderer::objects.push_back(&path);
			Renderer::update(false);
			Renderer::objects.pop_back();
		}
	}
}
