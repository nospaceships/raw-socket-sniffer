
all:
	cd src && cl -MT -Fe..\raw-socket-sniffer.exe -O2 -EHsc raw-socket-sniffer.c ws2_32.lib
