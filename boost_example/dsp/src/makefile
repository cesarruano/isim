build: dsp_boost.exe

dsp_boost.exe: dsp_boost.o generic_sync.o
	gcc -o dsp_boost.exe dsp_boost.o generic_sync.o

dsp_boost.o: dsp_boost.c
	gcc -c dsp_boost.c -o dsp_boost.o

generic_sync.o: generic_sync.c
	gcc -c generic_sync.c -o generic_sync.o

clean:
	del dsp_boost.o
	del generic_sync.o
	del dsp_boost.exe