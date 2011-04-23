#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "opentap.h"

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        printf("Usage: ./createtap [-d] <dev-name> <owner-uid>\n");
        return 1;
    }
    
    const char* name;
    int owner;
    int del;
    if (strcmp(argv[1], "-d") == 0) {
        name = argv[2];
        del = 1;
    } else {
        name = argv[1];
        owner = atoi(argv[2]);
        del = 0;
    }
    
    int fd = opentap(name, 0);
    if (fd < 0) {
        return 1;
    }
    
    if (del) {
        if(ioctl(fd, TUNSETPERSIST, 0) < 0){
            printf("Failed to disable TUNSETPERSIST\n");
            return 1;
        }
    } else {
        if(ioctl(fd, TUNSETOWNER, owner) < 0){
            printf("Failed to set owner\n");
            return 1;
        }
        if(ioctl(fd, TUNSETPERSIST, 1) < 0){
            printf("Failed to enable TUNSETPERSIST\n");
            exit(1);
        }
    }

    return 0;
}

