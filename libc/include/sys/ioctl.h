#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

// "linux standard", or deprecated stroptd.h in posix
int ioctl(int fildes, int request, ... /* arg */);

#define TIOCGWINSZ 1

struct winsize
{
  unsigned short ws_row;	/* rows, in characters */
  unsigned short ws_col;	/* columns, in characters */
  unsigned short ws_xpixel;	/* horizontal size, pixels */
  unsigned short ws_ypixel;	/* vertical size, pixels */
};

#endif
