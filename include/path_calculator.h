#ifndef PATH_CALCULATOR_H
#define PATH_CALCULATOR_H
#include <vector>
/*
 * TODO: Make an interface class for path calculators, this one only works
 * with tank drive controls
 */
class PathCalculator {
public:
    struct Goal { // Contains a position and a vector for an endpoint
        float x_position;
        float y_position;
        float direction; // In slope TODO: Add functions to create a goal from
            // radians/degrees
    };
    struct Result {
		 std::vector<float> left_tracks;
		 std::vector<float> right_tracks;
    };
    PathCalculator(float interval);
    void calculate_path(Goal goal);

private:
    float interval;
};
#endif
