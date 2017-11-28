#ifndef PATH_CALCULATOR_H
#define PATH_CALCULATOR_H
#include <cmath>
#include <misc_math.h>
#include <opencv2/opencv.hpp>
#include <renderable.h>
#include <tinysplinecpp.h>
#include <vector>
#include <socket.h>

class Path : public Renderable {
	public:
		struct PrimitivePoint {
			float position_left;
			float velocity_left;

			float position_right;
			float velocity_right;
		};
		struct TalonPoint {
			cv::Point2f display_point_left;
			cv::Point2f display_point_right;
			PrimitivePoint primitive;
			TalonPoint(
					float position_left, 
					float velocity_left, 
					float position_right, 
					float velocity_right, 
					cv::Point2f display_point_left, 
					cv::Point2f display_point_right
					)
			{
				this->display_point_left = display_point_left;
				this->display_point_right = display_point_right;
				primitive.position_left = position_left;
				primitive.velocity_left = velocity_left;
				primitive.position_right = position_right;
				primitive.velocity_right = velocity_right;
			}
			/*
			TalonPoint(PrimitivePoint* primitive, cv::Point2f display_point) {
				this->display_point_left = display_point_left;
				this->display_point_right = display_point_right;
				this->primitive = *primitive;
			}
			*/
		};

		//A serializable non-opencv dependent version of TalonPoint
		const float pi = acos(-1);
		std::vector<TalonPoint> points;
		Path(tinyspline::BSpline* spline, float wheel_distance, float max_change_time);
		Path(Socket* sock);
		void render();
		cv::Rect2f get_size();
		void color_by(float input);
		void to_socket (Socket* sock);
		void from_socket (Socket* sock);
};
#endif
