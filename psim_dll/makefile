build: psim_sync.dll

psim_sync.dll: psim_sync.o
	g++ -shared psim_sync.o -o psim_sync.dll
	del psim_sync.o

psim_sync.o: psim_sync.cpp
	g++ -c psim_sync.cpp -o psim_sync.o

clean:
	del psim_sync.dll
	del psim_sync.o