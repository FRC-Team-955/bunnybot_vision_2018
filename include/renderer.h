#ifndef RENDERER_H
#define RENDERER_H
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <renderable.h>
#include <vector>
namespace Renderer {
	extern void init();
	extern void update();
	extern std::vector<Renderable *> objects;
} // namespace Renderer
#endif
