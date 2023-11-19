#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define PORT 8266
#define BUFFER_SIZE 1024

void decodeAndWriteToFile(const char *buffer, size_t bufferLength) {
    if (bufferLength < 41) {
        return;
    }

    // Extract MAC address (bytes 3-8)
    char macAddress[18]; // Assuming a standard MAC address length
    snprintf(macAddress, sizeof(macAddress), "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
             (unsigned char)buffer[2], (unsigned char)buffer[3], (unsigned char)buffer[4],
             (unsigned char)buffer[5], (unsigned char)buffer[6], (unsigned char)buffer[7]);

    // Extract IP address (bytes 9-12)
    struct in_addr ipAddress;
    memcpy(&ipAddress, &buffer[8], sizeof(ipAddress));
    char ipAddressString[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddress, ipAddressString, sizeof(ipAddressString));

    // Extract string (25 bytes starting from byte 16)
    char extractedString[26]; // Assuming a null-terminated string
    memcpy(extractedString, &buffer[15], 25);
    extractedString[25] = '\0';

    // Write IP address to a file with the name of the extracted string in /dev/shm
    char filename[256]; // Adjust the size based on your requirements
    snprintf(filename, sizeof(filename), "/dev/shm/%s", extractedString);
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("File creation failed");
        return;
    }

    fprintf(file, "IP Address: %s\n", ipAddressString);
    fclose(file);
}

void daemonize() {
    pid_t pid, sid;

    // Fork the parent process
    pid = fork();

    // An error occurred
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    // If we got a good PID, then we can exit the parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Open any logs here

    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        // Log the failure
        exit(EXIT_FAILURE);
    }

    // Change the current working directory
    if ((chdir("/")) < 0) {
        // Log the failure
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    // Daemonize the process
    daemonize();

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Receive message
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                    (struct sockaddr*)&client_addr, &addr_len);

        // Decode and write to file
        decodeAndWriteToFile(buffer, recv_len);
    }

    // Close the socket (this line will not be reached in an infinite loop)
    close(sockfd);

    return 0;
}
