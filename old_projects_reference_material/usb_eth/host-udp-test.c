// compile with c:\programs\MinGW\bin\gcc -o host-udp-test.exe host-udp-test.c  -lwsock32
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "endian.c"

int main() {

  WSADATA wsaData;

  printf("starting WSA\n");

  /* Open windows connection */
  if (WSAStartup(0x0101, &wsaData) != 0) {
    fprintf(stderr, "Could not open Windows connection.\n");
    exit(0);
  }

  printf("opening UDP socket\n");

  /* Open a datagram socket */
  int sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd == INVALID_SOCKET) {
      fprintf(stderr, "Could not create socket.\n");
      WSACleanup();
      exit(0);
  }

  printf("preparing sockaddr_in\n");

  struct sockaddr_in local_addr;
  struct sockaddr_in remote_addr;

  memset((void *)&local_addr, 0, sizeof(struct sockaddr_in));

  local_addr.sin_family      = AF_INET;
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  local_addr.sin_port        = htons(1208);

  /* Bind address to socket */
  if (bind(sd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in)) == -1) {
    fprintf(stderr, "Could not bind name to socket: error %d\n",WSAGetLastError());
    //fprintf(stderr, "Could not bind name to socket.\n");
    closesocket(sd);
    WSACleanup();
    exit(0);
  }

  static uint8_t buf[8192];
  int count;

  while (1) {
    fd_set fd_read;
    FD_ZERO(&fd_read);
    //FD_SET(packetizerReadfd,&fd_read);
    FD_SET(sd,&fd_read);

    struct timeval timeout;
    timeout.tv_sec=0;
    timeout.tv_usec=1000000; //25000; /* 25ms */

    int count=select(FD_SETSIZE,&fd_read,0,0,&timeout);
    if (count == 0) {
      printf("timeout in select\n");
    }
    if(count<0) {
      //if (errno == EINTR) return -1;
      printf("Error in select"); // ,strerror(errno));
      exit(1);
    }
    if(count>0) {
      if (FD_ISSET(sd,&fd_read)) {

        int remote_len = (int)sizeof(struct sockaddr_in);
        int rcvsize = recvfrom(sd,buf,sizeof(buf),0,(struct sockaddr *) &remote_addr,&remote_len);
        if (rcvsize < 0) {
            fprintf(stderr, "Could not receive datagram.\n");
            closesocket(sd);
            WSACleanup();
            exit(0);
        }

        uint8_t* remote_ip   = (uint8_t*) &( ((struct sockaddr_in*) &remote_addr)->sin_addr );
        uint8_t* remote_port = (uint8_t*) &( ((struct sockaddr_in*) &remote_addr)->sin_port );
        uint16_t port = (remote_port[0] << 8) + remote_port[1];

        printf(">>> received a UDP packet (%d bytes) from %d.%d.%d.%d:%u\n",
               rcvsize,
               remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3],port);

      } /* file descriptor is set */
    } /* positive receive byte count */

    uint16ToLittleEndian('P',buf);
    count = sendto(sd,
                   buf, 2,
                   0, // flags
                   (struct sockaddr *)&remote_addr, sizeof(struct sockaddr_in));
    if (count != 2) {
      fprintf(stderr, "Error sending datagram.\n");
      //closesocket(sd);
      //WSACleanup();
      //exit(0);
    }

  }

#if 0
  while (1) {
    Sleep(1000);
    printf("sending a packet...\n");
    if (sendto(sd,
               "1234", 4,
               0, // flags
               (struct sockaddr *)&remote, sizeof(struct sockaddr_in)) !=
               4)
      {
          fprintf(stderr, "Error sending datagram.\n");
          closesocket(sd);
          WSACleanup();
          exit(0);
      }

    int bytes_received;
    char buffer[256];
    bytes_received = recv(sd, buffer, 256, 0);
    if (bytes_received < 0)
    {

        fprintf(stderr, "Could not receive datagram.\n");
        closesocket(sd);
        WSACleanup();
        exit(0);
    }
    int i;
    printf("received UDP packet: ");
    for (i=0; i<bytes_received; i++) printf("%c",buffer[i]);
    printf("\n");

  }
#endif
}
