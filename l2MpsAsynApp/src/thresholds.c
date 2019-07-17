#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
  int type;
  int appId;
  char deviceName[50];
} restoreRequest;

typedef struct {
  int status; /* Status=10 is OK */
  int appId;
  char statusMessage[200];
} restoreResponse;

int restoreThresholds(char *server, int port, int appId) {
  int sock;
  struct sockaddr_in serverAddr;
  socklen_t addrSize;
  struct hostent *host;
  char *errorMessage;

  host = gethostbyname(server);
  if (host == NULL) {
    printf("ERROR: Invalid host name (%s).\n", server);
    return 1;
  }
  
  sock = socket(PF_INET, SOCK_STREAM, 0);

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  memcpy(&serverAddr.sin_addr, host->h_addr_list[0], host->h_length);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  printf("MPS: Connecting to MpsManager server at %s:%d\n", server, port);
  addrSize = sizeof(serverAddr);
  if (connect(sock, (struct sockaddr *) &serverAddr, addrSize) != 0) {
    printf("ERROR: Failed to connect to server at %s:%d (errno=%d)\n",
	   server, port, errno);
    perror(errorMessage);
    printf("ERROR: %s\n", errorMessage);
    return 1;
  }
  
  /* Send request */
  restoreRequest request;
  request.type = 3; /* Restore type */
  request.appId = appId;

  printf("MPS: Requesting thresholds for app=%d\n", appId);
  if (send(sock, (char *) &request, sizeof(request), 0) != sizeof(request)) {
    printf("ERROR: Failed to send threshold restore request to server at %s:%d\n",
	   server, port);
    return 1;
  }

  /* Wait for respose */
  printf("MPS: Waiting for response...\n");
  restoreResponse response;
  if (read(sock, (char *) &response, sizeof(response)) != sizeof(response)) {
    printf("ERROR: Failed to received threshold restore response from server at %s:%d\n",
	   server, port);
    return 1;
  }

  if (response.status != 10) {
    printf("ERROR: Received status %d from MpsManager server, error message:\n", response.status);
    printf("%s\n", response.statusMessage);
    return 1;
  }
  printf("MPS: Thresholds restored.\n");

  return 0;
}

