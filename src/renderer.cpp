#include <renderer.h>

namespace Renderer {
	float min_x, max_x, min_y, max_y;
	std::vector<Renderable *> objects; // TODO: Use a smart pointer here
	size_t frame = 0;

	void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	void reshape(int w, int h) { glViewport(0, 0, (GLsizei)w, (GLsizei)h); }

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

	// TODO: Make this relative to actual coords rather than scaled coords
	void grid(float step_x, float step_y, float r, float g, float b) {
		glLineWidth(1);
		glColor3f(r, g, b);
		for (float i = step_y * (int)(min_y / step_y); i < max_y; i += step_y) {
			glBegin(GL_LINES);
			glVertex2f(min_x, i);
			glVertex2f(max_x, i);
			glEnd();
		}
		for (float i = step_x * (int)(min_x / step_x); i < max_x; i += step_x) {
			glBegin(GL_LINES);
			glVertex2f(i, min_y);
			glVertex2f(i, max_y);
			glEnd();
		}
	}

	void bound() {
		min_x = 0.0;
		max_x = 0.0;
		min_y = 0.0;
		max_y = 0.0;
		for (auto &object : objects) {
			object->get_size(&min_x, &max_x, &min_y, &max_y);
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
		// if (frame % 30 == 0) {
		bound();
		//}
		clear();
		grid(1, 1, 0.2, 0.2, 0.2);
		add_border(1.0, 0.0, 0.0);
		for (auto &object : objects) {
			object->render();
		}
		glutSwapBuffers();
		glutPostRedisplay();
		frame++;
	}

	void init() {
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		int argc = 0;
		glutInit(&argc, nullptr);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(1000, 1000);
		glutInitWindowPosition(0, 0);
		glutCreateWindow("Spline Display Out");
		glutReshapeFunc(reshape);
		glutDisplayFunc(display);
		bound();
	}
} // namespace Renderer
