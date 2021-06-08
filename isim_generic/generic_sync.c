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
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "isim_data.h"
#include "generic_config.h"

static HANDLE h_map_file;//handle for the shared map file
static LPCTSTR pt_sh_buff;//pointer to the shared memory buffer
static HANDLE h_sh_mutex; //handle to the shared mutex
DWORD wait_result; 


static double next_stop = 0.0;//next stop to sync with other processes
static volatile T_ISIM_ALL_SIGNALS * isim_pt;//pointer to the shared memory 

static double W;
static double R;
double t;//local time

declare_variables();

/**
* Initialize shared objects
*/
void isim_init(void){
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
		   FILE_MAP_ALL_ACCESS,  // read/write permission
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
	wait_result = WaitForSingleObject(h_sh_mutex, INFINITE);
		//initialize pointer to shared memory
		isim_pt = (volatile T_ISIM_ALL_SIGNALS *)pt_sh_buff;
	ReleaseMutex(h_sh_mutex);
}

/**
* Synchronize with other processes and R/W shared memory
*/
void isim_sync (void){

    while(t>next_stop){

		wait_result = WaitForSingleObject(h_sh_mutex, INFINITE);

			write_shm();

			isim_pt->pflags[process_slot].w= 1.0;
			isim_pt->pflags[process_slot].r= 0.0;
			FlushViewOfFile(pt_sh_buff, MAX_BUFF_SIZE);

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
					FlushViewOfFile(pt_sh_buff, MAX_BUFF_SIZE);
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
					FlushViewOfFile(pt_sh_buff, MAX_BUFF_SIZE);

				}
			ReleaseMutex(h_sh_mutex);
		}

    }

}



