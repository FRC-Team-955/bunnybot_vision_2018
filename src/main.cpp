#include <renderer.h>
#include <goal_path_calculator.h>
#include <path_calculator.h>
#include <opencv2/opencv.hpp>
#include <cstring>
#include <socket.h>
#include <mutex>
#include <vector>
#include <chrono>
#include <thread>

typedef GoalPathCalculator GPC;

void test_graphical();

Path* latest_path = nullptr;
std::mutex latest_path_mutex;

void server() {
	SocketServer sock(5068);
	while (1) {
		latest_path_mutex.lock();
		if (latest_path) {
			latest_path->to_socket(&sock);
			latest_path_mutex.unlock();
		} else {
			latest_path_mutex.unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
	}
}

int main () {
	GPC *calc = new GPC(0.5, 1.0, 1.0/100.0); //Wheel distance, max velocity, time step
	std::thread server_thread (server);
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
			latest_path_mutex.lock();
			latest_path = path;
			latest_path_mutex.unlock();
			delete path;
		}
	}
	server_thread.join();
}
