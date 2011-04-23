# 1 "../common/app.c"
# 1 "/home/pasha/embedded/ethvpn/hw/sim//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "../common/app.c"
# 1 "../common/app.h" 1



void app_loop();
# 2 "../common/app.c" 2

# 1 "./drivers/enc28j60.h" 1



# 1 "/usr/include/stdint.h" 1 3 4
# 26 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 322 "/usr/include/features.h" 3 4
# 1 "/usr/include/bits/predefs.h" 1 3 4
# 323 "/usr/include/features.h" 2 3 4
# 355 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 353 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 354 "/usr/include/sys/cdefs.h" 2 3 4
# 356 "/usr/include/features.h" 2 3 4
# 387 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4



# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 5 "/usr/include/gnu/stubs.h" 2 3 4


# 1 "/usr/include/gnu/stubs-32.h" 1 3 4
# 8 "/usr/include/gnu/stubs.h" 2 3 4
# 388 "/usr/include/features.h" 2 3 4
# 27 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/bits/wchar.h" 1 3 4
# 28 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/stdint.h" 2 3 4
# 37 "/usr/include/stdint.h" 3 4
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;



__extension__
typedef long long int int64_t;




typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;





__extension__
typedef unsigned long long int uint64_t;






typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;



__extension__
typedef long long int int_least64_t;



typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;



__extension__
typedef unsigned long long int uint_least64_t;






typedef signed char int_fast8_t;





typedef int int_fast16_t;
typedef int int_fast32_t;
__extension__
typedef long long int int_fast64_t;



typedef unsigned char uint_fast8_t;





typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
__extension__
typedef unsigned long long int uint_fast64_t;
# 126 "/usr/include/stdint.h" 3 4
typedef int intptr_t;


typedef unsigned int uintptr_t;
# 138 "/usr/include/stdint.h" 3 4
__extension__
typedef long long int intmax_t;
__extension__
typedef unsigned long long int uintmax_t;
# 5 "./drivers/enc28j60.h" 2

void enc28j60_init(void* iodir, void* iopin, uint8_t macaddr[],
                   const char* myport, const char* otherport);

void enc28j60_get_mac_address(uint8_t *macaddr);

void enc28j60_packet_send(uint32_t len, uint8_t *packet);
unsigned int enc28j60_packet_receive(uint32_t maxlen, uint8_t *packet);
# 4 "../common/app.c" 2
# 1 "./usbnet.h" 1





void usbnet_init(const char* myport, const char* otherport);

int usbnet_send(uint8_t* buffer, uint16_t length);
int usbnet_recv(uint8_t* buffer, uint16_t length);

int usbnet_pop_completed_send();
int usbnet_pop_completed_recv();
# 5 "../common/app.c" 2
# 1 "./log.h" 1



# 1 "/usr/include/stdio.h" 1 3 4
# 30 "/usr/include/stdio.h" 3 4




# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 1 3 4
# 211 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 3 4
typedef unsigned int size_t;
# 35 "/usr/include/stdio.h" 2 3 4

# 1 "/usr/include/bits/types.h" 1 3 4
# 28 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;




__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;







__extension__ typedef long long int __quad_t;
__extension__ typedef unsigned long long int __u_quad_t;
# 131 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/typesizes.h" 1 3 4
# 132 "/usr/include/bits/types.h" 2 3 4


__extension__ typedef __u_quad_t __dev_t;
__extension__ typedef unsigned int __uid_t;
__extension__ typedef unsigned int __gid_t;
__extension__ typedef unsigned long int __ino_t;
__extension__ typedef __u_quad_t __ino64_t;
__extension__ typedef unsigned int __mode_t;
__extension__ typedef unsigned int __nlink_t;
__extension__ typedef long int __off_t;
__extension__ typedef __quad_t __off64_t;
__extension__ typedef int __pid_t;
__extension__ typedef struct { int __val[2]; } __fsid_t;
__extension__ typedef long int __clock_t;
__extension__ typedef unsigned long int __rlim_t;
__extension__ typedef __u_quad_t __rlim64_t;
__extension__ typedef unsigned int __id_t;
__extension__ typedef long int __time_t;
__extension__ typedef unsigned int __useconds_t;
__extension__ typedef long int __suseconds_t;

__extension__ typedef int __daddr_t;
__extension__ typedef long int __swblk_t;
__extension__ typedef int __key_t;


__extension__ typedef int __clockid_t;


__extension__ typedef void * __timer_t;


__extension__ typedef long int __blksize_t;




__extension__ typedef long int __blkcnt_t;
__extension__ typedef __quad_t __blkcnt64_t;


__extension__ typedef unsigned long int __fsblkcnt_t;
__extension__ typedef __u_quad_t __fsblkcnt64_t;


__extension__ typedef unsigned long int __fsfilcnt_t;
__extension__ typedef __u_quad_t __fsfilcnt64_t;

__extension__ typedef int __ssize_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


__extension__ typedef int __intptr_t;


__extension__ typedef unsigned int __socklen_t;
# 37 "/usr/include/stdio.h" 2 3 4
# 45 "/usr/include/stdio.h" 3 4
struct _IO_FILE;



typedef struct _IO_FILE FILE;





# 65 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 75 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/libio.h" 1 3 4
# 32 "/usr/include/libio.h" 3 4
# 1 "/usr/include/_G_config.h" 1 3 4
# 15 "/usr/include/_G_config.h" 3 4
# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 1 3 4
# 16 "/usr/include/_G_config.h" 2 3 4




# 1 "/usr/include/wchar.h" 1 3 4
# 83 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {

    unsigned int __wch;



    char __wchb[4];
  } __value;
} __mbstate_t;
# 21 "/usr/include/_G_config.h" 2 3 4

typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 53 "/usr/include/_G_config.h" 3 4
typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
# 33 "/usr/include/libio.h" 2 3 4
# 53 "/usr/include/libio.h" 3 4
# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stdarg.h" 1 3 4
# 40 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 54 "/usr/include/libio.h" 2 3 4
# 170 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 180 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 203 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 271 "/usr/include/libio.h" 3 4
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;



  int _flags2;

  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 319 "/usr/include/libio.h" 3 4
  __off64_t _offset;
# 328 "/usr/include/libio.h" 3 4
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;

  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 364 "/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
# 416 "/usr/include/libio.h" 3 4
extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
# 460 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));
# 490 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));
# 76 "/usr/include/stdio.h" 2 3 4




typedef __gnuc_va_list va_list;
# 91 "/usr/include/stdio.h" 3 4
typedef __off_t off_t;
# 103 "/usr/include/stdio.h" 3 4
typedef __ssize_t ssize_t;







typedef _G_fpos_t fpos_t;




# 161 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/bits/stdio_lim.h" 1 3 4
# 162 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;







extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));




extern int renameat (int __oldfd, __const char *__old, int __newfd,
       __const char *__new) __attribute__ ((__nothrow__));








