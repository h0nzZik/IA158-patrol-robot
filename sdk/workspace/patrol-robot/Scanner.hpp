#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <Port.h>
#include <SonarSensor.h>
#include <array>
#include <vector>
#include <functional>

#include "Common.hpp"
#include "Target.hpp"

class Scanner {
public:
    explicit Scanner(ePortS sonar_port);
    void task();

    void received_position_message(PositionMessage msg);
    Event<ScannedTarget> on_target;

private:
    Distance make_sample(Position position);
    void scan_changes(Position position, Direction current_dir);

    Distance median_distance(std::vector<int16_t>& samples);

    bool has_moved(Position position, Distance sample) const;

    ScannedTarget scanned_target;
    bool detected_target = false;

    Direction _direction;

    static const size_t map_size = 256;
    static const size_t allowed_error = 10;      // in cm
    static const unsigned sample_precision = 20; // number of samples
	static const size_t min_target_size = 3;

    ev3api::SonarSensor _sonar;
    std::array<bool, map_size> _scanned_map;
    std::array<Distance, map_size> _depth_map;
};

#endif // SCANNER_HPP
