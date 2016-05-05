#ifndef TOWER_HPP
#define TOWER_HPP

#include <Port.h>
#include <Motor.h>
#include "Common.hpp"
#include "Position.hpp"

class Tower
{
	public:
		Tower(
            ePortM rotation_port,
            ePortM fire_port,
            PositionEvent& position_event,
            TowerCommandEvent& command_event);
		void task();

	private:
		void received_position_message(PositionMessage msg);
        void received_command_message(TowerMessage msg);

        void update_position(double angle);

        ev3api::Motor _rotation_motor;
        ev3api::Motor _fire_motor;

        int32_t _direction;
        bool _follow_target;
        Target _target;

        const int32_t speed = 200;
        const double gear_ratio = 1.95;
};

#endif // TOWER_HPP
