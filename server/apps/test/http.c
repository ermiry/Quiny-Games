#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>

int main (void) {

    char *fileName = "response.txt";
    FILE *htmlData = fopen (fileName, "r");
    if (htmlData == NULL) {
        fprintf (stderr, "Error openning file '%s'\n\n", fileName);
        return 1;
    }

    char res[1024] = "HTTP/1.1 200 OK\r\n\nHello World";
    // memset (res, 0, 1024);
    // char buffer[128];
    // while (fgets (buffer, 128, htmlData))
    //     strcat (res, buffer);

    printf ("%s\n", res);

    // char responseData[1024];
    // fgets (responseData, sizeof (responseData), htmlData);

    // char httpHeader[2048] = "HTTP/1.1 200 OK\r\n\n";  // the response to the request is okay
    // strcat (httpHeader, responseData);

    // char httpHeader[2048] = "HTTP/1.1 200 OK
    //     Date: Sun, 10 Oct 2010 23:26:07 GMT
    //     Server: Apache/2.2.8 (Ubuntu) mod_ssl/2.2.8 OpenSSL/0.9.8g\
    //     Last-Modified: Sun, 26 Sep 2010 22:04:35 GMT\
    //     Accept-Ranges: bytes\
    //     Content-Length: 12\
    //     Connection: close\
    //     Content-Type: text/html\
    //     Hello world!";

    // create a socket
    int serverSocket;
    // this are the same parameters as of a TCP connection
    serverSocket = socket (AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        fprintf(stderr, "Error creating the server socket!!\n\n");
        return 1;
    }

    // define the addres
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons (7000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // bind our socket
    int binding = bind (serverSocket, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
    if (binding < 0) {
        fprintf (stderr, "Error binding the server socket!!\n\n");
        return 1;
    }

    // if the binding is succesfull, we can listen for connections
    listen (serverSocket, 5);

    // the connection that we will create
    int clientScoket;
    // we want the server to continue respond to requests
    printf ("Waiting for requests...\n");
    while (1) {
        clientScoket = accept (serverSocket, NULL, NULL);
        if (clientScoket > 0) {
            printf ("Sending request to socket %i.\n", clientScoket);
            send (clientScoket, res, sizeof (res), 0);
            close (clientScoket);
            printf ("Connection to socket %i closed.\n", clientScoket);
        }

    }

    return 0;

}