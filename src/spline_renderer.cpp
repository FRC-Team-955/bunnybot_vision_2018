#include <spline_renderer.h>

namespace SplineRenderer {
	char string_buf[300];
	GLUnurbsObj *nurbs;
	float min_x, max_x, min_y, max_y;
	std::vector<tinyspline::BSpline *> *splines;
	size_t frame = 0;

	void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	void add_spline(tinyspline::BSpline *spline, float r, float g, float b) {
		glColor3f(r, g, b);
		glLineWidth(5);
		gluBeginCurve(nurbs);
		gluNurbsCurve(nurbs, (GLint)spline->nKnots(), &spline->knots()[0],
				(GLint)spline->dim(), &spline->ctrlp()[0],
				(GLint)spline->order(), GL_MAP1_VERTEX_3);
		gluEndCurve(nurbs);
	}

	void add_points(tinyspline::BSpline *spline, float r, float g, float b) {
		glColor3f(r, g, b);
		glPointSize(5);
		glBegin(GL_POINTS);
		for (size_t i = 0; i < spline->nCtrlp(); i++)
			glVertex3fv(&spline->ctrlp()[spline->dim() * i]);
		glEnd();
	}

	void draw_string(float x, float y, char *input) {
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(x, y);
		int len, i;
		len = (int)strlen(input);
		for (i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, input[i]);
		}
	}

	void add_border(float r, float g, float b) {
		glColor3f(r, g, b);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(max_x, max_y);
		glVertex2f(max_x, min_y);

		glVertex2f(max_x, min_y);
		glVertex2f(min_x, min_y);

		glVertex2f(min_x, min_y);
		glVertex2f(min_x, max_y);

		glVertex2f(max_x, max_y);
		glVertex2f(min_x, max_y);
		glEnd();
	}

	void grid(float step_x, float step_y, float r, float g, float b) {
		glLineWidth(1);
		glColor3f(r, g, b);
		for (float i = min_y; i < max_y; i += step_y) {
			glBegin(GL_LINES);
			glVertex2f(min_x, i);
			glVertex2f(max_x, i);
			glEnd();
		}
		for (float i = min_x; i < max_x; i += step_y) {
			glBegin(GL_LINES);
			glVertex2f(i, min_y);
			glVertex2f(i, max_y);
			glEnd();
		}
	}

	void reshape(int w, int h) { glViewport(0, 0, (GLsizei)w, (GLsizei)h); }

	//TODO: MAKE THIS WORK SO YOU CAN DISPLAY DERIVATIVESSS
	void draw_discreet_line(std::vector<tinyspline::real> *points, float r, float g, float b) {
		glLineWidth(5);
		glColor3f(r, g, b);
			glBegin(GL_LINES);
		for (int i = 0; i < points->size(); i+=3) {
			glVertex2f(points->at(i), points->at(i+1));
		}
		glEnd();
	}

	void bound() {
		max_y = 0.0;
		max_x = 0.0;
		min_y = 0.0;
		min_x = 0.0;
		for (auto &spline : *splines) {
			for (int i = 0; i < spline->ctrlp().size(); i += spline->dim()) {
				if (spline->ctrlp()[i] < min_x) {
					min_x = spline->ctrlp()[i];
				}
				if (spline->ctrlp()[i + 1] < min_y) {
					min_y = spline->ctrlp()[i + 1];
				}
				if (spline->ctrlp()[i] > max_x) {
					max_x = spline->ctrlp()[i];
				}
				if (spline->ctrlp()[i + 1] > max_y) {
					max_y = spline->ctrlp()[i + 1];
				}
			}
		}
		glLoadIdentity();
		max_x += 1.0;
		min_x -= 1.0;
		max_y += 1.0;
		min_y -= 1.0;
		gluOrtho2D(min_x, max_x, min_y, max_y);
	}

	void update() { glutMainLoopEvent(); }

	void display(void) {
		if (frame % 30) {
			bound();
		}
		clear();
		grid(1, 1, 0.2, 0.2, 0.2);
		add_border(1.0, 0.0, 0.0);
		for (auto &spline : *splines) {
			add_spline(spline, 1.0, 1.0, 1.0);
			add_points(spline, 1.0, 0.0, 0.0);
			for (int i = 0; i < spline->nCtrlp() * spline->dim(); i += spline->dim()) {
				sprintf(string_buf, "(%d, %d)", (int)spline->ctrlp()[i], (int)spline->ctrlp()[i + 1]);
				draw_string(spline->ctrlp()[i], spline->ctrlp()[i + 1], string_buf);
			}
			auto k = spline->ctrlp();
			draw_discreet_line(&k, 0.0, 1.0, 0.0);
		}
		glutSwapBuffers();
		glutPostRedisplay();
		frame++;
	}

	void init(int size_x, int size_y,
			std::vector<tinyspline::BSpline *> *splines_i) {
		splines = splines_i;
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		nurbs = gluNewNurbsRenderer();
		gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 10.0);

		int argc = 0;
		glutInit(&argc, nullptr);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(size_x, size_y);
		glutInitWindowPosition(0, 0);
		glutCreateWindow("Spline Display Out");
		glutReshapeFunc(reshape);
		glutDisplayFunc(display);
		bound();
	}
} // namespace SplineRenderer
