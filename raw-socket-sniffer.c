#ifndef RAW_SOCKET_SNIFFER_C
#define RAW_SOCKET_SNIFFER_C

// Copyright 2018 NoSpaceships Ltd

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <mstcpip.h>
#include <iphlpapi.h>

// We use defines and structures copied from libpcap to synthesize a PCAP file.
#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4

#define DLT_EN10MB 1

struct pcap_timeval {
	int32_t tv_sec;
	int32_t tv_usec;
};

struct pcap_file_header {
	uint32_t magic;
	uint16_t version_major;
	uint16_t version_minor;
	int32_t thiszone;
	uint32_t sigfigs;
	uint32_t snaplen;
	uint32_t linktype;
};

struct pcap_sf_pkthdr {
	struct pcap_timeval ts;
	uint32_t caplen;
	uint32_t len;
};

// Size of the buffer we use to read packets from the network.
#define BUFFER_SIZE ((256*256) - 1)

// A couple of defines used to calculate high resolution timestamps.
#define EPOCH_BIAS 116444736000000000
#define UNITS_PER_SEC  10000000

// Normally we would break this up into smaller functions, but here we lay out
// all the steps to capture packets using raw sockets one step after another.
int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <interface-ip> <capture-file>\n", argv[0]);
		exit(-1);
	}

	// Windows winsock requires this.
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Open our capture file, overwrite if it already exists.
	FILE* fp = fopen(argv[2], "wb");
	if (! fp) {
		fprintf(stderr, "fopen(%s) failed: %d\n", argv[2], errno);
		exit(-1);
	}

	// Disable file buffering to prevent file corruption on termination.
	setbuf(fp, NULL);

	// Create a PCAP file header.
	struct pcap_file_header hdr;
	hdr.magic = 0xa1b2c3d4;
	hdr.version_major = PCAP_VERSION_MAJOR;
	hdr.version_minor = PCAP_VERSION_MINOR;
	hdr.thiszone = 0;
	hdr.snaplen = BUFFER_SIZE;
	hdr.sigfigs = 0;
	hdr.linktype = DLT_EN10MB;

	// Write the PCAP file header to our capture file.
	if (fwrite((char*)&hdr, sizeof(hdr), 1, fp) != 1) {
		fprintf(stderr, "fwrite(pcap_file_header) failed: %u", errno);
		exit(-1);
	}

	// Create a raw socket which supports IPv4 only.
	SOCKET sd = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (sd == INVALID_SOCKET) {
		fprintf(stderr, "socket() failed: %u", WSAGetLastError());
		exit(-1);
	}

	// Captured IP packets sent and received by the network interface the
	// specified IP address is associated with.
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(0);

	// Bind the socket to the specified IP address.
	int rc = bind(sd, (struct sockaddr*) &addr, sizeof(addr));
	if (rc == SOCKET_ERROR) {
		fprintf(stderr, "bind() failed: %u", WSAGetLastError());
		exit(-1);
	}

	// Give us ALL IPv4 packets sent and received to the specific IP address.
	int value = RCVALL_IPLEVEL;
	DWORD out = 0;
	rc = WSAIoctl(sd, SIO_RCVALL, &value, sizeof(value), NULL, 0, &out, NULL, NULL);
	if (rc == SOCKET_ERROR) {
		fprintf(stderr, "WSAIoctl() failed: %u", WSAGetLastError());
		exit(-1);
	}

	// First 14 bytes are a fake ethernet header with IPv4 as the protocol.
	unsigned char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	buffer[12] = 0x08;
	struct pcap_sf_pkthdr pkt;

	// Reuse this on each loop to calculate a high resolution timestamp.
	union {
		FILETIME ft_ft;
		uint64_t ft_64;
	} ft;

	// Read packets forever.
	while (1) {
		// Read the next packet, blocking forever.
		int rc = recv(sd, (char*) buffer + 14, BUFFER_SIZE - 14, 0);
		if (rc == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed: %u", WSAGetLastError());
			exit(-1);
		}

		// End of file for some strange reason, so stop reading packets.
		if (rc == 0)
			break;
		
		// Calculate a high resolution timestamp for this packet.
		GetSystemTimeAsFileTime(&ft.ft_ft);
		ft.ft_64 -= EPOCH_BIAS;
		time_t ctime = (time_t) (ft.ft_64 / UNITS_PER_SEC);
		uint32_t ms = ft.ft_64 % UNITS_PER_SEC;

		// Set out PCAP packet header fields.
		pkt.ts.tv_sec = ctime;
		pkt.ts.tv_usec = ms;
		pkt.caplen = rc + 14;
		pkt.len = rc + 14;

		// Write our PCAP packet header
		if (fwrite((char*) &pkt, sizeof(pkt), 1, fp) != 1) {
			fprintf(stderr, "fwrite(pcap_sf_pkthdr) failed: %d", errno);
			exit(-1);
		}

		// Write our packet data immediately after the PCAP packet header.
		if (fwrite(buffer, rc + 14, 1, fp) != 1) {
			fprintf(stderr, "fwrite(buffer) failed: %d", errno);
			exit(-1);
		}
	}

	// Our socket and file will be closed automatically.
	return 0;
}

#endif /* RAW_SOCKET_SNIFFER_C */