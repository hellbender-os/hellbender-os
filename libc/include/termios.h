#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <sys/types.h> // nonconforming: pid_t.

// The Open Group Base Specifications Issue 7

typedef unsigned cc_t;
typedef unsigned speed_t;
typedef unsigned tcflag_t;

#define NCCS 12

struct  termios {
  tcflag_t  c_iflag; //     Input modes. 
  tcflag_t  c_oflag; //     Output modes. 
  tcflag_t  c_cflag; //     Control modes. 
  tcflag_t  c_lflag; //     Local modes. 
  cc_t      c_cc[NCCS]; //  Control characters. 
};

// The <termios.h> header shall define the following symbolic constants for use as subscripts for the array c_cc

#define VEOF 0
// EOF character.

#define VEOL 1
// EOL character.

#define VERASE 2
// ERASE character.

#define VINTR 3
// INTR character.

#define VKILL 4
// KILL character.
 
#define VMIN 5
// MIN value.

#define VQUIT 6
// QUIT character.

#define VSTART 7
// START character.

#define VSTOP 8
// STOP character.

#define VSUSP 9
// SUSP character.

#define VTIME 10
// TIME value.

// The <termios.h> header shall define the following symbolic constants for use as flags in the c_iflag field. The c_iflag field describes the basic terminal input control.

#define BRKINT 0x001
// Signal interrupt on break.

#define ICRNL 0x002
// Map CR to NL on input.

#define IGNBRK 0x004
// Ignore break condition.

#define IGNCR 0x008
// Ignore CR.

#define IGNPAR 0x010
// Ignore characters with parity errors.

#define INLCR 0x020
// Map NL to CR on input.

#define INPCK 0x040
// Enable input parity check.

#define ISTRIP 0x080
// Strip character.

#define IXANY 0x100
// Enable any character to restart output.

#define IXOFF 0x200
// Enable start/stop input control.

#define IXON 0x400
// Enable start/stop output control.

#define PARMRK 0x800
// Mark parity errors.

// The <termios.h> header shall define the following symbolic constants for use as flags in the c_oflag field. The c_oflag field specifies the system treatment of output.

#define OPOST  0x00001
// Post-process output.

#define ONLCR  0x00002
// [XSI] [Option Start] Map NL to CR-NL on output. [Option End]

#define OCRNL  0x00004
// [XSI] [Option Start] Map CR to NL on output. [Option End]

#define ONOCR  0x00008
// [XSI] [Option Start] No CR output at column 0. [Option End]

#define ONLRET 0x00010
// [XSI] [Option Start] NL performs CR function. [Option End]

#define OFDEL  0x00020
// [XSI] [Option Start] Fill is DEL. [Option End]

#define OFILL  0x00040
// [XSI] [Option Start] Use fill characters for delay. [Option End]

#define NLDLY  0x00080
// [XSI] [Option Start] Select newline delays:

# define NL0   0x00000
// Newline type 0.

# define NL1   0x00100
// Newline type 1.

#define CRDLY  0x00200
// [XSI] [Option Start] Select carriage-return delays:

# define CR0   0x00000
// Carriage-return delay type 0.

# define CR1   0x00400
// Carriage-return delay type 1.

# define CR2   0x00800
// Carriage-return delay type 2.

# define CR3   0x00c00
// Carriage-return delay type 3.

#define TABDLY 0x01000
// [XSI] [Option Start] Select horizontal-tab delays:

#define TAB0 0x000000
// Horizontal-tab delay type 0.

#define TAB1 0x002000
// Horizontal-tab delay type 1.

#define TAB2 0x004000
// Horizontal-tab delay type 2.

#define TAB3 0x006000
// Expand tabs to spaces.

#define BSDLY 0x008000
//[XSI] [Option Start] Select backspace delays:

#define BS0 0x000000
// Backspace-delay type 0.

#define BS1 0x010000
// Backspace-delay type 1.