extern FILE *tmpfile (void) ;
# 206 "/usr/include/stdio.h" 3 4
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__)) ;





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__)) ;
# 224 "/usr/include/stdio.h" 3 4
extern char *tempnam (__const char *__dir, __const char *__pfx)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 249 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 263 "/usr/include/stdio.h" 3 4






extern FILE *fopen (__const char *__restrict __filename,
      __const char *__restrict __modes) ;




extern FILE *freopen (__const char *__restrict __filename,
        __const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 292 "/usr/include/stdio.h" 3 4

# 303 "/usr/include/stdio.h" 3 4
extern FILE *fdopen (int __fd, __const char *__modes) __attribute__ ((__nothrow__)) ;
# 316 "/usr/include/stdio.h" 3 4
extern FILE *fmemopen (void *__s, size_t __len, __const char *__modes)
  __attribute__ ((__nothrow__)) ;




extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__)) ;






extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__));





extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__));








extern int fprintf (FILE *__restrict __stream,
      __const char *__restrict __format, ...);




extern int printf (__const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      __const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       __const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

# 414 "/usr/include/stdio.h" 3 4
extern int vdprintf (int __fd, __const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, __const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));








extern int fscanf (FILE *__restrict __stream,
     __const char *__restrict __format, ...) ;




extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
     __const char *__restrict __format, ...) __attribute__ ((__nothrow__));
# 445 "/usr/include/stdio.h" 3 4
extern int fscanf (FILE *__restrict __stream, __const char *__restrict __format, ...) __asm__ ("" "__isoc99_fscanf") ;


extern int scanf (__const char *__restrict __format, ...) __asm__ ("" "__isoc99_scanf") ;

extern int sscanf (__const char *__restrict __s, __const char *__restrict __format, ...) __asm__ ("" "__isoc99_sscanf") __attribute__ ((__nothrow__));
# 465 "/usr/include/stdio.h" 3 4








extern int vfscanf (FILE *__restrict __s, __const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (__const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (__const char *__restrict __s,
      __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__scanf__, 2, 0)));
# 496 "/usr/include/stdio.h" 3 4
extern int vfscanf (FILE *__restrict __s, __const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfscanf")



     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (__const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vscanf")

     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (__const char *__restrict __s, __const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vsscanf") __attribute__ ((__nothrow__))



     __attribute__ ((__format__ (__scanf__, 2, 0)));
# 524 "/usr/include/stdio.h" 3 4









extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

# 552 "/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 563 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 596 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;






extern char *gets (char *__s) ;

# 658 "/usr/include/stdio.h" 3 4
extern __ssize_t __getdelim (char **__restrict __lineptr,
          size_t *__restrict __n, int __delimiter,
          FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
        size_t *__restrict __n, int __delimiter,
        FILE *__restrict __stream) ;







extern __ssize_t getline (char **__restrict __lineptr,
       size_t *__restrict __n,
       FILE *__restrict __stream) ;








extern int fputs (__const char *__restrict __s, FILE *__restrict __stream);





extern int puts (__const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);

# 730 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream);








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 766 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 785 "/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, __const fpos_t *__pos);
# 808 "/usr/include/stdio.h" 3 4

# 817 "/usr/include/stdio.h" 3 4


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;








extern void perror (__const char *__s);






# 1 "/usr/include/bits/sys_errlist.h" 1 3 4
# 27 "/usr/include/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern __const char *__const sys_errlist[];
# 847 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
# 866 "/usr/include/stdio.h" 3 4
extern FILE *popen (__const char *__command, __const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__));
# 906 "/usr/include/stdio.h" 3 4
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__));
# 936 "/usr/include/stdio.h" 3 4

# 5 "./log.h" 2
# 6 "../common/app.c" 2
# 1 "./util.h" 1
# 9 "./util.h"
void sim_sleep(int ms);




uint32_t get_current_time();





int time_ms_diff(uint32_t t1, uint32_t t2);
# 7 "../common/app.c" 2
# 1 "../common/pkt_channel.h" 1



# 1 "../common/channel_interface.h" 1



# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 1 3 4
# 149 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 323 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 3 4
typedef int wchar_t;
# 5 "../common/channel_interface.h" 2



typedef void (*channel_input_function_t)(void* state, void* data, size_t size);
typedef void (*channel_poll_function_t)(void* state);


typedef int (*channel_send_function_t)(void* state, void* data, size_t size);
typedef void (*channel_close_function_t)(void* state);

typedef struct {





    void* upper_state;

    channel_input_function_t input_function;

    channel_poll_function_t poll_function;





    size_t header_size;


    void* lower_state;

    channel_send_function_t send_function;

    channel_close_function_t close_function;

} channel_interface_t;
# 5 "../common/pkt_channel.h" 2
# 24 "../common/pkt_channel.h"
enum PacketChannelPacketState {
    PKT_CHANNEL_NEW_PACKET,
    PKT_CHANNEL_PARTIAL_PACKET
};


typedef void* (*allocate_buffer_function_t)(size_t* size);
typedef void (*free_buffer_function_t)(void* data);




typedef struct {

    channel_interface_t interface;


    allocate_buffer_function_t allocate_buffer;


    free_buffer_function_t free_buffer;


    enum PacketChannelPacketState packet_state;


    void* work_buffer;


    size_t work_buffer_size;


    size_t current_pkt_total_size;


    size_t current_pkt_recv_size;
} pkt_channel_state_t;



void pkt_channel_init(pkt_channel_state_t* state, channel_interface_t* interface,
                         allocate_buffer_function_t allocate_buffer,
                         free_buffer_function_t free_buffer);

void pkt_channel_reset(pkt_channel_state_t* state);
void pkt_channel_input_packet(pkt_channel_state_t* state, void* data, size_t size);
void pkt_channel_poll(pkt_channel_state_t* state);
int pkt_channel_send(pkt_channel_state_t* state, void* data, size_t size);
void pkt_channel_close(pkt_channel_state_t* state);

size_t pkt_channel_header_size();
# 8 "../common/app.c" 2
# 1 "../common/secure_channel.h" 1







# 1 "../../external/polarssl-0.14.0/include/polarssl/aes.h" 1
# 37 "../../external/polarssl-0.14.0/include/polarssl/aes.h"
typedef struct
{
    int nr;
    unsigned long *rk;
    unsigned long buf[68];
}
aes_context;
# 58 "../../external/polarssl-0.14.0/include/polarssl/aes.h"
int aes_setkey_enc( aes_context *ctx, const unsigned char *key, int keysize );
# 69 "../../external/polarssl-0.14.0/include/polarssl/aes.h"
int aes_setkey_dec( aes_context *ctx, const unsigned char *key, int keysize );
# 81 "../../external/polarssl-0.14.0/include/polarssl/aes.h"
int aes_crypt_ecb( aes_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] );
# 100 "../../external/polarssl-0.14.0/include/polarssl/aes.h"
int aes_crypt_cbc( aes_context *ctx,
                    int mode,
                    int length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );
