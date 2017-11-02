#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>
#include <opencv2/opencv.hpp>
class Renderable {
	private:
	public:
		char string_buf[300];
		void draw_string(float x, float y, char *input);
		virtual void render(){};
		virtual cv::Rect2f get_size(){
			return cv::Rect2f(0.0, 0.0, 0.0, 0.0);
		};
};
#endif
