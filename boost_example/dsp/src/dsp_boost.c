/* 
 * This file is part of the ISIM distribution (https://github.com/cesarruano/isim).
 * Copyright (c) 2021 Cesar Ruano.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "generic_api.h"

double t;

double v_in;
double v_out;
double i_in;
double i_out;

double period = 10e-6;
double duty_cycle;

static const double Kp = 0.01;
static const double Ki = 0.0001;
static double i_acc;
static double setpoint = 20.0;



double PI(double error, double min, double max){
	double new_iacc = i_acc + error*Ki;
	double out = error*Kp + new_iacc;
	if(out < min){
		out = min;
	} else if(out > max){
		out = max;
	} else {
		i_acc = new_iacc;
	}
	return out;
}

void main(void){
	isim_init();
	
	while(1){
		t = t+period;//timestep here is switching time, but it could be any arbitrary value
		
		//PI every switching period
		duty_cycle = PI((setpoint-v_out), 0.0, 0.9);
		
		isim_sync();
	}
	
}