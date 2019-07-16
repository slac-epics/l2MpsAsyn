#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
  int type;
  int appId;
  char deviceName[50];
} restoreRequest;

typedef struct {
  int status;
  int appId;
  char statusMessage[200];
} restoreResponse;

static int requestRestore(char *server, int port, int appId) {
  int sock;
  struct sockaddr_in serverAddr;
  socklen_t addrSize;
  struct hostent *host;

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

  addrSize = sizeof(serverAddr);
  connect(sock, (struct sockaddr *) &serverAddr, addrSize);
  
  /* Send request */
  restoreRequest request;
  request.type = 3; /* Restore type */
  request.appId = appId;

  send(sock, (char *) &request, sizeof(request), 0);

  /* Wait for respose */
  restoreResponse response;
  read(sock, (char *) &response, sizeof(response));
  printf("%s\n", response.statusMessage);

  return 0;
}

int restoreThresholds(char *appIdString) {
  int len = strlen(appIdString);

  // ApplicationId string from environment variable should not have more that 4 chars or 
  // have none
  if (len == 0 || len > 4) {
    printf("ERROR: Invalid application ID string (%s).\n", appIdString);
    return 1;
  }

  char *endPtr;
  strtol(appIdString, &endPtr, 0);
  if (endPtr == appIdString) {
    printf("ERROR: Invalid application ID. Cannot convert string \"%s\" to a number.\n",
	   appIdString);
    return 1;
  }

  int appId = atoi(appIdString);
  printf("Restoring thresholds for app %d.\n", appId);

  char *server = "lcls-dev3";
  int port = 1975;
  requestRestore(server, port, appId);

  return 0;
}

