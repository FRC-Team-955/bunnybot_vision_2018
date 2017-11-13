#include <goal_path_calculator.h>

GoalPathCalculator::GoalPathCalculator(float wheel_distance, float step) {
	this->step = step;
	this->wheel_distance = wheel_distance;
	this->spline = new tinyspline::BSpline(6, 3, 3); //Three dimensions, so we can display it later using native OpenGL functions
	this->ctrlp = this->spline->ctrlp();
}

Path GoalPathCalculator::calculate_path(float direction, cv::Point2f position) {
	this->ctrlp[0] = 0.0;
	this->ctrlp[1] = 0.0;
	this->ctrlp[2] = 0.0;

	this->ctrlp[3] = 0.0;
	this->ctrlp[4] = this->wheel_distance;
	this->ctrlp[5] = 0.0;

	this->ctrlp[6] = 0.0;
	this->ctrlp[7] = this->wheel_distance * 2.0;
	this->ctrlp[8] = 0.0;

	auto leading_a = MiscMath::RadialOffset(direction, this->wheel_distance * 2.0, position);
	this->ctrlp[9] = leading_a.x;
	this->ctrlp[10] = leading_a.y;
	this->ctrlp[11] = 0.0;

	auto leading_b = MiscMath::RadialOffset(direction, this->wheel_distance, position);
	this->ctrlp[12] = leading_b.x;
	this->ctrlp[13] = leading_b.y;
	this->ctrlp[14] = 0.0;

	this->ctrlp[15] = position.x;
	this->ctrlp[16] = position.y;
	this->ctrlp[17] = 0.0;

	spline->setCtrlp(this->ctrlp);
	return Path(this->spline, this->wheel_distance, this->step);
}
