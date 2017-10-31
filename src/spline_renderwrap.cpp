#include <spline_renderwrap.h>
void SplineRenderWrap::render() {
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(5);
	gluBeginCurve(nurbs);
	gluNurbsCurve(nurbs, (GLint)spline->nKnots(), &spline->knots()[0],
			(GLint)spline->dim(), &spline->ctrlp()[0],
			(GLint)spline->order(), GL_MAP1_VERTEX_3);
	gluEndCurve(nurbs);
	add_points(1.0, 0.0, 0.0);

	for (size_t i = 0; i < spline->nCtrlp() * spline->dim(); i += 3) {
		sprintf(string_buf, "(%f, %f)", spline->ctrlp()[i], spline->ctrlp()[i + 1]);
		this->draw_string(spline->ctrlp()[i], spline->ctrlp()[i + 1], string_buf);
	}
}

void SplineRenderWrap::add_points(float r, float g, float b) {
	glColor3f(r, g, b);
	glPointSize(9);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < spline->nCtrlp(); i++)
		glVertex3fv(&spline->ctrlp()[spline->dim() * i]);
	glEnd();
}

SplineRenderWrap::SplineRenderWrap(tinyspline::BSpline *spline) {
	this->spline = spline;
	this->nurbs = gluNewNurbsRenderer();
	gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 10.0);
}

void SplineRenderWrap::get_size(float *min_x, float *max_x, float *min_y,
		float *max_y) {
	for (int i = 0; i < spline->ctrlp().size(); i += spline->dim()) {
		if (spline->ctrlp()[i] < *min_x) {
			*min_x = spline->ctrlp()[i];
		}
		if (spline->ctrlp()[i + 1] < *min_y) {
			*min_y = spline->ctrlp()[i + 1];
		}
		if (spline->ctrlp()[i] > *max_x) {
			*max_x = spline->ctrlp()[i];
		}
		if (spline->ctrlp()[i + 1] > *max_y) {
			*max_y = spline->ctrlp()[i + 1];
		}
	}
}

// TODO: Add destructor
