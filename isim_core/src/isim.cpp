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
#include <tchar.h>
#include <iostream>
#include <iomanip>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <string>
#include <vector>

#include "isim_basic.hpp"

using namespace std;

int num_procs;//number of active processes
long long mem_size;//memory size parsed from ini file
basic_string<char> base_name;//base string to name shared resources
double sim_time;//centralized simulation time
double sync_step;//time increments between synchronizations
vector<int> positions;//active processes in current execution

HANDLE h_map_file;//handle for the shared map file
HANDLE h_mutex; //handle to the shared mutex
LPCTSTR pt_sh_buff;//pointer to the shared memory buffer
DWORD wait_result; 

volatile double * bp;//pointer to shared memory as double array

bool all_write;//true if all active processes have already written their signals
bool all_read;//true if all active processes have already read their signals
double next_sync;//absolut time for next synchronization

/**
* Reads configuration parameters from .ini file
*/
int parse_ini(string ini_file){
	try{
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(ini_file, pt);
		
		string::size_type sz;
		num_procs = stoi ((pt.get<string>("Global.NumProcs")),&sz);
		mem_size = stoi ((pt.get<string>("Global.MemSize")),&sz);
		sim_time = stod ((pt.get<string>("Global.SimTime")),&sz);
		sync_step = stod ((pt.get<string>("Global.SyncStep")),&sz);
		base_name = pt.get<string>("Global.ResName");
		
		cout << "Simulation name: "+base_name << endl;
		cout << "Run until: "+to_string(sim_time)+"(s)" << endl;
		cout << "Synchronization step is: "+to_string(sync_step)+"(s)" << endl;
		cout << "Shared memory allocated: "+to_string(mem_size)+"(bytes)"<< endl;
		cout << "Mapping "+to_string(num_procs)+" processes." << endl;
		
		for(int i=0;i<num_procs;i++){
			int pos = stoi ((pt.get<string>("Proc"+to_string(i)+".Pos")),&sz);
			positions.push_back(pos);
			cout << "Mapping process "+to_string(i)+" to address "+to_string(pos) << endl;
			
		}
	}
	catch(...){
		return 1;
	}
	return 0;
}

/**
* Creates shared resources under the common name from .ini file
*/
int create_shared_resources(void){
    h_map_file = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 mem_size,                // maximum object size (low-order DWORD)
                 ("Local\\"+base_name+"_SHM").c_str());                 // name of mapping object

   if (h_map_file == NULL)
   {
      _tprintf(TEXT("Could not create file mapping object (%d).\n"),
             GetLastError());
      return GetLastError();
   }
   pt_sh_buff = (LPTSTR) MapViewOfFile(h_map_file,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        mem_size);

   if (pt_sh_buff == NULL)
   {
      _tprintf(TEXT("Could not map view of file (%d).\n"),
             GetLastError());

       CloseHandle(h_map_file);

      return GetLastError();
   }
   
   h_mutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        ("Local\\"+base_name+"_MUTEX").c_str()); 
   
   return 0;
}


//macros for flag access
#define SetGT(x) {bp[0]=x;}
#define SetGW(x) {bp[1]=x;}
#define SetGR(x) {bp[2]=x;}

#define GetW(p) (bp[positions[p]*2+3])
#define GetR(p) (bp[positions[p]*2+4])

/**
* Exit routine if initialization fails
*/
void exit_routine(void){
		
	wait_result = WaitForSingleObject(h_mutex, INFINITE);
	ReleaseMutex(h_mutex);
    UnmapViewOfFile(pt_sh_buff);
    CloseHandle(h_map_file);
	exit(0);
	
}

int main(int argc, char **argv, char **envp)
{
	cout << fixed << setprecision(9);

	if(parse_ini(argv[1]) != 0){cout << "Could not parse ini file."<<endl;return 1;}
	if(create_shared_resources() != 0){cout << "Could not create shared memory."<<endl;return 2;}
	
	wait_result = WaitForSingleObject(h_mutex, INFINITE);
		bp = (volatile double *)pt_sh_buff;
		memset((void *)pt_sh_buff, 0, mem_size);
		cout << "\r" << "t = " << next_sync;
	ReleaseMutex(h_mutex);
	
	int i;
	next_sync = 0.0;
    while(true){

		all_write = false;
		while(!all_write){//sync writes
			//Mutex to wait for writes
			wait_result = WaitForSingleObject(h_mutex, INFINITE);
				all_write = true;
				for(i=0;i<num_procs;i++){
					all_write = all_write && ((GetW(i))==1.0);
				}
				if(all_write == true){
					SetGR(0.0);
					SetGW(1.0);
					//will break
				}
				FlushViewOfFile(pt_sh_buff, mem_size);
			ReleaseMutex(h_mutex);
		}
		
		all_read = false;
		while(!all_read){//sync reads and time
			//Mutex to wait for reads
			wait_result = WaitForSingleObject(h_mutex, INFINITE);
				all_read = true;
				for(i=0;i<num_procs;i++){
					all_read = all_read && ((GetR(i))==1.0);
				}
				if(all_read == true){
					SetGR(1.0);
					SetGW(0.0);
					next_sync = next_sync + sync_step;
					if(next_sync > sim_time){exit_routine();}
					SetGT(next_sync);
					cout << "\r" << "t = " << next_sync;
					FlushViewOfFile(pt_sh_buff, mem_size);
					//will break
				}
			ReleaseMutex(h_mutex);
		}

    }
	
    return 0;
}