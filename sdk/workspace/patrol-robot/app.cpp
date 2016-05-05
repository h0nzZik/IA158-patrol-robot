/**
 * Hello EV3
 *
 * This is a program used to test the whole platform.
 */

#include "ev3api.h"
#include "app.h"
#include <unistd.h>
#include <ctype.h>
#include <Port.h>
#include <Motor.h>
#include <Clock.h>

#include "Common.hpp"
#include "Position.hpp"
#include "Walker.hpp"
#include "Scanner.hpp"
#include "Tower.hpp"

extern "C" {
void* __dso_handle = NULL;
}

FILE *bt;

struct PatrolRobot {
    PositionEvent
            position_event; // must be constructed before scanner and tower!
    TowerCommandEvent tower_command;

    Walker walker;
    Scanner scanner;
    Tower tower;

    PatrolRobot()
        : walker(ePortM::PORT_A, ePortS::PORT_1, position_event)
        , scanner(ePortS::PORT_2, position_event)
        , tower(ePortM::PORT_B, ePortM::PORT_C, position_event, tower_command)
    {
        // Temp
        TowerMessage ev;
        ev.command = TowerMessage::Command::LOCK;
        ev.params.target = {5, 10};
        tower_command.invoke(ev);
    }
};

PatrolRobot* robot;

void main_task(intptr_t unused) {
    bt = fdopen(/*SIO_BT_FILENO*/5, "a+");
    assert(bt != NULL);

    robot = new PatrolRobot;

    act_tsk(TOWER_TASK);
    act_tsk(WALKER_TASK);
}

void walker_task(intptr_t exinf) {
    ev3_speaker_set_volume(100);
    robot->walker.init();
    ev3_speaker_play_tone(1000, 100);
    ev3api::Clock c;
    while (true)
    {
        robot->walker.task();
        tslp_tsk(2);
    }
        
}

void scanner_task(intptr_t exinf) {
    /*while (true)
            scanner.task();*/
}

void tower_task(intptr_t exinf) {
    while (true) {
        robot->tower.task();
        tslp_tsk(50);
    }
}
