/*
 * client.c
 * Ritik Shah, rgshah@umass.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"


int main(int argc, char **argv)
{
    int clientfd;
    char *num1, *num2;
    char *host, *port;
    rio_t rio;
    size_t n;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <num1> <num2>\n", argv[0]);
        exit(0);
    }

    num1 = argv[1];
    num2 = argv[2];

    host = "localhost";
    port = "8080";

    char content[MAXLINE/2];
    char info[MAXLINE/2];
    char xml[MAXLINE];

    // Add xml version of the content sent to the server
    sprintf(content, "<?xml version=\'1.0\'?>\r\n"
                     "<methodCall>\r\n"
                     "<methodName>sample.addmultiply</methodName>\r\n"
                     "<params>\r\n"
                     "<param>\r\n"
                     "<value><double>%s</double></value>\r\n"
                     "</param>\r\n"
                     "<param>\r\n"
                     "<value><double>%s</double></value>\r\n"
                     "</param>\r\n"
                     "</params>\r\n"
                     "</methodCall>\r\n", num1, num2);

    // Add xml version of the info sent to the server
    sprintf(info, "POST /RPC2 HTTP/1.1\r\n"
                   "Host: localhost:8080\r\n"
                   "Accept-Encoding: gzip\r\n"
                   "User-Agent: xmlrpclib.py/1.0.1 (by www.pythonware.com)\r\n"
                   "Content-Type: text/xml\r\n"
                   "Content-Length: %d\r\n\r\n", strlen(content));

    // Add both the info and content in that order to the final xml format message that will be sent to the server
    strcat(xml, info);
    strcat(xml, content);

    char buf[MAXLINE];
    char str1[20]={'\0'};
    char str2[20];
    clientfd = Open_clientfd(host, port);
    // Read from the file descriptor
    Rio_readinitb(&rio, clientfd);
    // Send the file to the server
    Rio_writen(clientfd, xml, strlen(xml));
    // Find the numbers from the xml message that the server sends
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        const char delimiters[] = "< >< >";
        char *token = strtok(buf, delimiters); // Separate string by delimiters
        while (token != NULL) { // while string is not empty
            if (!strcmp(token, "double")) { // if there is a substr called "double"
                token = strtok(NULL, delimiters); // get next item in string
                if (str1[0] == '\0') { // if str1 is empty
                    // copy number to str1
                    strcpy(str1, token);
                }
                else { // if str1 has a value
                    // copy number to str2
                    strcpy(str2, token);
                }
            } else {
                token = strtok(NULL, delimiters);  // get next item in string
            }
        }
    }
    // Convert strings to float
    double val1 = atof(str1);
    double val2 = atof(str2);
    // Round value to upto 15 significat digits
    printf("%.15g  ", val1);
    printf(" %.15g\n", val2);
    Close(clientfd);
    exit(0);
}