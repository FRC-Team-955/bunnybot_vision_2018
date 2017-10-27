#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <spline_renderer.h>
#include <stdlib.h>
#include <thread>
#include <tinysplinecpp.h>

using namespace SplineRenderer;

int main() {
	tinyspline::BSpline spline(4, 3, 3);

	std::vector<tinyspline::real> ctrlp = spline.ctrlp();
	ctrlp[0] = 0.0f; // x
	ctrlp[1] = 1.0f; // y
	ctrlp[2] = 0.0f; // z

	ctrlp[3] = 0.0f; // x
	ctrlp[4] = 2.0f; // y
	ctrlp[5] = 0.0f; // z

	ctrlp[6] = 5.0f; // x
	ctrlp[7] = 4.0f; // y
	ctrlp[8] = 0.0f; // z

	ctrlp[9] = 5.0f;  // x
	ctrlp[10] = 5.0f; // y
	ctrlp[11] = 0.0f; // z

	spline.setCtrlp(ctrlp);

	std::vector<tinyspline::BSpline *> splines;
	splines.push_back(&spline);
	SplineRenderer::init(1600, 800, &splines);

	while (true) {
		for (int i = 0; i < spline.nCtrlp() * spline.dim(); i += spline.dim()) {
			ctrlp[i] += (float)((rand() % 1000) - 500) / 10000.0;
			ctrlp[i + 1] += (float)((rand() % 1000) - 500) / 10000.0;
		}
		spline.setCtrlp(ctrlp);
		SplineRenderer::update();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
