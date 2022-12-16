#!/bin/bash
sudo ./rangefinder_gp2y_Mishchenko -q 1000 gp2y_data
sudo ./keypad_v10 -q > keypad_data &
sudo ./step_motor -q 1

