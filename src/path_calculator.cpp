#include <path_calculator.h>
Path::Path(tinyspline::BSpline spline, float wheel_distance, float max_change_time)
{
	this->spline = spline;
	this->spline_derive = this->spline.derive();
	this->spline_derive_sq = this->spline_derive.derive();
	this->wheel_distance = wheel_distance;
	this->max_change_time = max_change_time;
}

bool Path::next_point (TalonPoint* output) { //TODO: Optionally return these coords or display them
	cv::Point2f dummy_left;
	cv::Point2f dummy_right;
	return next_point_raw(output, &this->official_traversal, &dummy_left, &dummy_right);
}

bool Path::next_point_raw (TalonPoint* output, Traversal* traversal, cv::Point2f* out_left, cv::Point2f* out_right) {
		//Calculate spline evaluations
		auto point_sp = spline.evaluate(traversal->spline_index).result();
		auto point_dr = spline_derive.evaluate(traversal->spline_index).result();
		auto point_dr_sq = spline_derive_sq.evaluate(traversal->spline_index).result();
		cv::Point2f point_sp_cv = cv::Point2f(point_sp[0], point_sp[1]);
		cv::Point2f point_dr_cv = cv::Point2f(point_dr[0], point_dr[1]);
		cv::Point2f point_dr_sq_cv = cv::Point2f(point_dr_sq[0], point_dr_sq[1]);

		//We hijack the third axis (Z) to use it as a velocity max set point. heh.
		float max_allowed_velocity = point_sp[2];

		//How much change in dist do we expect?
		float sum_dr_squares = powf(point_dr_cv.x, 2.0) + powf(point_dr_cv.y, 2.0); //TODO: Just use multiplication here instead of powf?
		float speed_center = sqrtf(sum_dr_squares); //Distance between this point and the next

		//Slope of the center line
		float slope = point_dr_cv.y / point_dr_cv.x;

		//Create paths for each wheel
		cv::Point2f point_norm_raw_cv = cv::Point2f(-point_dr_cv.y, point_dr_cv.x);
		cv::Point2f point_norm_raw_sq_cv = cv::Point2f(-point_dr_sq_cv.y, point_dr_sq_cv.x);
		cv::Point2f point_norm_cv = (point_norm_raw_cv / speed_center) * wheel_distance;
		*out_left = point_sp_cv + point_norm_cv;
		*out_right = point_sp_cv - point_norm_cv;

		//Rate of travel in center over i
		float dr_speed_center = (point_dr_sq_cv.x * point_dr_cv.x) + (point_dr_sq_cv.y * point_dr_cv.y) / speed_center;

		//How much should we move the derivative from the original
		cv::Point2f dr_offset_speed = (wheel_distance * ((point_norm_raw_sq_cv * speed_center) - (point_norm_raw_cv * dr_speed_center)) / sum_dr_squares);

		//Calculate left speed with some calculus
		cv::Point2f dr_left = point_dr_cv + dr_offset_speed; 
		float speed_left = sqrtf(powf(dr_left.x, 2.0) + powf(dr_left.y, 2.0));

		//Calculate right speed with some more calculus
		cv::Point2f dr_right = point_dr_cv - dr_offset_speed;
		float speed_right = sqrtf(powf(dr_right.x, 2.0) + powf(dr_right.y, 2.0));
		
		//The max of the two is the farthest distance
		float speed_max = std::max(speed_left, speed_right);

		//Find change in angle
		double change_in_slope = ((point_dr_sq_cv.y*point_dr_cv.x) - (point_dr_sq_cv.x*point_dr_cv.y)) / powf(point_dr_cv.x, 2.0);
		double change_in_angle = (1.0 / (1.0 + powf(point_dr_cv.y / point_dr_cv.x, 2.0))) * change_in_slope;
		float reverse_left = change_in_angle > pi * 2.0 ? -1.0 : 1.0;
		float reverse_right = -change_in_angle > pi * 2.0 ? -1.0 : 1.0;

		//Accumulate distances, assuming the longest side always goes one distance unit
		traversal->left_accum += (speed_left / speed_max) * max_allowed_velocity * max_change_time;
		traversal->right_accum += (speed_right / speed_max) * max_allowed_velocity * max_change_time;

		float absolute_velocity_left = max_allowed_velocity * (speed_left / speed_max) * reverse_left; 
		float absolute_velocity_right = max_allowed_velocity * (speed_right / speed_max) * reverse_right; 
		
		traversal->spline_index += (1.0 / speed_max) * max_allowed_velocity * max_change_time;
		output->position_left = traversal->left_accum;
		output->velocity_left = absolute_velocity_left;
		output->position_right = traversal->right_accum;
		output->velocity_right = absolute_velocity_right;
		output->is_end = traversal->spline_index >= 1.0;
		return traversal->spline_index < 1.0; //Allow further reads if we're not too far
}

void Path::color_by(float input) { //Green to black to red from 1.0 to 0.0 to -1.0 respectively
	if (input > 0) {
		glColor3f(0.0, input, 0.0);
	} else {
		glColor3f(fabs(input), 0.0, 0.0);
	}
}

void Path::render()
{
	Traversal traversal;
	TalonPoint current_talonpoint;
	cv::Point2f last_left;
	cv::Point2f last_right;
	cv::Point2f current_left;
	cv::Point2f current_right;
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(9);
	glBegin(GL_LINES);

	next_point_raw(&current_talonpoint, &traversal, &current_left, &current_right);
	last_left = current_left;
	last_right = current_right;
	while(next_point_raw(&current_talonpoint, &traversal, &current_left, &current_right)) {
		color_by(current_talonpoint.velocity_left);
		glVertex2f(last_left.x, last_left.y);
		glVertex2f(current_left.x, current_left.y);

		color_by(current_talonpoint.velocity_right);
		glVertex2f(last_right.x, last_right.y);
		glVertex2f(current_right.x, current_right.y);
		last_left = current_left;
		last_right = current_right;
	}

	glEnd();
}

cv::Rect2f Path::get_size()
{
	cv::Rect2f rect;
	//TODO: Actually do something here :P
	return rect;
}

/*
	void Path::to_socket (Socket* sock) {
	size_t length = this->points.size();
	sock->write_to(&length, sizeof(size_t));

	for (auto& point : this->points) {
	bool keep_running = true;
	sock->read_to(&keep_running, sizeof(keep_running));
	if (!keep_running) break;
	sock->write_to(&point.primitive, sizeof(Path::PrimitivePoint));
	}
	}
	*/

/*
	Path::Path(Socket* sock) {
	size_t points;
	sock->read_to(&points, sizeof(points));

	char buffer[sizeof(Path::PrimitivePoint)];
	for (size_t i = 0; i < points; i++) {
	sock->read_to(&buffer, sizeof(Path::PrimitivePoint));
	path_left.push_back(TalonPoint(reinterpret_cast<Path::PrimitivePoint*>(buffer), cv::Point2f(0.0, 0.0)));
	}
	for (size_t i = 0; i < points; i++) {
	sock->read_to(&buffer, sizeof(Path::PrimitivePoint));
	path_right.push_back(TalonPoint(reinterpret_cast<Path::PrimitivePoint*>(buffer), cv::Point2f(0.0, 0.0)));
	}
	}
	*/

//TODO: Add concat function for paths
