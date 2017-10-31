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
#include <spline_renderwrap.h>

int main() {
	tinyspline::BSpline spline(6, 3, 3);
	std::vector<tinyspline::real> ctrlp = spline.ctrlp();
	ctrlp[0] = -1.75;
	ctrlp[1] = -1.0;
	ctrlp[2] = 0.0;

	ctrlp[3] = -1.5;
	ctrlp[4] = -0.5;
	ctrlp[5] = 0.0;

	ctrlp[6] = -1.5;
	ctrlp[7] = 0.0;
	ctrlp[8] = 0.0;

	ctrlp[9] = -1.25;
	ctrlp[10] = 0.5;
	ctrlp[11] = 0.0;

	ctrlp[12] = -0.75;
	ctrlp[13] = 0.75;
	ctrlp[14] = 0.0;

	ctrlp[15] = 0.0;
	ctrlp[16] = 0.5;
	ctrlp[17] = 0.0;
	spline.setCtrlp(ctrlp);

	SplineRenderWrap *wrap = new SplineRenderWrap(&spline);
	Renderer::init();
	Renderer::objects.push_back(wrap);

	while (true) {
		for (int i = 0; i < spline.nCtrlp() * spline.dim(); i += spline.dim()) {
			ctrlp[i] += (float)((rand() % 1000) - 500) / 10000.0;
			ctrlp[i + 1] += (float)((rand() % 1000) - 500) / 10000.0;
		}
		spline.setCtrlp(ctrlp);
		Renderer::update();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}
