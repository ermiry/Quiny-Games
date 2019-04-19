#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>

#include <errno.h>

#include "picoParser.h"

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
    serverAddress.sin_port = htons (9001);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // bind our socket
    int binding = bind (serverSocket, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
    if (binding < 0) {
        fprintf (stderr, "Error binding the server socket!!\n\n");
        return 1;
    }

    char buf[4096], *method, *path;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;

    // if the binding is succesfull, we can listen for connections
    listen (serverSocket, 5);

    // the connection that we will create
    int clientScoket;
    // we want the server to continue respond to requests
    printf ("Waiting for requests...\n");
    while (1) {
        clientScoket = accept (serverSocket, NULL, NULL);
        // if (clientScoket > 0) {
        //     printf ("Sending request to socket %i.\n", clientScoket);
        //     send (clientScoket, res, sizeof (res), 0);
        //     close (clientScoket);
        //     printf ("Connection to socket %i closed.\n", clientScoket);
        // }

        /* read the request */
        while ((rret = read (clientScoket, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR)

        printf ("buffer: %s\n", buf);

        // if (rret <= 0)
        //     return IOError;
        prevbuflen = buflen;
        buflen += rret;
        /* parse the request */
        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(buf, buflen, (const char **) &method, &method_len, (const char **) &path, &path_len,
                                &minor_version, headers, &num_headers, prevbuflen);
        if (pret > 0) {
            printf("\nrequest is %d bytes long\n", pret);
            printf("method is %.*s\n", (int)method_len, method);
            printf("path is %.*s\n", (int)path_len, path);
            printf("HTTP version is 1.%d\n", minor_version);
            printf("headers:\n");
            for (int i = 0; i != num_headers; ++i) {
                printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
                    (int)headers[i].value_len, headers[i].value);
            }
            printf ("\n");
            
            //  break; /* successfully parsed the request */
        }
           
        // else if (pret == -1)
        //     return ParseError;
        // /* request is incomplete, continue the loop */
        // assert(pret == -2);
        // if (buflen == sizeof(buf))
        //     return RequestIsTooLongError;

        }
    
    close (serverSocket);

    return 0;

}