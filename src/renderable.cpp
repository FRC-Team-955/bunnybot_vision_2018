#include <renderable.h>

void Renderable::draw_string(float x, float y, char *input) {
	glRasterPos2f(x, y);
	int len = (int)strlen(input);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, input[i]);
}
