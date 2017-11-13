#include <renderer.h>
#include <goal_path_calculator.h>
#include <path_calculator.h>
#include <opencv2/opencv.hpp>
#include <cstring>
#include <thread>
#include <socket.h>
#include <vector>

typedef GoalPathCalculator GPC;

//void test_graphical();

void client () {
	SocketClient sock (5068, "localhost");
	auto path = Path(&sock);
}

void server() {
	GPC *calc = new GPC(0.5, 100.0);
	const double pi = std::acos(-1);
	auto path = calc->calculate_path(pi / 2.0, cv::Point2f(5.0, 5.0));

	SocketServer sock(5068);
	path.to_socket(&sock);
}

int main () {
	std::thread server_thread (server);
	std::thread client_thread (client);
	server_thread.join();
	client_thread.join();
}

/*
	void test_graphical () {
	GPC *calc = new GPC(0.5, 100.0);
	Renderer::init();
	Renderer::objects.push_back(calc);

	const double pi = std::acos(-1);
	calc->calculate_path(pi / 2.0, cv::Point2f(5.0, 5.0));
	Renderer::update(true);

	while (true) {
	for (float i = 0; i < (pi * 2.0); i+= pi/256.0) {
	auto path = calc->calculate_path(i, cv::Point2f(5.0, 5.0));
	std::cout << "Left: " << path.path_left.size() << " Right: " << path.path_right.size() << std::endl;
	Renderer::objects.push_back(&path);
	Renderer::update(false);
	Renderer::objects.pop_back();
	}
	}
	}
	*/
