#include "appcon.h"

#include "log.h"
#include "drivers/usb.h"
#include "usbcon.h"

void appcon_loop() {
    for(;;) {
      LOG_DEBUG("I am still alive!\n");
    }
}
