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


#ifndef _GENERIC_CFG_H_
#define _GENERIC_CFG_H_

#define SHM_ID "Local\\ISIM_GENERIC_SHM"

#define MUTEX_ID "Local\\ISIM_GENERIC_MUTEX"

#define process_slot (0)

#define declare_variables() \
	extern double t;\
	extern double v_in;\
	extern double v_out;\
	extern double i_in;\
	extern double i_out;\
	extern double period;\
	extern double duty_cycle;\

#define write_shm() {\
	isim_pt->dsp.period = period;\
	isim_pt->dsp.duty_cycle = duty_cycle;\
}

#define shm_to_vars() {\
	v_in = isim_pt->psim.v_in;\
	v_out= isim_pt->psim.v_out;\
	i_in = isim_pt->psim.i_in;\
	i_out = isim_pt->psim.i_out;\
}

#define MAX_BUFF_SIZE (4096)

#endif


