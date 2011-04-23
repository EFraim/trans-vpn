#ifndef IAP_H
#define IAP_H
/* Defines In-Application-Programming interface wrappers
   targetAddress should by 256-aligned
   source should by word-aligned
   byteCount should be 256 | 512 | 1024 | 4096
*/

bool writeSector(void *targetAddress, int sectorNum, const void *source, unsigned byteCount);

bool eraseSector(int sectorNum);

#endif
