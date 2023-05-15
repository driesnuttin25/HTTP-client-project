#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h> //for all socket programming
#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
#include <stdio.h> //for fprintf, perror
#include <unistd.h> //for close
#include <stdlib.h> //for exit
#include <string.h> //for memset
#include <stdbool.h>

void OSInit( void )
{
    WSADATA wsaData;
    int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
    if( WSAError != 0 )
    {
        fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
        exit( -1 );
    }
}
void OSCleanup( void )
{
    WSACleanup();
}
#define perror(string) fprintf( stderr, string ": WSA errno = %d\n", WSAGetLastError() )
#else
#include <sys/socket.h> //for sockaddr, socket, socket
	#include <sys/types.h> //for size_t
	#include <netdb.h> //for getaddrinfo
	#include <netinet/in.h> //for sockaddr_in
	#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
	#include <errno.h> //for errno
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif

int initialization();
int connection( int internet_socket );
void execution( int internet_socket );
void cleanup( int internet_socket, int client_internet_socket );

int main( int argc, char * argv[] )
{
    printf("Test, Test.... Does this work?!\n");
    //////////////////
    //Initialization//
    //////////////////

    OSInit();

    int internet_socket = initialization();

    //////////////
    //Connection//
    //////////////

    int client_internet_socket = connection( internet_socket );

    /////////////
    //Execution//
    /////////////

    execution( client_internet_socket );


    ////////////
    //Clean up//
    ////////////

    cleanup( internet_socket, client_internet_socket );

    OSCleanup();

    return 0;
}

int initialization()
{
    //Step 1.1
    struct addrinfo internet_address_setup;
    struct addrinfo * internet_address_result;
    memset( &internet_address_setup, 0, sizeof internet_address_setup );
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_STREAM;
    internet_address_setup.ai_flags = AI_PASSIVE;
    int getaddrinfo_return = getaddrinfo( NULL, "22", &internet_address_setup, &internet_address_result );
    if( getaddrinfo_return != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
        exit( 1 );
    }

    int internet_socket = -1;
    struct addrinfo * internet_address_result_iterator = internet_address_result;
    while( internet_address_result_iterator != NULL )
    {
        //Step 1.2
        internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
        if( internet_socket == -1 )
        {
            perror( "socket" );
        }
        else
        {
            //Step 1.3
            int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
            if( bind_return == -1 )
            {
                perror( "bind" );
                close( internet_socket );
            }
            else
            {
                //Step 1.4
                int listen_return = listen( internet_socket, SOMAXCONN ); // use SOMAXCONN for a system-defined maximum backlog value
                if( listen_return == -1 )
                {
                    close( internet_socket );
                    perror( "listen" );
                }
                else
                {
                    break;
                }
            }
        }
        internet_address_result_iterator = internet_address_result_iterator->ai_next;
    }

    freeaddrinfo( internet_address_result );

    if( internet_socket == -1 )
    {
        fprintf( stderr, "socket: no valid socket address found\n" );
        exit( 2 );
    }

    return internet_socket;
}

char ip_address[INET6_ADDRSTRLEN];

int connection(int internet_socket) {
    //Step 2.1
    struct sockaddr_storage client_internet_address;
    socklen_t client_internet_address_length = sizeof client_internet_address;
    int client_socket = accept(internet_socket, (struct sockaddr *)&client_internet_address, &client_internet_address_length);
    if (client_socket == -1) {
        perror("accept");
        close(internet_socket);
        exit(3);
    }

    // INET6 contains the exact length of a human-readable IPV6 adress.
    void *addr;
    if (client_internet_address.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&client_internet_address;
        addr = &(s->sin_addr);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&client_internet_address;
        addr = &(s->sin6_addr);
    }
    // Convert binary IP to human-readable ip adress.
    inet_ntop(client_internet_address.ss_family, addr, ip_address, sizeof ip_address);

    // Log IP address
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("fopen");
        close(client_socket);
        exit(4);
    }
    fprintf(log_file, "------------------------\n");
    fprintf(log_file, "Connection from %s\n", ip_address);
    fprintf(log_file, "------------------------\n");
    fclose(log_file);

    return client_socket;
}


// Function to send an HTTP GET request
void http_request(int internet_socket, const char *ip_address)
{
    char request[1000];
    snprintf(request, sizeof(request), "GET /json/%s HTTP/1.0\r\nHost: ip-api.com\r\n\r\n", ip_address);

    // Step 1: Set up destination address
    struct addrinfo hints;
    struct addrinfo *dest_addr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int getaddrinfo_return = getaddrinfo("208.95.112.1", "80", &hints, &dest_addr);
    if (getaddrinfo_return != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_return));
        exit(1);
    }

    // Step 2: Connect to the destination address
    int connection_return = connect(internet_socket, dest_addr->ai_addr, dest_addr->ai_addrlen);
    if (connection_return == -1)
    {
        perror("connect");
        exit(1);
    }

    freeaddrinfo(dest_addr);

    // Step 3: Send the HTTP GET request
    int number_of_bytes_send = send(internet_socket, request, strlen(request), 0);
    if (number_of_bytes_send == -1)
    {
        perror("send");
    }

    // Step 4: Receive and print the response
    char buffer[1000];
    int number_of_bytes_received;
    bool header_complete = false;

    while (!header_complete)
    {
        number_of_bytes_received = recv(internet_socket, buffer, (sizeof buffer) - 1, 0);
        if (number_of_bytes_received == -1)
        {
            perror("recv");
            break;
        }
        else if (number_of_bytes_received == 0)
        {
            // Connection closed by the server
            break;
        }
        else
        {
            buffer[number_of_bytes_received] = '\0';
            printf("%s", buffer);

            // Check if the header is complete
            char *header_end = strstr(buffer, "\r\n\r\n");
            if (header_end != NULL)
            {
                header_complete = true;
                int body_start_index = header_end - buffer + 4;
                printf("Response Body:\n%s\n", &buffer[body_start_index]);
            }
        }
    }
}

void execution(int internet_socket)
{
    // Step 1: Receive initial data
    printf("Test, Test.... Does this work?!\n");
    char buffer[1000];
    int number_of_bytes_received = recv(internet_socket, buffer, (sizeof buffer) - 1, 0);
    if (number_of_bytes_received == -1)
    {
        perror("recv");
    }
    else
    {
        buffer[number_of_bytes_received] = '\0';
        printf("Received: %s\n", buffer);
    }

    // Step 2: Send HTTP GET request
    http_request(internet_socket, ip_address);

    // Receive and save the response in log.txt
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL)
    {
        perror("fopen");
        close(internet_socket);
        exit(4);
    }

    fprintf(log_file, "------------------------\n");
    fprintf(log_file, "Response from GET request:\n%s\n", buffer);
    fprintf(log_file, "------------------------\n");

    fclose(log_file);
}

void cleanup( int internet_socket, int client_internet_socket )
{
    //Step 4.2
    int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
    if( shutdown_return == -1 )
    {
        perror( "shutdown" );
    }

    //Step 4.1
    close( client_internet_socket );
    close( internet_socket );
}