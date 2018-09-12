
# raw-socket-sniffer - Packet capture on Windows without drivers

This repository contains the source for a program which demonstrates how to
capture IP packets on Windows using just raw sockets.

It requires no additional software, such as WinPCAP or npcap, and will simply
use existing operating system functionality.

The program saves captured packets to a file in PCAP format so that it can be
opened on a separate host with a tool which can read that file format
installed.

# Build

Once the project has been checked out simply run the following command to
compile it:

    cd <repository>
    nmake

The file `raw-socket-sniffer.exe` will be placed in to the root of the
repository.

# Capture

Once built, transfer the `raw-socket-sniffer.exe` program to the host on which
packet capture should be performed.  Then run the following command to capture
packets:

    raw-socket-sniffer.exe 127.0.0.1 capture.cap

Replace `127.0.0.1` with the IP address for which packets should be captured,
and the file `capture.cap` with the name of the file to which to write packets.

Once finished simply press `CTRL+C` to stop the program.

Transfer the `capture.cap` to a host with Wireshark (or another similar
program) installed, and then open the `capture.cap` file using the tool.

# Changes

## Version 1.0.0 - 12/09/2018

 * Initial version

# License

Copyright 2018 NoSpaceships Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
