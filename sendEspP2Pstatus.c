#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>

#define PORT 8266
#define BUFFER_SIZE 42  // Size of the byte sequence

void sendUDPMessage(int sockfd, const char *interface);

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address for broadcast
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    // Enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        perror("setsockopt (SO_BROADCAST) failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Sleep for 10 seconds
        sleep(10);

        // Send UDP message
        sendUDPMessage(sockfd, "enxb827eb5dd804");  // Assuming the ethernet port is named enxb827eb5dd804
    }

    // Close the socket (this line will not be reached in an infinite loop)
    close(sockfd);

    return 0;
}

void sendUDPMessage(int sockfd, const char *interface) {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char macAddress[18]; // Assuming a standard MAC address length
    char ipAddress[16]; // Assuming a standard IPv4 address length

    // Byte sequence
    buffer[0] = 255;
    buffer[1] = 1;

    // Get MAC address using system command
    char macCommand[100];
    snprintf(macCommand, sizeof(macCommand), "cat /sys/class/net/%s/address", interface);
    FILE *macFile = popen(macCommand, "r");
    if (macFile == NULL) {
        perror("Error reading MAC address");
        return;
    }
    fgets(macAddress, sizeof(macAddress), macFile);
    pclose(macFile);

    // Convert MAC address string to bytes
    sscanf(macAddress, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
           &buffer[2], &buffer[3], &buffer[4], &buffer[5], &buffer[6], &buffer[7]);

    // Get IP address using getifaddrs
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs failed");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET &&
            strcmp(ifa->ifa_name, interface) == 0) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), ipAddress, sizeof(ipAddress));
            break;
        }
    }

    freeifaddrs(ifaddr);

    // Print IP address to stdout
    printf("IP Address: %s\n", ipAddress);

    // Placeholder IP address (broadcast will be used)
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    // Copy IP address to bytes 9-12
    sscanf(ipAddress, "%hhu.%hhu.%hhu.%hhu", &buffer[8], &buffer[9], &buffer[10], &buffer[11]);

    // Unit number
    buffer[12] = 51;

    // ESPeasy data version number
    buffer[13] = 0;
    buffer[14] = 51;

    // Node name ("controller 51") filled with zeros if needed
    snprintf(&buffer[15], 25, "%-25s", "controller 51");

    // Additional byte
    buffer[40] = 1;

    // Send UDP packet
    if (sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Sendto failed");
    } else {
        printf("UDP message sent successfully\n");
    }
}
