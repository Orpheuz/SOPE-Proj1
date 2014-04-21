all: monitor AuxMonitor FileMonitor
	mkdir -p bin
	gcc monitor.c -o  bin/monitor -Wall
	gcc AuxMonitor.c -o bin/AuxMonitor -Wall
	gcc FileMonitor.c -o bin/FileMonitor -Wall
