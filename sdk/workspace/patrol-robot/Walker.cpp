#include <Clock.h>
#include "Common.hpp"
#include "Walker.hpp"

/*
 * Robot starts on leftmost cell, which is red and has index '0'.
 * The rightmost cell is red, too, and has index 'k' for some k \in \Nats.
 * There must be at least one non-red cell, so k has to be >= 2;
 */


Walker::Walker (
		SmoothMotor & motor,
		ePortS color_port )
:
	_motor          ( motor          ),
	_color_sensor   ( color_port     )
{
	on_position_change = nullptr;
}

void Walker::init()
{
	_current_position  = 0;
	_current_direction = Direction::Right;
	_current_color     = PositionColor::Blue;
	_motor.set_speed ( +abs_speed, wheel_response_time );
}

Walker::PositionColor Walker::next_color(rgb_raw_t const & rgb) const
{
	PositionColor c = _current_color;

	if ( rgb.r > 120 && rgb.g > 240 && rgb.b > 280 )
	{
		c = PositionColor::White;
	}

	if ( 40 < rgb.r && rgb.r < 70 && rgb.g > 140 && rgb.b > 180 )
	{
		c = PositionColor::Blue;
	}

	if ( rgb.r <= 30 && rgb.g < 50 && rgb.b < 50 )
	{
		c = PositionColor::Black;
	}

	return c;
}

void Walker::update_position(PositionColor c)
{
	if ( c == _current_color )
		return;

	step();

	if ( c == PositionColor::Blue )
		change_direction();

	_current_color = c;
	if ( on_position_change != nullptr )
		on_position_change ( PositionMessage{_current_direction, _current_position} );
}

void Walker::step()
{
	if ( _current_direction == Direction::Right )
		_current_position++;

	if ( _current_direction == Direction::Left )
		_current_position--;
}

void Walker::change_direction()
{
	switch ( _current_direction )
	{
		case Direction::Right:
			_current_direction = Direction::Left;
			_motor.set_speed ( -abs_speed, wheel_response_time );
			break;

		case Direction::Left:
			_current_direction = Direction::Right;
			_motor.set_speed ( +abs_speed, wheel_response_time );
			if ( _current_position != 0 )
			{
				fprintf ( bt, "Warning: position is %d instead of 0. Forcing 0.\n", _current_position );
				_current_position = 0;
			}
			break;
	}
}

void Walker::update_led() {
	switch(_current_color) {
		case PositionColor::Black:
			ev3_led_set_color(LED_OFF);
			break;
		case PositionColor::White:
			ev3_led_set_color(LED_GREEN);
			break;
		case PositionColor::Blue:
			ev3_led_set_color(LED_RED);
			break;
	}
}

Walker::PositionColor Walker::candidate_color(PositionColor c) {
	PositionColor curr = _current_color;
	if ( c == _candidate_color ) {
		if ( _candidate_remaining == 0 )
			curr = c;
		else
			_candidate_remaining--;
	} else {
		_candidate_color = c;
		_candidate_remaining = 5;
	}

	return curr;
}

void Walker::task()
{
	rgb_raw_t rgb;
	_color_sensor.getRawColor(rgb);
	update_position(candidate_color(next_color(rgb)));
	update_led();
}

