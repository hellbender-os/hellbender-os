#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <kernel/io.h>
#include <kernel/kernel.h>
#include <coresrv/ps2.h>

#define DATA_PORT 0x60
#define STATUS_REGISTER 0x64
#define COMMAND_REGISTER 0x64

ps2_t ps2;

static int wait_till_output_set() {
  iowait();
  for (uint32_t i = 100; i && (inb(STATUS_REGISTER) & 1) == 0; --i) iowait();
  return inb(STATUS_REGISTER) & 1;
}

static void wait_till_input_clear() {
  do iowait();
  while (inb(STATUS_REGISTER) & 2);
}

static void send_command(unsigned cmd) {
  wait_till_input_clear();
  outb(COMMAND_REGISTER, cmd);
}

static int get_result() {
  if (wait_till_output_set()) {
    return inb(DATA_PORT);
  } else return -1;
}
/*
static int get_result2() {
  int result = get_result();
  while (result == 0xaa) result = get_result();
  return result;
}
*/
static void send_data(unsigned data) {
  wait_till_input_clear();
  outb(DATA_PORT, data);
}
/*
static void send_data2(unsigned data) {
  send_command(0xD4);
  wait_till_input_clear();
  outb(DATA_PORT, data);
}
*/
static void flush_data() {
  iowait();
  while (inb(STATUS_REGISTER) & 1) {
    inb(DATA_PORT);
    iowait();
  }
}

void ps2_init() {
  memset(&ps2, 0, sizeof(ps2));
         
  // TODO: initialise USB controllers and disable USB Legacy Support.
  // TODO: Determine if the PS/2 Controller Exists.

  // Disable Devices:
  send_command(0xAD);
  //send_command(0xA7);

  // Flush The Output Buffer:
  flush_data();

  // Set the Controller Configuration Byte:
  send_command(0x20);
  int config = get_result();
  send_command(0x60);
  config &= ~(/*1+2+*/64);
  send_data(config);
  /*int dual_channel = config & 32;

  // Perform Controller Self Test:
  send_command(0xAA);
  int self_test = get_result();
  if (self_test != 0x55) {
    printf("PS/2 controller self test failed.\n");
    abort();
  }

  // Determine If There Are 2 Channels
  if (dual_channel) {
    send_command(0xA8);
    send_command(0x20);
    int new_config = get_result();
    dual_channel = !(new_config & 32);
    send_command(0xA7);
  }

  // Perform Interface Tests:
  send_command(0xAB);
  int chan1_ok = get_result() == 0;
  int chan2_ok = 0;
  if (dual_channel) {
    send_command(0xA9);
    chan2_ok = get_result() == 0;
  }

  // Enable Devices
  if (chan1_ok) {*/
    send_command(0xAE);
    /*    config |= 1;
  }
  if (chan2_ok) {
    send_command(0xA8);
    config |= 2;
  }
  send_command(0x60);
  send_data(config);

  // Reset Devices; may cause self-tests to run, thus we'll skip any 0xAA.
  if (chan1_ok) {
    send_data(0xFF);
    int reset = get_result2();
    if (reset != 0xFA) {
      chan1_ok = 0;
      //printf("chan1 reset failed: %x\n", reset);
    }
    flush_data();
  }
  if (chan2_ok) {
    send_data2(0xFF);
    int reset = get_result2();
    if (reset != 0xFA) {
      chan2_ok = 0;
      //printf("chan2 reset failed: %x\n", reset);
    }
    flush_data();
  }

  // Identify devices
  if (chan1_ok) {
    send_data(0xF5);
    int disable = get_result();
    if (disable != 0xFA) {
      chan1_ok = 0;
      //printf("chan1 disable failed: %x\n", disable);
    } else {
      send_data(0xF2);
      int identify = get_result();
      if (identify != 0xFA) {
        chan1_ok = 0;
        //printf("chan1 identify failed: %x\n", identify);
      } else {
        int id0 = get_result();
        int id1 = get_result();
        //printf("chan1 identity: %x %x\n", id0, id1);
        if (id0 == 0xAB && id1 == 0x83) {*/
          printf("Keyboard detected on PS/2 channel 1.\n");
          ps2.chan1_mode = 1;
          /*        } else if (id0 == 0x00) {
          printf("Mouse detected on PS/2 channel 1.\n");
          ps2.chan1_mode = 2;
        } else if (id0 == 0x03) {
          printf("Wheel mouse detected on PS/2 channel 1.\n");
          ps2.chan1_mode = 3;
        } else if (id0 == 0x04) {
          printf("5-button mouse detected on PS/2 channel 1.\n");
          ps2.chan1_mode = 4;
        }
      }
    }
  }
  if (chan2_ok) {
    send_data2(0xF5);
    unsigned disable = get_result();
    if (disable == 0xAA) disable = get_result();
    if (disable != 0xFA) {
      chan2_ok = 0;
      printf("chan2 disable failed: %x\n", disable);
    } else {
      send_data2(0xF2);
      unsigned identify = get_result();
      if (identify != 0xFA) {
        chan2_ok = 0;
        printf("chan2 identify failed: %x\n", identify);
      } else {
        unsigned id0 = get_result();
        unsigned id1 = get_result();
        //printf("chan2 identity: %x %x\n", id0, id1);
        if (id0 == 0xAB && id1 == 0x83) {
          printf("Keyboard detected on PS/2 channel 2.\n");
          ps2.chan2_mode = 1;
        } else if (id0 == 0x00) {
          printf("Mouse detected on PS/2 channel 2.\n");
          ps2.chan2_mode = 2;
        } else if (id0 == 0x03) {
          printf("Wheel mouse detected on PS/2 channel 2.\n");
          ps2.chan2_mode = 3;
        } else if (id0 == 0x04) {
          printf("5-button mouse detected on PS/2 channel 2.\n");
          ps2.chan2_mode = 4;
        }
      }
    }
  }

  // Start scanning
  if (chan1_ok) {
    send_data(0xF4);
    unsigned enable = get_result();
    if (enable != 0xFA) {
      printf("chan1 failed to start scanning.\n");
    }
  }
  if (chan2_ok) {
    send_data2(0xF4);
    unsigned enable = get_result();
    if (enable != 0xFA) {
      printf("chan2 failed to start scanning.\n");
    }
  }
  */

  flush_data();
}
