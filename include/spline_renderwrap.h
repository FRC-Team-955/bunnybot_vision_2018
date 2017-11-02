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
		tinyspline::BSpline *spline;

	public:
		SplineRenderWrap(tinyspline::BSpline *spline);
		virtual void render();
		virtual cv::Rect2f get_size();
};
#endif