#define VTDLY 0x020000
//[XSI] [Option Start] Select vertical-tab delays:

#define VT0 0x000000
// Vertical-tab delay type 0.

#define VT1 0x040000
// Vertical-tab delay type 1.

#define FFDLY 0x080000
// [XSI] [Option Start] Select form-feed delays:

#define FF0 0x000000
// Form-feed delay type 0.

#define FF1 0x100000
// Form-feed delay type 1.

// The <termios.h> header shall define the following symbolic constants for use as values of objects of type speed_t.

#define B0 0
//Hang up

#define B50 50
// 50 baud

#define B75 75
// 75 baud

#define B110 110
// 110 baud

#define B134 134
// 134.5 baud

#define B150 150
// 150 baud

#define B200 200
// 200 baud

#define B300 300
// 300 baud

#define B600 600
// 600 baud

#define B1200 1200
// 1200 baud

#define B1800 1800
// 1800 baud

#define B2400 2400
// 2400 baud

#define B4800 4800
// 4800 baud

#define B9600 9600
// 9600 baud

#define B19200 19200
// 19200 baud

#define B38400 38400
// 38400 baud

// The <termios.h> header shall define the following symbolic constants for use as flags in the c_cflag field. The c_cflag field describes the hardware control of the terminal:

#define CSIZE  0x0001
// Character size:

# define CS5   0x0000
// 5 bits

# define CS6   0x0002
// 6 bits

# define CS7   0x0004
// 7 bits

# define CS8   0x0006
// 8 bits

#define CSTOPB 0x0010
// Send two stop bits, else one.

#define CREAD  0x0020
// Enable receiver.

#define PARENB 0x0040
// Parity enable.

#define PARODD 0x0080
// Odd parity, else even.

#define HUPCL  0x0100
// Hang up on last close.

#define CLOCAL 0x0200
// Ignore modem status lines.

//The implementation shall support the functionality associated with the symbols CS7, CS8, CSTOPB, PARODD, and PARENB.

// The <termios.h> header shall define the following symbolic constants for use as flags in the c_lflag field. The c_lflag field of the argument structure is used to control various terminal functions.

#define ECHO   0x001
// Enable echo.

#define ECHOE  0x002
// Echo erase character as error-correcting backspace.

#define ECHOK  0x004
// Echo KILL.

#define ECHONL 0x008
// Echo NL.

#define ICANON 0x010
// Canonical input (erase and kill processing).

#define IEXTEN 0x020
// Enable extended input character processing.

#define ISIG   0x040
// Enable signals.

#define NOFLSH 0x080
// Disable flush after interrupt or quit.

#define TOSTOP 0x100
// Send SIGTTOU for background output.

// The <termios.h> header shall define the following symbolic constants for use with tcsetattr():

#define TCSANOW     0x10
// Change attributes immediately.

#define TCSADRAIN   0x20
// Change attributes when output has drained.

#define TCSAFLUSH   0x30
// Change attributes when output has drained; also flush pending input.

// The <termios.h> header shall define the following symbolic constants for use with tcflush():

#define TCIFLUSH   1
// Flush pending input.

#define TCIOFLUSH  2
// Flush both pending input and untransmitted output.

#define TCOFLUSH   3
// Flush untransmitted output.

// The <termios.h> header shall define the following symbolic constants for use with tcflow():

#define TCIOFF  1
// Transmit a STOP character, intended to suspend input data.

#define TCION   2
// Transmit a START character, intended to restart input data.

#define TCOOFF  3
// Suspend output.

#define TCOON   4
// Restart output.

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int     cfsetispeed(struct termios *, speed_t);
int     cfsetospeed(struct termios *, speed_t);
int     tcdrain(int);
int     tcflow(int, int);
int     tcflush(int, int);
int     tcgetattr(int, struct termios *);
pid_t   tcgetsid(int);
int     tcsendbreak(int, int);
int     tcsetattr(int, int, const struct termios *);

#endif
