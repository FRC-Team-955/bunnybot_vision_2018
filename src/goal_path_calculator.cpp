#include <goal_path_calculator.h>

GoalPathCalculator::GoalPathCalculator(float wheel_distance, float step) {
	this->step = step;
	this->wheel_distance = wheel_distance;
}

Path GoalPathCalculator::calculate_path(std::vector<Waypoint> *points) {
		std::cout << spline << std::endl;
	if (this->spline && points->size() != this->spline->dim() * this->spline->nCtrlp()) {
		std::cout << "DELETED" << std::endl;
		delete this->spline;	
	}
	std::cout << spline << std::endl;
	if (!spline) {
		std::cout << "CREATED" << std::endl;
		this->spline = new tinyspline::BSpline(points->size() * 3, 3, 3); //Three dimensions, so we can display it later using native OpenGL functions
		this->ctrlp = this->spline->ctrlp();
		std::cout << spline << std::endl;
	}

	for (size_t i = 0; i < points->size(); i++) {
		size_t spline_idx = i * spline->dim() * 3;
		cv::Point2f position = points->at(i).position;	
		float direction = points->at(i).direction;

		this->ctrlp[spline_idx + 0] = position.x;
		this->ctrlp[spline_idx + 1] = position.y;
		this->ctrlp[spline_idx + 2] = 0.0;

		auto leading_a = MiscMath::RadialOffset(direction, this->wheel_distance, position);
		this->ctrlp[spline_idx + 3] = leading_a.x;
		this->ctrlp[spline_idx + 4] = leading_a.y;
		this->ctrlp[spline_idx + 5] = 0.0;

		auto leading_b = MiscMath::RadialOffset(direction, this->wheel_distance * 2.0, position);
		this->ctrlp[spline_idx + 6] = leading_b.x;
		this->ctrlp[spline_idx + 7] = leading_b.y;
		this->ctrlp[spline_idx + 8] = 0.0;
	}

	spline->setCtrlp(this->ctrlp);
	return Path(this->spline, this->wheel_distance, this->step);
}
