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


#ifndef _DLL_H_
#define _DLL_H_


#include <stdio.h>


/** \brief DLL routine funtion.
      * \param t: Time, passed from PSIM by value
      * \param delt: Time step, passed from PSIM by value
      * \param in: Input array, passed from PSIM by reference
      * \return output array, sent back to PSIM (Note: the values of out[*] can be modified in PSIM)
      * 
      * The maximum length of the input and output array "in" and "out" is 30.
      *
      * PSIM calls the DLL routine at each simulation time step. However, when the inputs of the DLL block are connected to the output of one of these discrete elements 
	  * (zero-order hold, unit delay, integrator, differentiator, z-domain transfer function block, and digital filters), 
	  * the DLL block is considered as a discrete element. In this case, the DLL block is called only at the discrete times.
	  *
	  * The DLL block receives the values from PSIM as the input, performs the calculation, and sends the output back to PSIM. 
	  * The node assignments are: the input nodes are on the left, and the output nodes are on the right. The sequence is from the top to the bottom. 
	  *
      */
      
extern "C" {

	void __stdcall __declspec(dllexport) simuser (double t, double delt, double *in, double *out);
}

#endif


