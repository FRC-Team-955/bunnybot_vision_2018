#include <renderer.h>
#include <goal_path_calculator.h>
#include <path_calculator.h>
#include <opencv2/opencv.hpp>
#include <cstring>
#include <socket.h>
#include <vector>

typedef GoalPathCalculator GPC;

void test_graphical();

/*
void client () {
	SocketClient sock (5069, "localhost");
	auto path = Path(&sock);
	for (auto& pt : path.path_left) {
		std::cout << pt.primitive.velocity << " : " << pt.primitive.position << std::endl;
	}
}

void server() {
	GPC *calc = new GPC(0.5, 1000.0);
	const double pi = std::acos(-1);
	auto path = calc->calculate_path(pi / 2.0, cv::Point2f(0.0, 0.0), pi / 4.0, cv::Point2f(3.0, 5.0));

	SocketServer sock(5069);
	path.to_socket(&sock);
}
*/

int main () {
	test_graphical();
	/*
	std::thread server_thread (server);
	std::thread client_thread (client);
	server_thread.join();
	client_thread.join();
	*/
}

void test_graphical () {
	GPC *calc = new GPC(0.5, 1.0, 1.0/10.0); //Wheel distance, max velocity, time step
	Renderer::init();
	Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	calc->calculate_path(pi / 2.0, cv::Point2f(0.0, 0.0), pi / 4.0, cv::Point2f(5.0, 5.0));
	Renderer::update(true);

	while (true) {
		for (float i = 0; i < (pi * 2.0); i+= pi/256.0) {
			auto path = calc->calculate_path(pi / 2.0, cv::Point2f(0.0, 0.0), i, cv::Point2f(5.1, 5.1));
			Renderer::objects.push_back(&path);
			Renderer::update(false);
			Renderer::objects.pop_back();
		}
	}
}
