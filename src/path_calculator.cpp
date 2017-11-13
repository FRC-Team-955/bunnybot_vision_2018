#include <path_calculator.h>
Path::Path(tinyspline::BSpline* spline, float wheel_distance, float step)
{
	auto derive = spline->derive();
	auto derive_sq = derive.derive();
	cv::Point2f left_last(0.0, 0.0);
	cv::Point2f right_last(0.0, 0.0);
	float left_accum = 0.0;
	float right_accum = 0.0;

	//DIRTY HACK for inflection points
	bool inflect_left = true;
	bool inflect_right = true;

	float i = 0.0;
	while (i < 1.0) {
		//Calculate spline evaluations
		auto point_sp = spline->evaluate(i).result();
		auto point_dr = derive.evaluate(i).result();
		auto point_dr_sq = derive_sq.evaluate(i).result();

		//How much change in distance do we expect?
		float dist = sqrtf(powf(point_dr[0], 2.0) + powf(point_dr[1], 2.0)); //Distance between this point and the next

		//Slope of the center line
		float slope = point_dr[1] / point_dr[0];

		//Create paths for each wheel
		auto point_sp_cv = cv::Point2f(point_sp[0], point_sp[1]);
		cv::Point2f left = MiscMath::MoveAlongLine(point_dr[1] < 0, wheel_distance, MiscMath::NegativeReciprocal(slope), point_sp_cv);
		cv::Point2f right = MiscMath::MoveAlongLine(point_dr[1] > 0, wheel_distance, MiscMath::NegativeReciprocal(slope), point_sp_cv);

		//Get the distance travelled by each wheel
		float left_distance = MiscMath::PointDistance(left, left_last);
		float right_distance = MiscMath::PointDistance(right, right_last);

		//Normalize distances
		float max = std::max(left_distance, right_distance);
		left_distance /= max;
		right_distance /= max;

		//Accumulate travelled distances
		left_accum += left_distance;
		right_accum += right_distance;

		float left_velocity = left_distance;// / dist;
		float right_velocity = right_distance;// / dist;

		//Find change in angle
		double change_in_slope = ((point_dr_sq[1]*point_dr[0]) - (point_dr_sq[0]*point_dr[1])) / powf(point_dr[0], 2.0);
		double change_in_angle = (1.0 / (1.0 + powf(point_dr[1] / point_dr[0], 2.0))) * change_in_slope;
		float reverse_left = change_in_angle > (pi / 1.0) ? -1.0 : 1.0;
		float reverse_right = -change_in_angle > (pi / 1.0) ? -1.0 : 1.0;

		//Add path elements
		path_left.push_back(TalonPoint(left_accum, reverse_left * left_velocity, left)); 
		path_right.push_back(TalonPoint(right_accum, reverse_right * right_velocity, right)); 

		//Copy over positions and slope for next iteration
		left_last = left;
		right_last = right;

		//Prevent lockups TODO: Make this more deterministic
		if (dist / step > 0.0001) {
			i += dist / step; //Increment over the line by step over distance
		} else {
			i += 0.001;
		}
	}
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
		//Path::PrimitivePoint* point = reinterpret_cast<Path::PrimitivePoint*>(buffer);
		path_left.push_back(TalonPoint(reinterpret_cast<Path::PrimitivePoint*>(buffer), cv::Point2f(0.0, 0.0)));
	}
	for (size_t i = 0; i < points; i++) {
		sock->read_to(&buffer, sizeof(Path::PrimitivePoint));
		path_right.push_back(TalonPoint(reinterpret_cast<Path::PrimitivePoint*>(buffer), cv::Point2f(0.0, 0.0)));
	}
}

//TODO: Add concat function for paths
