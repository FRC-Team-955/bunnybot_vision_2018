#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>
#include <tinysplinecpp.h>
#include <vector>
namespace SplineRenderer {
extern void update();
extern void init(int size_x, int size_y,
                 std::vector<tinyspline::BSpline *> *splines_i);
} // namespace SplineRenderer
