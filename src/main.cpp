#include <renderer.h>
#include <goal_path_calculator.h>
#include <opencv2/opencv.hpp>

typedef GoalPathCalculator GPC;
int main() {
	GPC *calc = new GPC(0.5, 0.01);
	//Renderer::init();
	//Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	std::vector<GPC::Waypoint> points;

	points.push_back(GPC::Waypoint(pi / 2.0, cv::Point2f(0.0, 0.0)));
	points.push_back(GPC::Waypoint(pi / 2.0, cv::Point2f(5.0, 5.0)));
	size_t iter = 0;
	while (true) {
		for (float i = 0; i < (pi * 2.0); i+= pi/512.0) {
			std::cout << "==================================================" << std::endl;
			points[0].direction = i;
			auto path = calc->calculate_path(&points);
			iter++;
			//Renderer::objects.push_back(&path);
			//Renderer::update(true);
			//Renderer::objects.pop_back();
		}
	}
}
