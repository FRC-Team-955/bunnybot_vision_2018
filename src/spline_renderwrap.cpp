#include <spline_renderwrap.h>
void SplineRenderWrap::render()
{
    glColor3f(0.8, 0.8, 0.8);
    glLineWidth(5);
    gluBeginCurve(nurbs);
    gluNurbsCurve(nurbs, (GLint)spline->nKnots(), &spline->knots()[0],
        (GLint)spline->dim(), &spline->ctrlp()[0],
        (GLint)spline->order(), GL_MAP1_VERTEX_3);
    gluEndCurve(nurbs);

	glColor3f(1.0, 0.0, 0.0);
	glPointSize(9);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < spline->nCtrlp(); i++)
		glVertex3fv(&spline->ctrlp()[spline->dim() * i]);
	glEnd();

	for (size_t i = 0; i < spline->nCtrlp() * spline->dim(); i += 3) {
		sprintf(string_buf, "(%f, %f)", spline->ctrlp()[i], spline->ctrlp()[i + 1]);
		this->draw_string(spline->ctrlp()[i], spline->ctrlp()[i + 1], string_buf);
	}
}

SplineRenderWrap::SplineRenderWrap()
{
    this->nurbs = gluNewNurbsRenderer();
    gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 10.0);
}

cv::Rect2f SplineRenderWrap::get_size()
{
    cv::Rect2f rect;
    for (int i = 0; i < spline->nCtrlp(); i++) {
        cv::Point2f position = MiscMath::FromCtrlpt(this->spline, i);
        if (position.x < rect.x) {
            rect.x = position.x;
        }
        if (position.y < rect.y) {
            rect.y = position.y;
        }
        if (position.x > rect.br().x) {
            rect.width = fabs(rect.x - position.x);
        }
        if (position.y > rect.br().y) {
            rect.height = fabs(rect.y - position.y);
        }
    }
    return rect;
}

// TODO: Add destructor
