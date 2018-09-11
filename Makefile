
all:
	cd src && cl -Fe..\raw-socket-sniffer.exe -O2 -EHsc raw-socket-sniffer.c ws2_32.lib
