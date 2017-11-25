#include <goal_path_calculator.h>

GoalPathCalculator::GoalPathCalculator(float wheel_distance, float max_allowed_velocity, float max_time_step) {
	this->max_allowed_velocity = max_allowed_velocity;
	this->max_time_step = max_time_step;
	this->wheel_distance = wheel_distance;
	this->spline = new tinyspline::BSpline(6, 3, 3); //Three dimensions, so we can display it later using native OpenGL functions
	this->ctrlp = this->spline->ctrlp();
}

Path* GoalPathCalculator::calculate_path(float direction_start, cv::Point2f position_start, float direction_end, cv::Point2f position_end) {
	this->ctrlp[0] = position_start.x;
	this->ctrlp[1] = position_start.y;
	this->ctrlp[2] = 0.01;

	auto leading_a_start = MiscMath::RadialOffset(direction_start, this->wheel_distance, position_start);
	this->ctrlp[3] = leading_a_start.x;
	this->ctrlp[4] = leading_a_start.y;
	this->ctrlp[5] = max_allowed_velocity;

	auto leading_b_start = MiscMath::RadialOffset(direction_start, this->wheel_distance * 2.0, position_start);
	this->ctrlp[6] = leading_b_start.x;
	this->ctrlp[7] = leading_b_start.y;
	this->ctrlp[8] = max_allowed_velocity;

	auto leading_a_end = MiscMath::RadialOffset(direction_end, this->wheel_distance * 2.0, position_end);
	this->ctrlp[9] = leading_a_end.x;
	this->ctrlp[10] = leading_a_end.y;
	this->ctrlp[11] = max_allowed_velocity;

	auto leading_b_end = MiscMath::RadialOffset(direction_end, this->wheel_distance, position_end);
	this->ctrlp[12] = leading_b_end.x;
	this->ctrlp[13] = leading_b_end.y;
	this->ctrlp[14] = max_allowed_velocity;

	this->ctrlp[15] = position_end.x;
	this->ctrlp[16] = position_end.y;
	this->ctrlp[17] = 0.01;

	spline->setCtrlp(this->ctrlp);
	return new Path(this->spline, this->wheel_distance, this->max_time_step);
}
