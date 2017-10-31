#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>
class Renderable {
	private:
	public:
		char string_buf[300];
		void draw_string(float x, float y, char *input);
		virtual void render(){};
		virtual void get_size(float *min_x, float *max_x, float *min_y,
				float *max_y){};
};
#endif
