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
 
#include <windows.h>
#pragma comment(lib, "user32.lib")
#include <stdio.h>
#include <tchar.h>

#include "dll.h"
#include "isim_data.h"
#include "psim_sync_config.h"

using namespace std;

double t;  //PSIM time
double delt;  //PSIM time delta since last call
double *in;  //PSIM in arg vector, lenght must be edited in PSIM schematic
double *out;  //PSIM out arg vector, lenght must be edited in PSIM schematic
  
__stdcall void simuser (double t, double delt, double *in, double *out)
{
    static HANDLE h_map_file;//handle for the shared map file
    static LPCTSTR pt_sh_buff;//pointer to the shared memory buffer
	static HANDLE h_sh_mutex; //handle to the shared mutex
	DWORD wait_result; 
    
    static double next_stop = 0.0;//next stop to sync with other processes
    static bool first_time = false;
    
    static T_ISIM_ALL_SIGNALS * isim_pt;//pointer to the shared memory 
	
	static double cfg;
	static int i;
    
    double W;
    double R;
			
	declare_variables();
	
    if(first_time==false){
		first_time = true;

		//open file mapping
		h_map_file = OpenFileMapping(
				   FILE_MAP_ALL_ACCESS,   // read/write access
				   FALSE,                 // do not inherit the name
				   SHM_ID);               // name of mapping object

		if (h_map_file == NULL)
		{
		  _tprintf(TEXT("Could not open file mapping object (%d).\n"),
				 GetLastError());
		}
		 pt_sh_buff = (LPTSTR) MapViewOfFile(h_map_file, // handle to map object
			   FILE_MAP_ALL_ACCESS,//FILE_MAP_READ/*FILE_MAP_ALL_ACCESS*/,  // read/write permission
			   0,
			   0,
			   MAX_BUFF_SIZE);

		if (pt_sh_buff == NULL)
		{
		  CloseHandle(h_map_file);
		}

		//Get mutex
		h_sh_mutex = CreateMutex( 
			NULL,              // default security attributes
			FALSE,             // initially not owned
			MUTEX_ID); 
			
		next_stop = 0.0;

    }

	if(t <= delt){
		/*It is faster to run the DLL in the same process, but psim is not killing it
		so variables remain initialized after first execution, time is used to reinitialize
		everytime the simulation starts again.*/
		wait_result = WaitForSingleObject(h_sh_mutex, INFINITE);
			isim_pt = (T_ISIM_ALL_SIGNALS *)pt_sh_buff;
			next_stop = 0.0;
			isim_pt->sync.t = 0.0;
		ReleaseMutex(h_sh_mutex);
	}
	
    while(t>next_stop){

		wait_result = WaitForSingleObject(h_sh_mutex, INFINITE);
		
		in_to_shm();
		
        isim_pt->pflags[process_slot].w= 1.0;
		isim_pt->pflags[process_slot].r= 0.0;
		FlushViewOfFile(isim_pt, MAX_BUFF_SIZE);
		
		ReleaseMutex(h_sh_mutex);
		
        //wait for others to write
        W = 0.0;
        while(W==0.0){
			wait_result = WaitForSingleObject(h_sh_mutex, INFINITE);
				W = isim_pt->sync.w;
				if(W!=0.0){//if all have written then read
					//clear local write flag 
					isim_pt->pflags[process_slot].w= 0.0;

					shm_to_vars();
					
					isim_pt->pflags[process_slot].r= 1.0;
					FlushViewOfFile(isim_pt, MAX_BUFF_SIZE);
				}
			ReleaseMutex(h_sh_mutex);
        }
		
		//wait for others to read
        R = 0.0;
		while(R==0.0){
			wait_result = WaitForSingleObject(h_sh_mutex, INFINITE);
				R = isim_pt->sync.r;
				if(R!=0.0){//if all have read then load next stop
					//clear local read flag 
					isim_pt->pflags[process_slot].r= 0.0;
					//load next stop
					next_stop = isim_pt->sync.t;
					FlushViewOfFile(isim_pt, MAX_BUFF_SIZE);

				}
			ReleaseMutex(h_sh_mutex);
		}
        
    }

	vars_to_out();
}



