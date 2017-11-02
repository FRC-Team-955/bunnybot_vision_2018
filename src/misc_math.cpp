#include <misc_math.h>

bool MiscMath::ToleranceCheck(float input, float expect, float tolerance)
{
    return fabs(input - expect) <= tolerance;
}

float MiscMath::PointDistance(cv::Point* a, cv::Point* b)
{
    return cv::norm(*b - *a);
}

float MiscMath::PointDistance(cv::Point a, cv::Point b)
{
    return cv::norm(b - a);
}

float MiscMath::PointDistance(cv::Point2f a, cv::Point2f b)
{
    return cv::norm(b - a);
}

float MiscMath::NegativeReciprocal(float slope)
{
    return -1.0f / slope;
}

cv::Point2f MiscMath::MoveAlongLine(bool forward, float distance, float slope, cv::Point2f start)
{
    float k = (forward ? distance : -distance) / sqrtf(1 + pow(slope, 2));
    return start + (k * cv::Point2f(1, slope));
}

cv::Point MiscMath::MidPoint(cv::Point* a, cv::Point* b)
{
    return (*a + *b) / 2;
}

cv::Point MiscMath::MidPoint(cv::Point a, cv::Point b)
{
    return (a + b) / 2;
}

cv::Point MiscMath::GetCenter(cv::Rect* rectangle)
{
    return (rectangle->br() + rectangle->tl()) / 2;
}

cv::Point2f MiscMath::FromCtrlpt(tinyspline::BSpline* spline, size_t index)
{
    return cv::Point2f(spline->ctrlp()[index * spline->dim()], spline->ctrlp()[(index * spline->dim()) + 1]);
}

cv::Point2f MiscMath::RadialOffset(float radians, float distance, cv::Point2f offset)
{
	return cv::Point2f(
			(std::cos(radians) * distance) + offset.x,
			(std::sin(radians) * distance) + offset.y);
}
