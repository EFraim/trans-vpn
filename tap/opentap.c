
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <string.h>
#include <stdio.h>

int opentap(const char *dev, int flags) {
    
    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";
    
    /* Arguments taken by the function:
     * 
     * char *dev: the name of an interface (or '\0'). MUST have enough
     *   space to hold the interface name if '\0' is passed
     * int flags: interface flags (eg, IFF_TUN etc.)
     */
    
    /* open the clone device */
    if( (fd = open(clonedev, O_RDWR)) < 0 ) {
        printf("Failed opening device\n");
        return fd;
    }
    
    /* preparation of the struct ifr, of type "struct ifreq" */
    memset(&ifr, 0, sizeof(ifr));
    
    ifr.ifr_flags = IFF_TAP | flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */
    
    if (*dev) {
        /* if a device name was specified, put it in the structure; otherwise,
         * the kernel will try to allocate the "next" device of the
         * specified type */
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }
    
    /* try to create the device */
    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
        printf("Failed creating device\n");
        close(fd);
        return err;
    }
    
    /* this is the special file descriptor that the caller will use to talk
     * with the virtual interface */
    return fd;
}
