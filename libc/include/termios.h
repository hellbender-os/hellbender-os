#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <sys/types.h> // nonconforming: pid_t.

// The Open Group Base Specifications Issue 7

typedef unsigned cc_t;
typedef unsigned speed_t;
typedef unsigned tcflag_t;

#define NCCS 16

struct  termios {
  tcflag_t  c_iflag; //     Input modes. 
  tcflag_t  c_oflag; //     Output modes. 
  tcflag_t  c_cflag; //     Control modes. 
  tcflag_t  c_lflag; //     Local modes. 
  cc_t      c_cc[NCCS]; //  Control characters. 
};

#define VEOF 1
// EOF character.

#define VEOL 2
// EOL character.

#define VERASE 3
// ERASE character.

#define VINTR 4
// INTR character.

#define VKILL 5
// KILL character.
 
#define VMIN 6
// MIN value.

#define VQUIT 7
// QUIT character.

#define VSTART 8
// START character.

#define VSTOP 9
// STOP character.

#define VSUSP 10
// SUSP character.

#define VTIME 11
// TIME value.

#define BRKINT 01
// Signal interrupt on break.

#define ICRNL 02
// Map CR to NL on input.

#define IGNBRK 04
// Ignore break condition.

#define IGNCR 010
// Ignore CR.

#define IGNPAR 020
// Ignore characters with parity errors.

#define INLCR 040
// Map NL to CR on input.

#define INPCK 0100
// Enable input parity check.

#define ISTRIP 0200
// Strip character.

#define IXANY 0400
// Enable any character to restart output.

#define IXOFF 0100
// Enable start/stop input control.

#define IXON 0200
// Enable start/stop output control.

#define PARMRK 0400
// Mark parity errors.

#define OPOST 0x1
// Post-process output.

#define ONLCR 0x2
// [XSI] [Option Start] Map NL to CR-NL on output. [Option End]

#define OCRNL 0x4
// [XSI] [Option Start] Map CR to NL on output. [Option End]

#define ONOCR 0x8
// [XSI] [Option Start] No CR output at column 0. [Option End]

#define ONLRET 0x10
// [XSI] [Option Start] NL performs CR function. [Option End]

#define OFDEL 0x20
// [XSI] [Option Start] Fill is DEL. [Option End]

#define OFILL 0x40
// [XSI] [Option Start] Use fill characters for delay. [Option End]

#define NLDLY 0x80
// [XSI] [Option Start] Select newline delays:

#define NL0 0x100
// Newline type 0.

#define NL1 0x200
// Newline type 1.

#define CRDLY 0x400
// [XSI] [Option Start] Select carriage-return delays:

#define CR0 0x800
// Carriage-return delay type 0.

#define CR1 0x1000
// Carriage-return delay type 1.

#define CR2 0x2000
// Carriage-return delay type 2.

#define CR3 0x4000
// Carriage-return delay type 3.

#define TABDLY 0x8000
// [XSI] [Option Start] Select horizontal-tab delays:

#define TAB0 0x10000
// Horizontal-tab delay type 0.

#define TAB1 0x20000
// Horizontal-tab delay type 1.

#define TAB2 0x40000
// Horizontal-tab delay type 2.

#define TAB3 0x80000
// Expand tabs to spaces.

#define BSDLY 0x100000
//[XSI] [Option Start] Select backspace delays:

#define BS0 0x200000
// Backspace-delay type 0.

#define BS1 0x400000
// Backspace-delay type 1.

#define VTDLY 0x800000
//[XSI] [Option Start] Select vertical-tab delays:

#define VT0 0x1000000
// Vertical-tab delay type 0.

#define VT1 0x2000000
// Vertical-tab delay type 1.

#define FFDLY 0x4000000
// [XSI] [Option Start] Select form-feed delays:

#define FF0 0x8000000
// Form-feed delay type 0.

#define FF1 0x10000000
// Form-feed delay type 1.

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

#define CSIZE 0x1
// Character size:

#define CS5 0x2
// 5 bits

#define CS6 0x4
// 6 bits

#define CS7 0x8
// 7 bits

#define CS8 0x10
// 8 bits

#define CSTOPB 0x20
// Send two stop bits, else one.

#define CREAD 0x40
// Enable receiver.

#define PARENB 0x80
// Parity enable.

#define PARODD 0x100
// Odd parity, else even.

#define HUPCL 0x200
// Hang up on last close.

#define CLOCAL 0x400
//Ignore modem status lines.

//The implementation shall support the functionality associated with the symbols CS7, CS8, CSTOPB, PARODD, and PARENB.

#define ECHO 0x1
// Enable echo.

#define ECHOE 0x2
// Echo erase character as error-correcting backspace.

#define ECHOK 0x4
// Echo KILL.

#define ECHONL 0x8
// Echo NL.

#define ICANON 0x10
// Canonical input (erase and kill processing).

#define IEXTEN 0x20
// Enable extended input character processing.

#define ISIG 0x40
// Enable signals.

#define NOFLSH 0x80
// Disable flush after interrupt or quit.

#define TOSTOP 0x100
// Send SIGTTOU for background output.

#define TCSANOW 0x1
// Change attributes immediately.

#define TCSADRAIN 0x2
// Change attributes when output has drained.

#define TCSAFLUSH 0x4
// Change attributes when output has drained; also flush pending input.

#define TCIFLUSH 0x1
// Flush pending input.

#define TCIOFLUSH 0x2
// Flush both pending input and untransmitted output.

#define TCOFLUSH 0x4
// Flush untransmitted output.

#define TCIOFF 1
// Transmit a STOP character, intended to suspend input data.

#define TCION 2
// Transmit a START character, intended to restart input data.

#define TCOOFF 3
// Suspend output.

#define TCOON 4
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
