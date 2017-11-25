#ifndef GOAL_PATH_CALCULATOR_H
#define GOAL_PATH_CALCULATOR_H

#include <vector>
#include <spline_renderwrap.h>
#include <path_calculator.h>
class GoalPathCalculator : public SplineRenderWrap {
	public:
		struct Waypoint {
			float direction;
			cv::Point2f position;
			Waypoint(float direction, cv::Point2f position) {
				this->direction = direction;
				this->position = position;
			}
		};
		GoalPathCalculator(float wheel_distance, float max_allowed_velocity, float max_time_step);
		Path* calculate_path(float direction_start, cv::Point2f position_start, float direction_end, cv::Point2f position_end);
		using SplineRenderWrap::render;
		using SplineRenderWrap::get_size;

	private:
		float max_allowed_velocity; //The max allowed velocity 
		float max_time_step; //How much time does each point last for?
		float wheel_distance; //Wheel distance from the center of the robot
		std::vector<tinyspline::real> ctrlp;
};
#endif
