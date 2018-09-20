
##
## Usage: raw-socket-sniffer.ps1 -InterfaceIp 127.0.0.1 -CaptureFile capture.cap
##
param(
    [Parameter(Mandatory=$true)][String]$InterfaceIp,
    [Parameter(Mandatory=$true)][String]$CaptureFile
)

##
## Create a PCAP file header.  This is the same as doing the following in
## libpcap:
##
##    // Create a PCAP file header.
##    struct pcap_file_header hdr;
##    hdr.magic = 0xa1b2c3d4;
##    hdr.version_major = PCAP_VERSION_MAJOR;
##    hdr.version_minor = PCAP_VERSION_MINOR;
##    hdr.thiszone = 0;
##    hdr.snaplen = BUFFER_SIZE;
##    hdr.sigfigs = 0;
##    hdr.linktype = DLT_EN10MB;
##
[Byte[]]$pcap_header = `
        0xd4, 0xc3, 0xb2, 0xa1, 0x02, 0x00, 0x04, 0x00, `
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, `
        0xff, 0xff, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00

##
## Open the capture file, deleting if already exists, and write a PCAP file
## header.
##
if ([IO.File]::Exists($CaptureFile)) {
    [IO.File]::Delete($CaptureFile)
}
$file = [IO.File]::OpenWrite($CaptureFile)
$file.Write($pcap_header, 0, $pcap_header.Length)

##
## Create a raw socket to capture IP packets.
##
$socket = New-object System.Net.Sockets.Socket(
        [Net.Sockets.AddressFamily]::InterNetwork,
        [Net.Sockets.SocketType]::Raw, [Net.Sockets.ProtocolType]::Unspecified)

##
## Bind our socket to the specified IP address, then use an IO control to
## specify that all IP packets sent and received on the network interface the
## IP address is associated should be captured and passed to our socket.
##
$socket.Bind((New-Object System.Net.IPEndPoint([Net.IPAddress]$InterfaceIp, 0)))
$null = $socket.IOControl([Net.Sockets.IOControlCode]::ReceiveAll,
        [BitConverter]::GetBytes(1), $null)

##
## Calculate an epoch date/time which is then used to calculate a per-packet
## date/time.
##
$epoch = Get-Date -Date "01/01/1970"

##
## Create a receive buffer.  The first 14 bytes are a fake ethernet header
## and we set the next protocol to IPv4.
##
$buffer = New-Object Byte[] $socket.ReceiveBufferSize
$buffer[12] = 0x08

while($true) {
    ##
    ## IP packets read here will NOT include an ethernet header.  Our buffer
    ## has a fake ethernet header so we read into the buffer after that.
    ##
    $rc = $socket.Receive($buffer, 14, $buffer.Length - 14, 0) + 14

    ##
    ## Calculate date/time the packet was captured.  This is not accurate!
    ##
    $now = Get-Date
    $timespan = New-TimeSpan -Start $epoch -End $now
    $ctime = [Int][Math]::Truncate($timespan.TotalSeconds)

    ##
    ## Write a PCAP packet header which looks like this in libpcap:
    ##
    ## struct pcap_sf_pkthdr {
    ##     struct pcap_timeval {
    ##         int32_t tv_sec;
    ##         int32_t tv_usec;
    ##     };
    ##     uint32_t caplen;
    ##     uint32_t len;
    ## };
    ##
    $file.Write([BitConverter]::GetBytes($ctime), 0, 4)
    $file.Write([BitConverter]::GetBytes($timespan.Milliseconds), 0, 4)
    $file.Write([BitConverter]::GetBytes($rc), 0, 4)
    $file.Write([BitConverter]::GetBytes($rc), 0, 4)

    ##
    ## Now write the packet we captured.
    ##
    $file.Write($buffer, 0, $rc)
}
