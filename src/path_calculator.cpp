#include <path_calculator.h>

PathCalculator::PathCalculator(size_t ctrlp_count) : SplineRenderWrap()
{
	this->spline = new tinyspline::BSpline(ctrlp_count, 3, 3); //Three dimensions, so we can display it later using native OpenGL functions
	this->ctrlp = this->spline->ctrlp();
}

Path::Path(tinyspline::BSpline* spline, float wheel_distance, float step)
{
	this->step = step;
	auto derive = spline->derive();
	auto derive_sq = derive.derive();
	cv::Point2f left_last(0.0, 0.0);
	cv::Point2f right_last(0.0, 0.0);
	float left_accum = 0.0;
	float right_accum = 0.0;

	//DIRTY HACK for inflection points
	bool inflect_left = true;
	bool inflect_right = true;

	float slope_last_left = 1.0;
	float slope_last_right = 1.0;

	float i = 0.0;
	while (i < 1.0) {
		//Calculate spline evaluations
		auto point_sp = spline->evaluate(i).result();
		auto point_dr = derive.evaluate(i).result();

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

		//Get the slope between each point
		float slope_left = MiscMath::LineSlope(left, left_last);
		float slope_right = MiscMath::LineSlope(right, right_last);

		//Normalize distances
		float max = std::max(left_distance, right_distance);
		left_distance /= max;
		right_distance /= max;

		//Accumulate travelled distances
		left_accum += left_distance;
		right_accum += right_distance;

		//Add path elements
		//float pi = acosf(-1);
		//bool rev_left = (left_distance - right_distance) / (dist * 2.0) > 1.0;
		//bool rev_right = (right_distance - right_distance) / (dist * 2.0) > 1.0;
		//path_left.push_back(TalonPoint(left_accum, rev_left ? -1 : 1, left)); 
		//path_right.push_back(TalonPoint(right_accum, rev_right ? -1 : 1, right)); 
		path_left.push_back(TalonPoint(left_accum, left_distance, left)); 
		path_right.push_back(TalonPoint(right_accum, right_distance, right)); 

		//Copy over positions and slope for next iteration
		left_last = left;
		right_last = right;
		slope_last_left = slope_left;
		slope_last_right = slope_right;

		i += step / dist; //Increment over the line by step over distance
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
	glBegin(GL_LINES);

	glVertex2f(path_left.front().display_point.x, path_left.front().display_point.y);
	for (auto& left : this->path_left) {
		color_by(left.velocity);
		glVertex2f(left.display_point.x, left.display_point.y);
		glVertex2f(left.display_point.x, left.display_point.y);
	}
	glVertex2f(path_left.back().display_point.x, path_left.back().display_point.y);

	glVertex2f(path_right.front().display_point.x, path_right.front().display_point.y);
	glColor3f(0.0, 1.0, 1.0);
	for (auto& right : this->path_right) {
		color_by(right.velocity);
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