# 120 "../../external/polarssl-0.14.0/include/polarssl/aes.h"
int aes_crypt_cfb128( aes_context *ctx,
                       int mode,
                       int length,
                       int *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output );






int aes_self_test( int verbose );
# 9 "../common/secure_channel.h" 2
# 1 "../../external/polarssl-0.14.0/include/polarssl/rsa.h" 1
# 28 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
# 1 "../../external/polarssl-0.14.0/include/polarssl/bignum.h" 1
# 51 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
  typedef unsigned long t_int;
# 71 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
typedef struct
{
    int s;
    int n;
    t_int *p;
}
mpi;
# 86 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
void mpi_init( mpi *X, ... );




void mpi_free( mpi *X, ... );
# 102 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_grow( mpi *X, int nblimbs );
# 113 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_copy( mpi *X, const mpi *Y );







void mpi_swap( mpi *X, mpi *Y );
# 132 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_lset( mpi *X, int z );






int mpi_lsb( const mpi *X );






int mpi_msb( const mpi *X );






int mpi_size( const mpi *X );
# 164 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_read_string( mpi *X, int radix, const char *s );
# 181 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_write_string( const mpi *X, int radix, char *s, int *slen );
# 192 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_read_file( mpi *X, int radix, FILE *fin );
# 206 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_write_file( const char *p, const mpi *X, int radix, FILE *fout );
# 218 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_read_binary( mpi *X, const unsigned char *buf, int buflen );
# 230 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_write_binary( const mpi *X, unsigned char *buf, int buflen );
# 241 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_shift_l( mpi *X, int count );
# 252 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_shift_r( mpi *X, int count );
# 264 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_cmp_abs( const mpi *X, const mpi *Y );
# 276 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_cmp_mpi( const mpi *X, const mpi *Y );
# 288 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_cmp_int( const mpi *X, int z );
# 300 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_add_abs( mpi *X, const mpi *A, const mpi *B );
# 312 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_sub_abs( mpi *X, const mpi *A, const mpi *B );
# 324 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_add_mpi( mpi *X, const mpi *A, const mpi *B );
# 336 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_sub_mpi( mpi *X, const mpi *A, const mpi *B );
# 348 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_add_int( mpi *X, const mpi *A, int b );
# 360 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_sub_int( mpi *X, const mpi *A, int b );
# 372 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_mul_mpi( mpi *X, const mpi *A, const mpi *B );
# 386 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_mul_int( mpi *X, const mpi *A, t_int b );
# 402 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_div_mpi( mpi *Q, mpi *R, const mpi *A, const mpi *B );
# 418 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_div_int( mpi *Q, mpi *R, const mpi *A, int b );
# 432 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_mod_mpi( mpi *R, const mpi *A, const mpi *B );
# 446 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_mod_int( t_int *r, const mpi *A, int b );
# 465 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_exp_mod( mpi *X, const mpi *A, const mpi *E, const mpi *N, mpi *_RR );
# 477 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_gcd( mpi *G, const mpi *A, const mpi *B );
# 491 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_inv_mod( mpi *X, const mpi *A, const mpi *N );
# 504 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_is_prime( mpi *X, int (*f_rng)(void *), void *p_rng );
# 519 "../../external/polarssl-0.14.0/include/polarssl/bignum.h"
int mpi_gen_prime( mpi *X, int nbits, int dh_flag,
                   int (*f_rng)(void *), void *p_rng );






int mpi_self_test( int verbose );
# 29 "../../external/polarssl-0.14.0/include/polarssl/rsa.h" 2
# 126 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
typedef struct
{
    int ver;
    int len;

    mpi N;
    mpi E;

    mpi D;
    mpi P;
    mpi Q;
    mpi DP;
    mpi DQ;
    mpi QP;

    mpi RN;
    mpi RP;
    mpi RQ;

    int padding;
    int hash_id;
}
rsa_context;
# 167 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
void rsa_init( rsa_context *ctx,
               int padding,
               int hash_id);
# 185 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_gen_key( rsa_context *ctx,
                 int (*f_rng)(void *),
                 void *p_rng,
                 int nbits, int exponent );
# 197 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_check_pubkey( const rsa_context *ctx );
# 206 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_check_privkey( const rsa_context *ctx );
# 224 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_public( rsa_context *ctx,
                const unsigned char *input,
                unsigned char *output );
# 240 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_private( rsa_context *ctx,
                 const unsigned char *input,
                 unsigned char *output );
# 260 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_pkcs1_encrypt( rsa_context *ctx,
                       int (*f_rng)(void *),
                       void *p_rng,
                       int mode, int ilen,
                       const unsigned char *input,
                       unsigned char *output );
# 283 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_pkcs1_decrypt( rsa_context *ctx,
                       int mode, int *olen,
                       const unsigned char *input,
                       unsigned char *output,
                 int output_max_len );
# 305 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_pkcs1_sign( rsa_context *ctx,
                    int mode,
                    int hash_id,
                    int hashlen,
                    const unsigned char *hash,
                    unsigned char *sig );
# 328 "../../external/polarssl-0.14.0/include/polarssl/rsa.h"
int rsa_pkcs1_verify( rsa_context *ctx,
                      int mode,
                      int hash_id,
                      int hashlen,
                      const unsigned char *hash,
                      unsigned char *sig );






void rsa_free( rsa_context *ctx );






int rsa_self_test( int verbose );
# 10 "../common/secure_channel.h" 2
# 26 "../common/secure_channel.h"
enum SecureChannelAuthState {
    SECURE_CHANNEL_INITIAL,
    SECURE_CHANNEL_ID_SENT,
    SECURE_CHANNEL_ESTABLISHED
};







typedef struct {
    const char* CLIENT_PUBLIC_MODULUS;
    const char* CLIENT_PUBLIC_KEY;
    const char* CLIENT_PRIVATE_KEY;
    const char* SERVER_PUBLIC_MODULUS;
    const char* SERVER_PUBLIC_KEY;
} secure_channel_auth_info_t;




typedef struct {

    channel_interface_t interface;


    enum SecureChannelAuthState auth_state;


    rsa_context rsa_ctx;


    aes_context aes_encrypt_ctx;


    aes_context aes_decrypt_ctx;


    uint8_t challenge[16];


    uint8_t auth_send_buffer[2 +
                                                 256 * 2 +
                                                 16];


    uint8_t* auth_send_data;


    secure_channel_auth_info_t auth_info;
} secure_channel_state_t;



void secure_channel_init(secure_channel_state_t* state, channel_interface_t* interface,
                            secure_channel_auth_info_t* auth_info);


