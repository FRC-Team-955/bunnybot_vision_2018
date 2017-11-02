#ifndef PATH_CALCULATOR_H
#define PATH_CALCULATOR_H
#include <cmath>
#include <misc_math.h>
#include <opencv2/opencv.hpp>
#include <spline_renderwrap.h>
#include <tinysplinecpp.h>
#include <vector>
/*
 * TODO: Make an interface class for path calculators, this one only works
 * with tank drive controls
 * TODO: Make PathCalculator inherit from Renderable
 */

class Path : public Renderable {
public:
    struct TalonPoint {
        float position;
        float velocity;
        cv::Point2f display_point;
        TalonPoint(float position, float velocity, cv::Point2f display_point)
        {
            this->display_point = display_point;
            this->position = position;
            this->velocity = velocity;
        }
    };
    std::vector<TalonPoint> path_left;
    std::vector<TalonPoint> path_right;
    Path(tinyspline::BSpline* spline, float wheel_distance, float step);
    virtual void render();
    virtual cv::Rect2f get_size();
};

class PathCalculator : public Renderable {
public:
    struct Goal { // Contains a position and a vector for an endpoint
        cv::Point2f position;
        float direction; //Radians
        Goal(cv::Point2f position, float direction)
        {
            this->position = position;
            this->direction = direction;
        }
    };
    PathCalculator(float interval, float outcrop, float wheel_distance, float step);
    Path calculate_path(Goal goal);
    virtual void render();
    virtual cv::Rect2f get_size();

private:
    float step; //The step 
    float wheel_distance; //Wheel distance from the center of the robot
    float interval; //Milliseconds between each talon update
    float outcrop; //How far to define the next point in the spline (Makes movements sharper at lower values)
    tinyspline::BSpline* spline;
    std::vector<tinyspline::real> ctrlp;
    SplineRenderWrap* spline_renderer;
};
#endif
