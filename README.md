# isim
Integrated simulator platform to synchronize different simulation processes and environments.

It uses shared memory and mutex to synchronize execution of several processes at defined time steps.

In the examples folder there is a boost converter simulated with Psim and controlled by an external process written in C.

In order to try it:
  First run isim.exe wiht the .ini file as only parameter
  Then start dsp.exe and launch the Psim simulation in any order.
 
 The api to integrate a new process is quite simple, just an init and a sync function that can be called quite often.
 
 It must have the same isim_data.h file structure in order to find singnals in shared memory.
