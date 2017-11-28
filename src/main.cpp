#include <renderer.h>
#include <goal_path_calculator.h>
#include <path_calculator.h>
#include <opencv2/opencv.hpp>
#include <cstring>
#include <socket.h>
#include <vector>
#include <chrono>

typedef GoalPathCalculator GPC;

void test_graphical();

int main () {
	GPC *calc = new GPC(0.5, 1.0, 1.0/100.0); //Wheel distance, max velocity, time step
	//SocketServer sock(5068);
	Renderer::init();
	Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	calc->calculate_path(pi / 2.0, cv::Point2f(0.0, 0.0), pi / 4.0, cv::Point2f(3.0, 3.0));
	Renderer::update(true);

	while (true) {
		for (float i = 0; i < (pi * 2.0); i+= pi/256.0) {
			Path* path = calc->calculate_path(pi / 2.0, cv::Point2f(0.0, 0.0), i, cv::Point2f(3.0, 3.0));
			Renderer::objects.push_back(path);
			Renderer::update(false);
			Renderer::objects.pop_back();
			//path->to_socket(&sock);
			delete path;
		}
	}
}
