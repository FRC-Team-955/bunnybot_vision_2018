#include <path_calculator.h>
Path::Path(tinyspline::BSpline* spline, float wheel_distance, float max_change_time)
{
	auto derive = spline->derive();
	auto derive_sq = derive.derive();
	cv::Point2f left_last(0.0, 0.0); //TODO: Set these to the origin/first instance
	cv::Point2f right_last(0.0, 0.0);
	float left_accum = 0.0;
	float right_accum = 0.0;

	float i = 0.0;
	while (i < 1.0) {
		//Calculate spline evaluations
		auto point_sp = spline->evaluate(i).result();
		float max_allowed_velocity = point_sp[2];
		auto point_dr = derive.evaluate(i).result();
		auto point_dr_sq = derive_sq.evaluate(i).result();
		cv::Point2f point_sp_cv = cv::Point2f(point_sp[0], point_sp[1]);
		cv::Point2f point_dr_cv = cv::Point2f(point_dr[0], point_dr[1]);
		cv::Point2f point_dr_sq_cv = cv::Point2f(point_dr_sq[0], point_dr_sq[1]);

		//How much change in dist do we expect?
		float sum_dr_squares = powf(point_dr_cv.x, 2.0) + powf(point_dr_cv.y, 2.0); //TODO: Just use multiplication here instead of powf?
		float speed_center = sqrtf(sum_dr_squares); //Distance between this point and the next

		//Slope of the center line
		float slope = point_dr_cv.y / point_dr_cv.x;

		//Create paths for each wheel
		cv::Point2f point_norm_raw_cv = cv::Point2f(-point_dr_cv.y, point_dr_cv.x);
		cv::Point2f point_norm_raw_sq_cv = cv::Point2f(-point_dr_sq_cv.y, point_dr_sq_cv.x);
		cv::Point2f point_norm_cv = (point_norm_raw_cv / speed_center) * wheel_distance;
		cv::Point2f left = point_sp_cv + point_norm_cv;
		cv::Point2f right = point_sp_cv - point_norm_cv;

		//Get the dist travelled by each wheel
		float left_dist = MiscMath::PointDistance(left, left_last);
		float right_dist = MiscMath::PointDistance(right, right_last);

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
		left_accum += (speed_left / speed_max) * max_allowed_velocity * max_change_time;
		right_accum += (speed_right / speed_max) * max_allowed_velocity * max_change_time;

		//Add path elements
		path_left.push_back(TalonPoint(left_accum, max_allowed_velocity * (speed_left / speed_max) * reverse_left, left)); 
		path_right.push_back(TalonPoint(right_accum, max_allowed_velocity * (speed_right / speed_max) * reverse_right, right)); 

		//Copy over positions and slope for next iteration
		left_last = left;
		right_last = right;

		i += (1.0 / speed_max) * max_allowed_velocity * max_change_time;
	}
	std::cout << left_accum << " : " << right_accum << std::endl;
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
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(9);
	//glBegin(GL_POINTS);
	glBegin(GL_LINES);

	glVertex2f(path_left.front().display_point.x, path_left.front().display_point.y);
	for (auto& left : this->path_left) {
		color_by(left.primitive.velocity);
		glVertex2f(left.display_point.x, left.display_point.y);
		glVertex2f(left.display_point.x, left.display_point.y);
	}
	glVertex2f(path_left.back().display_point.x, path_left.back().display_point.y);

	glVertex2f(path_right.front().display_point.x, path_right.front().display_point.y);
	glColor3f(0.0, 1.0, 1.0);
	for (auto& right : this->path_right) {
		color_by(right.primitive.velocity);
		glVertex2f(right.display_point.x, right.display_point.y);
		glVertex2f(right.display_point.x, right.display_point.y);
	}
	glVertex2f(path_right.back().display_point.x, path_right.back().display_point.y);

	glEnd();
}

cv::Rect2f Path::get_size()
{
	cv::Rect2f rect;
	for (auto& point : path_right) {
		cv::Point2f position = point.display_point;
		if (position.x < rect.x) {
			rect.x = position.x;
		}
		if (position.y < rect.y) {
			rect.y = position.y;
		}
		if (position.x > rect.br().x) {
			rect.width = fabs(rect.x - position.x);
		}
		if (position.y > rect.br().y) {
			rect.height = fabs(rect.y - position.y);
		}
	}
	for (auto& point : path_left) {
		cv::Point2f position = point.display_point;
		if (position.x < rect.x) {
			rect.x = position.x;
		}
		if (position.y < rect.y) {
			rect.y = position.y;
		}
		if (position.x > rect.br().x) {
			rect.width = fabs(rect.x - position.x);
		}
		if (position.y > rect.br().y) {
			rect.height = fabs(rect.y - position.y);
		}
	}
	return rect;
}

//TODO: Use boost?
void Path::to_socket (Socket* sock) {
	size_t length = this->path_left.size();
	sock->write_to(&length, sizeof(size_t));

	for (auto& point : this->path_left)
		sock->write_to(&point.primitive, sizeof(Path::PrimitivePoint));

	for (auto& point : this->path_right)
		sock->write_to(&point.primitive, sizeof(Path::PrimitivePoint));
}

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

//TODO: Add concat function for paths
