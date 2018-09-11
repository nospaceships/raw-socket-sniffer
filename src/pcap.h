#ifndef STRUCT_PCAP_H
#define STRUCT_PCAP_H

// The following structures were taken from the libpcap source.

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

#endif /* STRUCT_PCAP_H */