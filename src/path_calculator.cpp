#include <path_calculator.h>

PathCalculator::PathCalculator(float interval, float outcrop, float wheel_distance, float step)
{
	this->step = step;
	this->wheel_distance = wheel_distance;
	this->interval = interval;
	this->outcrop = outcrop;
	this->spline = new tinyspline::BSpline(6, 3, 3); //Three dimensions, so we can display it later using native OpenGL functions
	this->ctrlp = this->spline->ctrlp();
	//this->spline_renderer = SplineRenderWrap(this->spline);
	this->spline_renderer = new SplineRenderWrap(this->spline);
}

Path PathCalculator::calculate_path(Goal goal)
{
	this->ctrlp[0] = 0.0;
	this->ctrlp[1] = 0.0;
	this->ctrlp[2] = 0.0;

	this->ctrlp[3] = 0.0;
	this->ctrlp[4] = this->outcrop;
	this->ctrlp[5] = 0.0;

	this->ctrlp[6] = 0.0;
	this->ctrlp[7] = 2.0 * this->outcrop;
	this->ctrlp[8] = 0.0;

	auto leading_a = MiscMath::RadialOffset(goal.direction, this->outcrop * 2.0, goal.position);
	this->ctrlp[9] = leading_a.x;
	this->ctrlp[10] = leading_a.y;
	this->ctrlp[11] = 0.0;

	auto leading_b = MiscMath::RadialOffset(goal.direction, this->outcrop, goal.position);
	this->ctrlp[12] = leading_b.x;
	this->ctrlp[13] = leading_b.y;
	this->ctrlp[14] = 0.0;

	this->ctrlp[15] = goal.position.x;
	this->ctrlp[16] = goal.position.y;
	this->ctrlp[17] = 0.0;
	spline->setCtrlp(this->ctrlp);
	return Path(this->spline, this->wheel_distance, this->step);
}

void PathCalculator::render()
{
	this->spline_renderer->render();
}

Path::Path(tinyspline::BSpline* spline, float wheel_distance, float step)
{
	this->step = step;
	auto derive = spline->derive();
	float i = 0.0;
	cv::Point2f left_last(0.0, 0.0);
	cv::Point2f right_last(0.0, 0.0);
	float left_accum = 0.0;
	float right_accum = 0.0;
	while (i < 1.0) {
		auto point_sp = spline->evaluate(i).result();
		auto point_sp_cv = cv::Point2f(point_sp[0], point_sp[1]);
		auto point_dr = derive.evaluate(i).result();

		float dist = sqrtf(powf(point_dr[0], 2) + powf(point_dr[1], 2));
		float slope = point_dr[1] / point_dr[0];

		cv::Point2f left = MiscMath::MoveAlongLine(point_dr[1] < 0, wheel_distance, MiscMath::NegativeReciprocal(slope), point_sp_cv);
		cv::Point2f right = MiscMath::MoveAlongLine(point_dr[1] > 0, wheel_distance, MiscMath::NegativeReciprocal(slope), point_sp_cv);

		float left_distance = MiscMath::PointDistance(left, left_last);
		float right_distance = MiscMath::PointDistance(right, right_last);

		left_accum += left_distance;
		right_accum += right_distance;

		path_left.push_back(TalonPoint(left_accum, left_distance, left));
		path_right.push_back(TalonPoint(right_accum, right_distance, right));

		left_last = left;
		right_last = right;

		i += step / dist;
	}
}

cv::Rect2f PathCalculator::get_size()
{
	return this->spline_renderer->get_size();
}

void Path::render()
{
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(9);
	glBegin(GL_LINES);

	glVertex2f(path_left.front().display_point.x, path_left.front().display_point.y);
	for (auto& left : this->path_left) {
		glColor3f(1.0, fabs(step / left.velocity), fabs(step / left.velocity));
		glVertex2f(left.display_point.x, left.display_point.y);
		glVertex2f(left.display_point.x, left.display_point.y);
	}
	glVertex2f(path_left.back().display_point.x, path_left.back().display_point.y);

	glVertex2f(path_right.front().display_point.x, path_right.front().display_point.y);
	glColor3f(0.0, 1.0, 1.0);
	for (auto& right : this->path_right) {
		glColor3f(fabs(step / right.velocity), 1.0, fabs(step / right.velocity));
		glVertex2f(right.display_point.x, right.display_point.y);
		glVertex2f(right.display_point.x, right.display_point.y);
	}
	glVertex2f(path_right.back().display_point.x, path_right.back().display_point.y);

	glEnd();
}

cv::Rect2f Path::get_size()
{ //Ignore me I'm small!
	return cv::Rect2f(0.0, 0.0, 0.0, 0.0);
}
