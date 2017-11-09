#ifndef SPLINE_RENDERWRAP_H
#define SPLINE_RENDERWRAP_H
#include <opencv2/opencv.hpp>
#include <renderable.h>
#include <tinysplinecpp.h>
#include <misc_math.h>
#include <cmath>
class SplineRenderWrap : public Renderable {
	private:
		GLUnurbsObj *nurbs;

	public:
		tinyspline::BSpline* spline = nullptr;
		SplineRenderWrap();
		void render();
		cv::Rect2f get_size();
};
#endif
