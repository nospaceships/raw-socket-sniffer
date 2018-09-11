
# raw-socket-sniffer - Packet capture on Windows without drivers

This repository contains an program which demonstrates how to capture IP
packets on Windows using just raw sockets.

It requires no additional software, such as WinPCAP or npcap, and will simply
use existing operating system functionality.

# Getting Started

Once the project has been checked out, simply run the following command to
compile it:

    cd <repository>
    nmake

The file `raw-socket-sniffer.exe` will be placed in to the root of the
repository.

# Running a Packet Capture

Transfer the `raw-socket-sniffer.exe` program to the host on which packet
capture should be performed.  Then run the following command to capture
packets:

    raw-socket-sniffer.exe x.x.x.x capture.cap

Replace `x.x.x.x` with the IP address for which packets should be captured,
and the file `capture.cap` with the name of the file to which to write packets.

Once finished simply press `CTRL+C` to stop the program.

Transfer the `capture.cap` to a host with Wireshark (or similar program)
installed, and then open it using the tool.

# Changes

## Version 1.0.0 - 16/06/2018

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
