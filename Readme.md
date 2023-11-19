#EasyEsp & Raspberry Pi
A set of DIY home automation tools for easyEsp and raspberry pi.

###detectEsp.c
This program detects easyesp nodes in network if they have UDP configured.
Listens on port 8266 and decodes IP and node names according to https://espeasy.readthedocs.io/en/latest/Controller/C013.html

The decoded IP is stored in file with hostname as name in /dev/shm

###sendEspP2Pstatus.c
Sends IP and node name of Raspberry PI (set as "controller 51") to udp P2P network so that the actual easyEsp nodes will detect the Raspberry Pi. It fakes easyesp model and version. The purpose is only to be detected by Esp nodes.
The idea behind was to be able to communicate among nodes and Raspberry without having to know the IP address of the server.
