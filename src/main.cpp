#include <renderer.h>
#include <goal_path_calculator.h>
#include <opencv2/opencv.hpp>

typedef GoalPathCalculator GPC;
int main() {
	GPC *calc = new GPC(0.5, 100.0);
	Renderer::init();
	Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	calc->calculate_path(pi / 2.0, cv::Point2f(10.0, 5.0));
	Renderer::update(true);

	while (true) {
		for (float i = 0; i < (pi * 2.0); i+= pi/256.0) {
			auto path = calc->calculate_path(i, cv::Point2f(10.0, 5.0));
			Renderer::objects.push_back(&path);
			Renderer::update(false);
			Renderer::objects.pop_back();
		}
	}
}
