#ifndef GOAL_PATH_CALCULATOR_H
#define GOAL_PATH_CALCULATOR_H

#include <path_calculator.h>
class GoalPathCalculator : public PathCalculator {
	public:
		GoalPathCalculator(float wheel_distance, float step);
		using PathCalculator::render;
		using PathCalculator::get_size;
		Path calculate_path(cv::Point2f position, float direction);

	private:
		float step; //The step 
		float wheel_distance; //Wheel distance from the center of the robot
};
#endif
