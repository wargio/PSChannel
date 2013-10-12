#include "debug_stuff.h"
static int SocketFD;

void debugPrintf(const char* fmt, ...)
{
  char buffer[0x800];
  va_list arg;
  va_start(arg, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, arg);
  va_end(arg);
  netSend(SocketFD, buffer, strlen(buffer), 0);
}

void debugInit()
{
  struct sockaddr_in stSockAddr;
  SocketFD = netSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  memset(&stSockAddr, 0, sizeof stSockAddr);

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(DEBUG_PORT);
  inet_pton(AF_INET, DEBUG_IP, &stSockAddr.sin_addr);

  netConnect(SocketFD, (struct sockaddr *)&stSockAddr, sizeof stSockAddr);
	
  debugPrintf("network debug module initialized\n") ;
  debugPrintf("ready to have a lot of fun\nHello deroad!\n") ;
}

void debugStop(){
}
