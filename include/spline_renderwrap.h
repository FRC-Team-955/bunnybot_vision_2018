#ifndef SPLINE_RENDERWRAP_H
#define SPLINE_RENDERWRAP_H
#include <renderable.h>
#include <tinysplinecpp.h>
class SplineRenderWrap : public Renderable {
	private:
		GLUnurbsObj *nurbs;
		tinyspline::BSpline *spline;

	public:
		SplineRenderWrap(tinyspline::BSpline *spline);
		virtual void render();
		virtual void get_size(float *min_x, float *max_x, float *min_y, float *max_y);
		void add_spline(float r, float g, float b);
		void add_points(float r, float g, float b);
};
#endif
