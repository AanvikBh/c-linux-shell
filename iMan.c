#include "headers.h"

int iMan(char* argString[], int argCount, int port) {
    char *hostname = "man.he.net";
    char *command = argString[1];
    char ip[100];

    if (resolveHost(hostname, ip) != 0) {
        printf("Failed to resolve %s\n", hostname);
        exit(1);
    }

    

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }
    //printf("Connection established\n");

    // Prepare the HTTP GET request with the provided command
    char request[512];
    snprintf(request, sizeof(request), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\n\r\n", command, hostname);

    // Send the HTTP GET request
    if (write(sockfd, request, strlen(request)) == -1) {
        perror("write");
        close(sockfd);
        return 4;
    }

    // Receive and print the server's response
    char response[1000000]; // Increased buffer size to handle large HTML responses
    ssize_t bytes_read;
    size_t response_len = 0;

    while ((bytes_read = read(sockfd, response + response_len, sizeof(response) - 1 - response_len)) > 0) {
        response_len += bytes_read;
    }
    // printf("Received HTML:\n%s\n", response);
    response[response_len] = '\0';

    // printf("Received %zu bytes of HTML content\n", response_len);
    // printf("\n\n");
    char error_check[100];
    snprintf(error_check, sizeof(error_check), "No matches for ");
    if (strstr(response, error_check) != NULL) {
        printf("ERROR\n\t No such command\n");
    } else {
        extractBodyContent(response);
    }

    // Close the socket when you're done
    close(sockfd);

    return 0;
}

void extractBodyContent(char *html) {
    //char *body_start = strstr(html, "<body BGCOLOR=\"#ffffff\" LINK=\"#0000ff\" VLINK=\"#0000ff\" ALINK=\"#0000ff\" TEXT=\"#000000\">");
    char *body_start = strstr(html, "<PRE>");
    char *body_end = strstr(html, "</body>");

    if (body_start && body_end) {

        // body_start += strlen("<body BGCOLOR=\"#ffffff\" LINK=\"#0000ff\" VLINK=\"#0000ff\" ALINK=\"#0000ff\" TEXT=\"#000000\">");
        body_start += strlen("<PRE>");
        // Calculate the length of the body content
        size_t body_length = body_end - body_start;

        // Create a buffer to store the body content
        char body_content[body_length + 1];

        // Copy the body content to the buffer without modifying the original HTML
        strncpy(body_content, body_start, body_length);
        body_content[body_length] = '\0';
        
        char *body_new = strstr(body_content, "NAME");

        // Print the extracted body content
        printf("%s\n", body_new);
    } 
}

int resolveHost(char *hostname, char *ip) {
    struct hostent *hent;
    struct in_addr **addr_list;
    int i;
    if ((hent = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname error");
        return 1;
    }
    addr_list = (struct in_addr **)hent->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }
    return 1;
}


