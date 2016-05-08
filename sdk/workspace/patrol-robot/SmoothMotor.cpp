#include "SmoothMotor.hpp"

		
SmoothMotor::SmoothMotor ( ePortM motor_port, ID mutex_id ) :
	_motor ( motor_port ),
	_mutex_id ( mutex_id )
{
	_current_time = 1;
	_time_to_change = 1;
	_current_speed = 0;
	_desired_speed = 0;
	_last_stable_speed = 0;
}

bool SmoothMotor::valid ( Speed_t speed )
{
	return (speed <= 100) && (speed >= -100);
}

void SmoothMotor::set_speed ( Speed_t speed, Milliseconds_t time_to_change )
{
	assert ( time_to_change >= 1 );
	assert ( valid ( speed ) );

	loc_mtx ( _mutex_id );

	_last_stable_speed = _current_speed;
	_desired_speed = speed;
	_current_time = 0;
	_time_to_change = time_to_change;

	unl_mtx ( _mutex_id );
}

void SmoothMotor::update_speed()
{
	if (_current_time >= _time_to_change)
		return;

	_current_time++;
	Speed_t current_speed;
	if ( _current_time < _time_to_change )
	{
		SpeedDiff_t speed_difference = int16_t(_desired_speed) - int16_t(_last_stable_speed);
		int16_t acceleration = ((int32_t(speed_difference) * int32_t(_current_time))) / int32_t(_time_to_change);
		current_speed = _last_stable_speed + acceleration;
	} else {
		current_speed = _desired_speed;
	}

	if ( current_speed > 0 )
		assert ( current_speed < 20 );

	if ( current_speed < 0 )
		assert ( current_speed > -20 );

	if ( current_speed != _current_speed )
	{
		_motor.setPWM(current_speed);
		_current_speed = current_speed;
	}
}

void SmoothMotor::every_1ms()
{
	loc_mtx( _mutex_id );
	update_speed ();
	unl_mtx ( _mutex_id );
}
