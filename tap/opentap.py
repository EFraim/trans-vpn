import ctypes

IFF_NO_PI = 0x1000  # do not provide packet information

lib = ctypes.CDLL("libopentap.so")

def opentap(devname):
    fd = lib.opentap(devname, IFF_NO_PI)
    if fd < 0:
        raise RuntimeError("Failed to open '%s' device" % devname)
    return fd

