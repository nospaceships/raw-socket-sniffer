
all:
	cl -MT -Feraw-socket-sniffer.exe -O2 -EHsc raw-socket-sniffer.c ws2_32.lib
