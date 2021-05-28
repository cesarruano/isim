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


#ifndef _PSIM_SYNC_CFG_HPP_
#define _PSIM_SYNC_CFG_HPP_

//name of the shared memory
#define SHM_ID "Local\\ISIM_DCDC_SHM"

//name of the shared mutex
#define MUTEX_ID "Local\\ISIM_DCDC_MUTEX"

//slot for this process in shared memory flags
//if multiple instances are needed it should be overriden by cfg input
#define process_slot (1)

//maximum size of the shared memory
#define MAX_BUFF_SIZE (4096)

//variables to be read from shared memory
#define declare_variables() \
	static double period;\
	static double duty_cycle;\


//copy to shared memory from psim signals
#define in_to_shm() {\
	isim_pt->psim.v_in = in[0];\
	isim_pt->psim.v_out = in[1];\
	isim_pt->psim.i_in = in[2];\
	isim_pt->psim.i_out = in[3];\
}

//copy from shared memory to internal variables
#define shm_to_vars() {\
	period = isim_pt->dsp.period;\
	duty_cycle = isim_pt->dsp.duty_cycle;\
}

//copy from internal variables to psim signals
#define vars_to_out() {\
	out[0] = period;\
	out[1] = duty_cycle;\
}



#endif


