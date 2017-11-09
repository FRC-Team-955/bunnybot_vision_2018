#ifndef GOAL_PATH_CALCULATOR_H
#define GOAL_PATH_CALCULATOR_H

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
		GoalPathCalculator(float wheel_distance, float step);
		Path calculate_path(cv::Point2f position, float direction);
		using SplineRenderWrap::render;
		using SplineRenderWrap::get_size;

	private:
		float step; //The step 
		float wheel_distance; //Wheel distance from the center of the robot
		std::vector<tinyspline::real> ctrlp;
};
#endif