void secure_channel_reset(secure_channel_state_t* state);
void secure_channel_input_packet(secure_channel_state_t* state, void* data, size_t size);
void secure_channel_poll(secure_channel_state_t* state);
int secure_channel_send(secure_channel_state_t* state, void* data, size_t size);
void secure_channel_close(secure_channel_state_t* state);

size_t secure_channel_header_size();
# 9 "../common/app.c" 2

# 1 "/usr/include/string.h" 1 3 4
# 29 "/usr/include/string.h" 3 4





# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 1 3 4
# 35 "/usr/include/string.h" 2 3 4









extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 95 "/usr/include/string.h" 3 4
extern void *memchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 126 "/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));






# 1 "/usr/include/xlocale.h" 1 3 4
# 28 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;


typedef __locale_t locale_t;
# 163 "/usr/include/string.h" 2 3 4


extern int strcoll_l (__const char *__s1, __const char *__s2, __locale_t __l)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, __const char *__src, size_t __n,
    __locale_t __l) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));





extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (__const char *__string, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 210 "/usr/include/string.h" 3 4

# 235 "/usr/include/string.h" 3 4
extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 262 "/usr/include/string.h" 3 4
extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 281 "/usr/include/string.h" 3 4



extern size_t strcspn (__const char *__s, __const char *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 314 "/usr/include/string.h" 3 4
extern char *strpbrk (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 342 "/usr/include/string.h" 3 4
extern char *strstr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 397 "/usr/include/string.h" 3 4


extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strnlen (__const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

# 427 "/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
# 445 "/usr/include/string.h" 3 4
extern char *strerror_l (int __errnum, __locale_t __l) __attribute__ ((__nothrow__));





extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (__const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 489 "/usr/include/string.h" 3 4
extern char *index (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 517 "/usr/include/string.h" 3 4
extern char *rindex (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern int ffs (int __i) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 536 "/usr/include/string.h" 3 4
extern int strcasecmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 559 "/usr/include/string.h" 3 4
extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) __attribute__ ((__nothrow__));


extern char *__stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
# 646 "/usr/include/string.h" 3 4

# 11 "../common/app.c" 2
# 1 "/usr/include/stdlib.h" 1 3 4
# 33 "/usr/include/stdlib.h" 3 4
# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 1 3 4
# 34 "/usr/include/stdlib.h" 2 3 4








# 1 "/usr/include/bits/waitflags.h" 1 3 4
# 43 "/usr/include/stdlib.h" 2 3 4
# 1 "/usr/include/bits/waitstatus.h" 1 3 4
# 65 "/usr/include/bits/waitstatus.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 37 "/usr/include/endian.h" 3 4
# 1 "/usr/include/bits/endian.h" 1 3 4
# 38 "/usr/include/endian.h" 2 3 4
# 61 "/usr/include/endian.h" 3 4
# 1 "/usr/include/bits/byteswap.h" 1 3 4
# 28 "/usr/include/bits/byteswap.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/bits/byteswap.h" 2 3 4
# 62 "/usr/include/endian.h" 2 3 4
# 66 "/usr/include/bits/waitstatus.h" 2 3 4

union wait
  {
    int w_status;
    struct
      {

 unsigned int __w_termsig:7;
 unsigned int __w_coredump:1;
 unsigned int __w_retcode:8;
 unsigned int:16;







      } __wait_terminated;
    struct
      {

 unsigned int __w_stopval:8;
 unsigned int __w_stopsig:8;
 unsigned int:16;






      } __wait_stopped;
  };
# 44 "/usr/include/stdlib.h" 2 3 4
# 68 "/usr/include/stdlib.h" 3 4
typedef union
  {
    union wait *__uptr;
    int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));
# 96 "/usr/include/stdlib.h" 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;







__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;


# 140 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__)) ;




extern double atof (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





__extension__ extern long long int atoll (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





extern double strtod (__const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern float strtof (__const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern long double strtold (__const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern long int strtol (__const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern unsigned long int strtoul (__const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




__extension__
extern long long int strtoq (__const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtouq (__const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





__extension__
extern long long int strtoll (__const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtoull (__const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

# 311 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__)) ;


extern long int a64l (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;




# 1 "/usr/include/sys/types.h" 1 3 4
# 28 "/usr/include/sys/types.h" 3 4






typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;
# 61 "/usr/include/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;
# 99 "/usr/include/sys/types.h" 3 4
typedef __pid_t pid_t;





typedef __id_t id_t;
# 116 "/usr/include/sys/types.h" 3 4
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 133 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 58 "/usr/include/time.h" 3 4


typedef __clock_t clock_t;



# 74 "/usr/include/time.h" 3 4


typedef __time_t time_t;



# 92 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 104 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 134 "/usr/include/sys/types.h" 2 3 4
# 147 "/usr/include/sys/types.h" 3 4
# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 1 3 4
# 148 "/usr/include/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
# 201 "/usr/include/sys/types.h" 3 4
typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 220 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/sys/select.h" 1 3 4
# 31 "/usr/include/sys/select.h" 3 4
# 1 "/usr/include/bits/select.h" 1 3 4
# 23 "/usr/include/bits/select.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 24 "/usr/include/bits/select.h" 2 3 4
# 32 "/usr/include/sys/select.h" 2 3 4


# 1 "/usr/include/bits/sigset.h" 1 3 4
# 24 "/usr/include/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 35 "/usr/include/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





# 1 "/usr/include/time.h" 1 3 4
# 120 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
# 45 "/usr/include/sys/select.h" 2 3 4

# 1 "/usr/include/bits/time.h" 1 3 4
# 75 "/usr/include/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
# 47 "/usr/include/sys/select.h" 2 3 4


typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
# 67 "/usr/include/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
# 99 "/usr/include/sys/select.h" 3 4

# 109 "/usr/include/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 121 "/usr/include/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);



# 221 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/sysmacros.h" 1 3 4
# 30 "/usr/include/sys/sysmacros.h" 3 4
__extension__
extern unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern unsigned long long int gnu_dev_makedev (unsigned int __major,
            unsigned int __minor)
     __attribute__ ((__nothrow__));
# 224 "/usr/include/sys/types.h" 2 3 4





typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 271 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/bits/pthreadtypes.h" 1 3 4
# 23 "/usr/include/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 24 "/usr/include/bits/pthreadtypes.h" 2 3 4
# 50 "/usr/include/bits/pthreadtypes.h" 3 4
typedef unsigned long int pthread_t;


typedef union
{
  char __size[36];
  long int __align;
} pthread_attr_t;
# 67 "/usr/include/bits/pthreadtypes.h" 3 4
typedef struct __pthread_internal_slist
{
  struct __pthread_internal_slist *__next;
} __pthread_slist_t;





typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;





    int __kind;





    unsigned int __nusers;
    __extension__ union
    {
      int __spins;
      __pthread_slist_t __list;
    };

  } __data;
  char __size[24];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;





typedef union
{
# 170 "/usr/include/bits/pthreadtypes.h" 3 4
  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;


    unsigned char __flags;
    unsigned char __shared;
    unsigned char __pad1;
    unsigned char __pad2;
    int __writer;
  } __data;

  char __size[32];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[20];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 272 "/usr/include/sys/types.h" 2 3 4



# 321 "/usr/include/stdlib.h" 2 3 4






extern long int random (void) __attribute__ ((__nothrow__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern int rand (void) __attribute__ ((__nothrow__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__));




extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__));







extern double drand48 (void) __attribute__ ((__nothrow__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    unsigned long long int __a;
  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));









extern void *malloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;










extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__warn_unused_result__));

extern void free (void *__ptr) __attribute__ ((__nothrow__));




extern void cfree (void *__ptr) __attribute__ ((__nothrow__));



# 1 "/usr/include/alloca.h" 1 3 4
# 25 "/usr/include/alloca.h" 3 4
# 1 "/usr/lib/gcc/i686-linux-gnu/4.4.5/include/stddef.h" 1 3 4
# 26 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__));






# 498 "/usr/include/stdlib.h" 2 3 4





extern void *valloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern void abort (void) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 531 "/usr/include/stdlib.h" 3 4





extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern void exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));
# 554 "/usr/include/stdlib.h" 3 4






extern void _Exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));






extern char *getenv (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern char *__secure_getenv (__const char *__name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern int putenv (char *__string) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (__const char *__name, __const char *__value, int __replace)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int clearenv (void) __attribute__ ((__nothrow__));
# 606 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 620 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 642 "/usr/include/stdlib.h" 3 4
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) ;
# 663 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 712 "/usr/include/stdlib.h" 3 4





extern int system (__const char *__command) ;

# 734 "/usr/include/stdlib.h" 3 4
extern char *realpath (__const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__)) ;






typedef int (*__compar_fn_t) (__const void *, __const void *);
# 752 "/usr/include/stdlib.h" 3 4



extern void *bsearch (__const void *__key, __const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;



extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
# 771 "/usr/include/stdlib.h" 3 4
extern int abs (int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;



__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;







extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;




__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;

# 808 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));







extern int mblen (__const char *__s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int mbtowc (wchar_t *__restrict __pwc,
     __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__)) ;



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__));

extern size_t wcstombs (char *__restrict __s,
   __const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__));








extern int rpmatch (__const char *__response) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 896 "/usr/include/stdlib.h" 3 4
extern int getsubopt (char **__restrict __optionp,
        char *__const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2, 3))) ;
# 948 "/usr/include/stdlib.h" 3 4
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 964 "/usr/include/stdlib.h" 3 4

# 12 "../common/app.c" 2

# 1 "../../external/uip-1.0/uip/timer.h" 1
# 64 "../../external/uip-1.0/uip/timer.h"
# 1 "../../external/uip-1.0/uip/clock.h" 1
# 55 "../../external/uip-1.0/uip/clock.h"
# 1 "./clock-arch.h" 1





typedef uint32_t clock_time_t;
# 56 "../../external/uip-1.0/uip/clock.h" 2
# 64 "../../external/uip-1.0/uip/clock.h"
void clock_init(void);
# 73 "../../external/uip-1.0/uip/clock.h"
clock_time_t clock_time(void);
# 65 "../../external/uip-1.0/uip/timer.h" 2
# 74 "../../external/uip-1.0/uip/timer.h"
struct timer {
  clock_time_t start;
  clock_time_t interval;
};

void timer_set(struct timer *t, clock_time_t interval);
void timer_reset(struct timer *t);
void timer_restart(struct timer *t);
int timer_expired(struct timer *t);
# 14 "../common/app.c" 2
# 1 "../../external/uip-1.0/uip/uip.h" 1
# 56 "../../external/uip-1.0/uip/uip.h"
# 1 "../../external/uip-1.0/uip/uipopt.h" 1
# 70 "../../external/uip-1.0/uip/uipopt.h"
# 1 "../common/uip-conf.h" 1
# 60 "../common/uip-conf.h"
# 1 "/usr/include/inttypes.h" 1 3 4
# 35 "/usr/include/inttypes.h" 3 4
typedef int __gwchar_t;
# 274 "/usr/include/inttypes.h" 3 4

# 288 "/usr/include/inttypes.h" 3 4
typedef struct
  {
    long long int quot;
    long long int rem;
  } imaxdiv_t;





extern intmax_t imaxabs (intmax_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__const__));


extern imaxdiv_t imaxdiv (intmax_t __numer, intmax_t __denom)
      __attribute__ ((__nothrow__)) __attribute__ ((__const__));


extern intmax_t strtoimax (__const char *__restrict __nptr,
      char **__restrict __endptr, int __base) __attribute__ ((__nothrow__));


extern uintmax_t strtoumax (__const char *__restrict __nptr,
       char ** __restrict __endptr, int __base) __attribute__ ((__nothrow__));


extern intmax_t wcstoimax (__const __gwchar_t *__restrict __nptr,
      __gwchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));


extern uintmax_t wcstoumax (__const __gwchar_t *__restrict __nptr,
       __gwchar_t ** __restrict __endptr, int __base)
     __attribute__ ((__nothrow__));
# 442 "/usr/include/inttypes.h" 3 4

# 61 "../common/uip-conf.h" 2
# 69 "../common/uip-conf.h"
typedef uint8_t u8_t;
# 78 "../common/uip-conf.h"
typedef uint16_t u16_t;
# 88 "../common/uip-conf.h"
typedef unsigned short uip_stats_t;
# 150 "../common/uip-conf.h"
typedef int uip_tcp_appstate_t;
typedef int uip_udp_appstate_t;

void UIP_APPCALL();
void UIP_UDP_APPCALL();
# 71 "../../external/uip-1.0/uip/uipopt.h" 2
# 434 "../../external/uip-1.0/uip/uipopt.h"
void uip_log(char *msg);
# 57 "../../external/uip-1.0/uip/uip.h" 2





typedef u16_t uip_ip4addr_t[2];
typedef u16_t uip_ip6addr_t[8];



typedef uip_ip4addr_t uip_ipaddr_t;
# 188 "../../external/uip-1.0/uip/uip.h"
void uip_init(void);






void uip_setipid(u16_t id);
# 422 "../../external/uip-1.0/uip/uip.h"
extern u8_t uip_buf[1514 +2];
# 450 "../../external/uip-1.0/uip/uip.h"
void uip_listen(u16_t port);
# 464 "../../external/uip-1.0/uip/uip.h"
void uip_unlisten(u16_t port);
# 498 "../../external/uip-1.0/uip/uip.h"
struct uip_conn *uip_connect(uip_ipaddr_t *ripaddr, u16_t port);
# 538 "../../external/uip-1.0/uip/uip.h"
void uip_send(const void *data, int len);
# 766 "../../external/uip-1.0/uip/uip.h"
struct uip_udp_conn *uip_udp_new(uip_ipaddr_t *ripaddr, u16_t rport);
# 1084 "../../external/uip-1.0/uip/uip.h"
u16_t htons(u16_t val);
# 1099 "../../external/uip-1.0/uip/uip.h"
extern void *uip_appdata;
# 1134 "../../external/uip-1.0/uip/uip.h"
extern u16_t uip_len;
# 1153 "../../external/uip-1.0/uip/uip.h"
struct uip_conn {
  uip_ipaddr_t ripaddr;

  u16_t lport;
  u16_t rport;


  u8_t rcv_nxt[4];

  u8_t snd_nxt[4];

  u16_t len;
  u16_t mss;

  u16_t initialmss;

  u8_t sa;

  u8_t sv;

  u8_t rto;
  u8_t tcpstateflags;
  u8_t timer;
  u8_t nrtx;



  uip_tcp_appstate_t appstate;
};
# 1190 "../../external/uip-1.0/uip/uip.h"
extern struct uip_conn *uip_conn;

extern struct uip_conn uip_conns[1];
# 1201 "../../external/uip-1.0/uip/uip.h"
extern u8_t uip_acc32[4];
# 1210 "../../external/uip-1.0/uip/uip.h"
struct uip_udp_conn {
  uip_ipaddr_t ripaddr;
  u16_t lport;
  u16_t rport;
  u8_t ttl;


  uip_udp_appstate_t appstate;
};




extern struct uip_udp_conn *uip_udp_conn;
extern struct uip_udp_conn uip_udp_conns[1];







struct uip_stats {
  struct {
    uip_stats_t drop;

    uip_stats_t recv;

    uip_stats_t sent;

    uip_stats_t vhlerr;

    uip_stats_t hblenerr;

    uip_stats_t lblenerr;

    uip_stats_t fragerr;

    uip_stats_t chkerr;

    uip_stats_t protoerr;

  } ip;
  struct {
    uip_stats_t drop;
    uip_stats_t recv;
    uip_stats_t sent;
    uip_stats_t typeerr;

  } icmp;
  struct {
    uip_stats_t drop;
    uip_stats_t recv;
    uip_stats_t sent;
    uip_stats_t chkerr;

    uip_stats_t ackerr;

    uip_stats_t rst;
    uip_stats_t rexmit;
    uip_stats_t syndrop;

    uip_stats_t synrst;

  } tcp;

  struct {
    uip_stats_t drop;
    uip_stats_t recv;
    uip_stats_t sent;
    uip_stats_t chkerr;

  } udp;

};






extern struct uip_stats uip_stat;
# 1305 "../../external/uip-1.0/uip/uip.h"
extern u8_t uip_flags;
# 1347 "../../external/uip-1.0/uip/uip.h"
void uip_process(u8_t flag);
# 1386 "../../external/uip-1.0/uip/uip.h"
struct uip_tcpip_hdr {
# 1397 "../../external/uip-1.0/uip/uip.h"
  u8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  u16_t ipchksum;
  u16_t srcipaddr[2],
    destipaddr[2];



  u16_t srcport,
    destport;
  u8_t seqno[4],
    ackno[4],
    tcpoffset,
    flags,
    wnd[2];
  u16_t tcpchksum;
  u8_t urgp[2];
  u8_t optdata[4];
};


struct uip_icmpip_hdr {
# 1434 "../../external/uip-1.0/uip/uip.h"
  u8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  u16_t ipchksum;
  u16_t srcipaddr[2],
    destipaddr[2];



  u8_t type, icode;
  u16_t icmpchksum;

  u16_t id, seqno;





};



struct uip_udpip_hdr {
# 1471 "../../external/uip-1.0/uip/uip.h"
  u8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  u16_t ipchksum;
  u16_t srcipaddr[2],
    destipaddr[2];



  u16_t srcport,
    destport;
  u16_t udplen;
  u16_t udpchksum;
};
# 1534 "../../external/uip-1.0/uip/uip.h"
extern uip_ipaddr_t uip_hostaddr, uip_netmask, uip_draddr;







struct uip_eth_addr {
  u8_t addr[6];
};
# 1562 "../../external/uip-1.0/uip/uip.h"
u16_t uip_chksum(u16_t *buf, u16_t len);
# 1573 "../../external/uip-1.0/uip/uip.h"
u16_t uip_ipchksum(void);
# 1584 "../../external/uip-1.0/uip/uip.h"
u16_t uip_tcpchksum(void);
# 1595 "../../external/uip-1.0/uip/uip.h"
u16_t uip_udpchksum(void);
# 15 "../common/app.c" 2
# 1 "../../external/uip-1.0/uip/uip_arp.h" 1
# 55 "../../external/uip-1.0/uip/uip_arp.h"
# 1 "../../external/uip-1.0/uip/uip.h" 1
# 56 "../../external/uip-1.0/uip/uip_arp.h" 2


extern struct uip_eth_addr uip_ethaddr;




struct uip_eth_hdr {
  struct uip_eth_addr dest;
  struct uip_eth_addr src;
  u16_t type;
};
# 76 "../../external/uip-1.0/uip/uip_arp.h"
void uip_arp_init(void);
# 92 "../../external/uip-1.0/uip/uip_arp.h"
void uip_arp_arpin(void);
# 104 "../../external/uip-1.0/uip/uip_arp.h"
void uip_arp_out(void);



void uip_arp_timer(void);
# 16 "../common/app.c" 2

# 1 "../../util/polarssl/polarssl_util.h" 1







void rsa_set_public_modulus(rsa_context* ctx, const char* mod_hex);
void rsa_set_public_key(rsa_context* ctx, const char* pub_hex);
void rsa_set_private_key(rsa_context* ctx, const char* priv_hex);

void rsa_get_public_modulus(rsa_context* ctx, char* mod_hex, int* mod_len);
void rsa_get_public_key(rsa_context* ctx, char* pub_hex, int* pub_len);
void rsa_get_private_key(rsa_context* ctx, char* priv_hex, int* priv_len);

int rsa_gen_keypair(rsa_context* ctx, int nbits);
# 35 "../../util/polarssl/polarssl_util.h"
size_t rsa_get_encryption_size(rsa_context* ctx, size_t input_size);
size_t rsa_get_decryption_size(rsa_context* ctx, size_t input_size);

int rsa_encrypt(rsa_context* ctx, const uint8_t* input, size_t input_size,
                uint8_t* output, size_t* output_size, int mode);

int rsa_decrypt(rsa_context* ctx, const uint8_t* input, size_t input_size,
                uint8_t* output, size_t* output_size, int mode);
# 18 "../common/app.c" 2
# 27 "../common/app.c"
typedef struct {
    uint8_t data[1460];
} buffer_t;







typedef struct {
    buffer_t pkt[4];
    int begin;
    int size;
} ring_t;






void ring_init(ring_t* ring) {
    ring->begin = 0;
    ring->size = 0;
}


int ring_is_empty(ring_t* ring) {
    return ring->size == 0;
}


int ring_is_full(ring_t* ring) {
    return ring->size == 4;
}


buffer_t* ring_next(ring_t* ring) {
    return &(ring->pkt[(ring->begin + ring->size) & (4 - 1)]);
}


void ring_append(ring_t* ring) {
    ring->size++;
}


buffer_t* ring_top(ring_t* ring) {
    return &(ring->pkt[ring->begin]);
}


void ring_pop(ring_t* ring) {
    ring->begin = (ring->begin + 1) & (4 - 1);
    ring->size--;
}




const char* CLIENT_PUBLIC_MODULUS =
    "C3C8A4A90AE206C35E07DEE98648269B5FDA20B9424776403B350923F3EB491C"
    "60C793A818BE6B762EEFECC8E6B0AB9A64D792DFBE4B7A04582823BB64411A36"
    "80DD96D0C0526E3C3DF02E2D8FF575D7E6BDEE654157B31E319C80D59F40ED68"
    "AE8D87AD885E48053EA10DBF8B42800F7B2563A7AA5ED8D1ADBCD00C01713641";

const char* CLIENT_PUBLIC_KEY = "010001";

const char* CLIENT_PRIVATE_KEY =
    "232271B2485AB19E03D4E3C302AF1606921802015D0C980304DD0AABE20D1A4C"
    "B66D7A69132FB0F73F8B1CF21CBC1DBF9253416F57A611DA8FBB7C5617B2BB32"
    "365E2035D972D49D23354F774AF8EC74A32649A893BAA7426957DC452BF6FEB1"
    "1428AFE8555799ED56B2D5A40BD8F0B75EF10C4AEBEC51BDF8EEA2C84AFC3431";



const char* SERVER_PUBLIC_MODULUS =
    "AEEE654C29703F6A6895BECFEB12B8301DE9004E9FC42E52075E26AE8127A44C"
    "69FA3521F67387687605BE269D1CC72B9C7447B40B8779FC6AA5C68CB20A63AD"
    "CFA30A6A9CF2C49C5889CB8071C2910B47EEC5DA543A316270A6670CD7F5D49A"
    "29801B44994A3AE0DFF74A246A63775F1C773EDE0A939AEF2D415DB8347AA939";

const char* SERVER_PUBLIC_KEY = "010001";


void uip_log(char* msg) {
    printf("UIP: %s\n", msg);
}




typedef struct {
    ring_t rx_ring;
    ring_t tx_ring;
    size_t headers_size;
} host_state_t;
# 135 "../common/app.c"
enum TcpConnState {
    TCP_CONN_DISCONNECTED,
    TCP_CONN_CONNECTING,
    TCP_CONN_CONNECTED,
    TCP_CONN_CLOSED
};

typedef struct {
    enum TcpConnState state;
    void* sent_buffer;
    size_t sent_buffer_size;
    int is_sending_in_progress;
    struct timer reconnect_timer;

} connection_state_t;



connection_state_t* conn_state;
pkt_channel_state_t* pkt_channel_state;
secure_channel_state_t* secure_channel_state;
host_state_t* host_state;


void host_input(host_state_t* state, void* data, size_t size) {
    if (usbnet_send(data, size)) {
        if (4 >= 4) printf("DEBUG:   " "USB packet queued for sending" "%s", "\n");
        ring_append(&state->rx_ring);
    } else {
        if (4 >= 1) printf("ERROR:   " "*** Failed to send frame over USB! Probably rings desynchronization!" "%s", "\n");
    }
}

void host_poll(host_state_t* state) {
    size_t len = usbnet_pop_completed_recv();
    if (len > 0) {
        if (len > (1460 - state->headers_size)) {
            if (4 >= 1) printf("ERROR:   " "Received too long ethernet frame from USB!" "%s", "\n");
            secure_channel_close(secure_channel_state);
            return;
        }

        if (4 >= 4) printf("DEBUG:   " "Got new packet from USB. Processing..." "%s", "\n");
        buffer_t* pkt = ring_top(&state->tx_ring);

        secure_channel_send(secure_channel_state,
                            pkt->data + state->headers_size, len);
    }
}


void server_connection_close(connection_state_t* state) {
    if (4 >= 3) printf("INFO:    " "Closing server connection" "%s", "\n");
    (uip_flags = 16);
    timer_set(&state->reconnect_timer, 1000 * 1);

    state->state = TCP_CONN_CLOSED;
}

int server_connection_send(connection_state_t* state, void* data, size_t size) {
    if (state->is_sending_in_progress) {
        return 0;
    } else {
        if (4 >= 4) printf("DEBUG:   " "Sending packet to UIP of length %d" "%s", size, "\n");
        state->sent_buffer = data;
        state->sent_buffer_size = size;
        state->is_sending_in_progress = 1;
        uip_send(data, size);
        return 1;
    }
}

void* get_next_rx_buffer(size_t* size) {
    if (ring_is_full(&host_state->rx_ring)) {
        return ((void *)0);
    } else {
        buffer_t* pkt = ring_next(&host_state->rx_ring);
        *size = sizeof(pkt->data);
        return pkt->data;
    }
}

void do_nothing(void* data) {
}

void UIP_UDP_APPCALL() {}

void UIP_APPCALL() {


    if ((uip_flags & 64)) {

        if (4 >= 4) printf("DEBUG:   " "TCP connection established" "%s", "\n");

        conn_state->state = TCP_CONN_CONNECTED;
        conn_state->is_sending_in_progress = 0;
        conn_state->sent_buffer = ((void *)0);
        conn_state->sent_buffer_size = 0;

        pkt_channel_reset(pkt_channel_state);
        secure_channel_reset(secure_channel_state);
    }


    if ((uip_flags & 2) && uip_len > 0) {
        if (4 >= 4) printf("DEBUG:   " "New packet arrived from UIP" "%s", "\n");

        pkt_channel_input_packet(pkt_channel_state, uip_appdata, uip_len);
    }



    if ((uip_flags & 8) && !conn_state->is_sending_in_progress) {
        pkt_channel_poll(pkt_channel_state);
    }


    if ((uip_flags & 1)) {

        if (4 >= 4) printf("DEBUG:   " "Sent packet acked" "%s", "\n");
        if (conn_state->is_sending_in_progress &&
            conn_state->sent_buffer == ring_top(&host_state->tx_ring)->data)
        {
            ring_pop(&host_state->tx_ring);
        }
        conn_state->is_sending_in_progress = 0;
    }


    if ((uip_flags & 4)) {

        if (4 >= 4) printf("DEBUG:   " "Retransmit requested" "%s", "\n");
        uip_send(conn_state->sent_buffer, conn_state->sent_buffer_size);
    }


    if ((uip_flags & 16)) {
        if (4 >= 4) printf("DEBUG:   " "Connection closed!" "%s", "\n");
        server_connection_close(conn_state);
    }


    if ((uip_flags & 32)) {
        if (4 >= 4) printf("DEBUG:   " "Connection aborted!" "%s", "\n");
        server_connection_close(conn_state);
    }
}

void app_loop() {





    connection_state_t conn_state_instance = {
        TCP_CONN_DISCONNECTED,
        ((void *)0),
        0,
        0
    };
    pkt_channel_state_t pkt_channel_state_instance;
    secure_channel_state_t secure_channel_state_instance;
    host_state_t host_state_instance;

    conn_state = &conn_state_instance;
    pkt_channel_state = &pkt_channel_state_instance;
    secure_channel_state = &secure_channel_state_instance;
    host_state = &host_state_instance;

    channel_interface_t pkt_channel_interface = {
        secure_channel_state,
        (channel_input_function_t)secure_channel_input_packet,
        (channel_poll_function_t)secure_channel_poll,
        0,
        conn_state,
        (channel_send_function_t)server_connection_send,
        (channel_close_function_t)server_connection_close
    };
    pkt_channel_init(pkt_channel_state, &pkt_channel_interface, get_next_rx_buffer, do_nothing);

    channel_interface_t secure_channel_interface = {
        host_state,
        (channel_input_function_t)host_input,
        (channel_poll_function_t)host_poll,
        pkt_channel_header_size(),
        pkt_channel_state,
        (channel_send_function_t)pkt_channel_send,
        (channel_close_function_t)pkt_channel_close
    };
    secure_channel_auth_info_t secure_channel_auth_info = {
        CLIENT_PUBLIC_MODULUS,
        CLIENT_PUBLIC_KEY,
        CLIENT_PRIVATE_KEY,
        SERVER_PUBLIC_MODULUS,
        SERVER_PUBLIC_KEY
    };
    secure_channel_init(secure_channel_state, &secure_channel_interface, &secure_channel_auth_info);

    ring_init(&host_state->rx_ring);
    ring_init(&host_state->tx_ring);
    host_state->headers_size = secure_channel_header_size() + pkt_channel_header_size();


    struct uip_eth_addr mac_addr = {{20, 21, 22, 23, 24, 25}};
    uip_ipaddr_t addr;

    uip_init();

    do {uip_ethaddr.addr[0] = mac_addr.addr[0]; uip_ethaddr.addr[1] = mac_addr.addr[1]; uip_ethaddr.addr[2] = mac_addr.addr[2]; uip_ethaddr.addr[3] = mac_addr.addr[3]; uip_ethaddr.addr[4] = mac_addr.addr[4]; uip_ethaddr.addr[5] = mac_addr.addr[5];} while(0);


    do { ((u16_t *)(&addr))[0] = (((192) << 8) | (168)); ((u16_t *)(&addr))[1] = (((1) << 8) | (2)); } while(0);
    do { ((u16_t *)uip_hostaddr)[0] = ((u16_t *)(&addr))[0]; ((u16_t *)uip_hostaddr)[1] = ((u16_t *)(&addr))[1]; } while(0);

    do { ((u16_t *)(&addr))[0] = ((u16_t *)uip_hostaddr)[0]; ((u16_t *)(&addr))[1] = ((u16_t *)uip_hostaddr)[1]; } while(0);

    printf("HTONS(23) = %d\n", (23));


    do { ((u16_t *)(&addr))[0] = (((255) << 8) | (255)); ((u16_t *)(&addr))[1] = (((255) << 8) | (0)); } while(0);
    do { ((u16_t *)uip_netmask)[0] = ((u16_t *)(&addr))[0]; ((u16_t *)uip_netmask)[1] = ((u16_t *)(&addr))[1]; } while(0);


    do { ((u16_t *)(&addr))[0] = (((192) << 8) | (168)); ((u16_t *)(&addr))[1] = (((1) << 8) | (1)); } while(0);

    struct uip_conn* conn = ((void *)0);

    struct timer periodic_uip_timer;

    timer_set(&periodic_uip_timer, 1000 / 5);

    for(;;) {


        uip_len = enc28j60_packet_receive(sizeof(uip_buf), uip_buf);
        if (uip_len > 0) {
            struct uip_eth_hdr* eth_header = (struct uip_eth_hdr*)uip_buf;
            if (eth_header->type == (0x0800)) {
                ;
                uip_process(1);
                if (uip_len > 0) {
                    uip_arp_out();
                    enc28j60_packet_send(uip_len, uip_buf);
                }
            } else if (eth_header->type == (0x0806)) {
                uip_arp_arpin();
                if (uip_len > 0) {
                    enc28j60_packet_send(uip_len, uip_buf);
                }
            }
        } else if (conn) {
            do { uip_conn = conn; uip_process(3); } while (0);
            if (uip_len > 0) {
                uip_arp_out();
                enc28j60_packet_send(uip_len, uip_buf);
            }
        }

        if (conn_state->state == TCP_CONN_CLOSED) {
            if (timer_expired(&conn_state->reconnect_timer)) {
                if (4 >= 4) printf("DEBUG:   " "Timed out. Now will try to reconnect." "%s", "\n");
                conn_state->state = TCP_CONN_DISCONNECTED;
            }
        } else if (conn_state->state == TCP_CONN_DISCONNECTED) {
            if (4 >= 4) printf("DEBUG:   " "Trying to connect..." "%s", "\n");
            conn = uip_connect(&addr, (7777));
            if (!conn) {
                if (4 >= 4) printf("DEBUG:   " "Failed to create UIP connection" "%s", "\n");
                conn_state->state = TCP_CONN_CLOSED;
                timer_restart(&conn_state->reconnect_timer);
            } else {
                conn_state->state = TCP_CONN_CONNECTING;
            }
        }

        if (conn && timer_expired(&periodic_uip_timer)) {
            timer_reset(&periodic_uip_timer);
            do { uip_conn = conn; uip_process(2); } while (0);
            if (uip_len > 0) {
                uip_arp_out();
                enc28j60_packet_send(uip_len, uip_buf);
            }
        }


        while (!ring_is_full(&host_state->tx_ring)) {
            buffer_t* pkt = ring_next(&host_state->tx_ring);

            if (usbnet_recv(pkt->data + host_state->headers_size,
                            sizeof(pkt->data) - host_state->headers_size))
            {
                ring_append(&host_state->tx_ring);
            } else {
                if (4 >= 1) printf("ERROR:   " "*** Posting tx buffer to USB failed! Probably rings desynchronization!" "%s", "\n");
            }
        }


        while (usbnet_pop_completed_send()) {
            if (4 >= 4) printf("DEBUG:   " "Sending full frame to USB done" "%s", "\n");
            ring_pop(&host_state->rx_ring);
        }

        sim_sleep(1);
    }
}
