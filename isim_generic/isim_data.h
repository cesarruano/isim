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
 
#ifndef _ISIM_DATA_H_
#define _ISIM_DATA_H_


#define MAX_ALLOWED_PROCESSES (10)

typedef struct{
	double t;
	double w;
	double r;
}T_ISIM_Sync;

typedef struct{
	double w;
	double r;
} T_ProcessFlags;

typedef struct{
    volatile double period;
    volatile double duty_cycle;
} T_DSP_SIGNALS;

typedef struct{
    volatile double v_in;
    volatile double v_out;
    volatile double i_in;
    volatile double i_out;
} T_PSIM_SIGNALS;

typedef struct{
	T_ISIM_Sync sync;
	T_ProcessFlags pflags[MAX_ALLOWED_PROCESSES];
    T_DSP_SIGNALS dsp;
    T_PSIM_SIGNALS psim;
} T_ISIM_ALL_SIGNALS;

#endif


