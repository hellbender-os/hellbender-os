#include <hellbender/dev/ps2.h>
#include <hellbender/io.h>
#include <hellbender/syscall.h>
#include <hellbender/inline.h>
#include <hellbender/dev/input.h>

#include "../src/int_sig.h"
#include <stdio.h>

#define DATA_PORT 0x60
#define STATUS_REGISTER 0x64
#define COMMAND_REGISTER 0x64

#define COMMAND_READ_CONFIG_BYTE  0x20
#define COMMAND_WRITE_CONFIG_BYTE 0x60
#define COMMAND_TEST_CONTROLLER   0xAA
#define COMMAND_TEST_1ST_PORT     0xAB
#define COMMAND_TEST_2ND_PORT     0xA9
#define COMMAND_ENABLE_1ST_PORT   0xAE
#define COMMAND_ENABLE_2ND_PORT   0xA8
#define COMMAND_DISABLE_1ST_PORT  0xAD
#define COMMAND_DISABLE_2ND_PORT  0xA7

#define CONTROLLER_TEST_PASSED 0x55
#define CONTROLLER_TEST_FAILED 0xFC
#define PORT_TEST_PASSED 0x00

#define DEVICE_RESET        0xFF
#define DEVICE_START_SCAN   0xF4
#define DEVICE_STOP_SCAN    0xF5
#define DEVICE_ID_KEYBOARD  0xF2
#define DEVICE_SET_RATE     0xF3

#define DEVICE_TEST_PASSED  0xAA
#define DEVICE_RESET_FAILED 0xF9
#define DEVICE_ACK          0xFA

#define STATUS_OUTPUT_FULL  0x01 // Output buffer status (0 = empty, 1 = full) (must be set before attempting to read data from IO port 0x60)
#define STATUS_INPUT_FULL   0x02 // Input buffer status (0 = empty, 1 = full) (must be clear before attempting to write data to IO port 0x60 or IO port 0x64)
#define STATUS_SYSTEM_FLAG  0x04 // System Flag (Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST))
#define STATUS_COMMAND_DATA 0x08 // Command/data (0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command)
#define STATUS_TIME_OUT     0x40 // Time-out error (0 = no error, 1 = time-out error)
#define STATUS_PARITY_ERROR 0x80 // Parity error (0 = no error, 1 = parity error)

#define CONFIG_1ST_INT_ENABLED 0x01 // First PS/2 port interrupt (1 = enabled, 0 = disabled)
#define CONFIG_2ND_INT_ENABLED 0x02 // Second PS/2 port interrupt (1 = enabled, 0 = disabled, only if 2 PS/2 ports supported)
#define CONFIG_SYSTEM_FLAG 0x04 // System Flag (1 = system passed POST, 0 = your OS shouldn't be running)
#define CONFIG_1ST_CLK_DISABLED 0x10 // First PS/2 port clock (1 = disabled, 0 = enabled)
#define CONFIG_2ND_CLK_DISABLED 0x20 // Second PS/2 port clock (1 = disabled, 0 = enabled, only if 2 PS/2 ports supported)
#define CONFIG_1ST_PORT_TRANSLATE 0x40 // First PS/2 port translation (1 = enabled, 0 = disabled)

static struct ps2 {
  struct port {
    int ok;
    int mode;
    union {
      struct {
        uint8_t escape_next;
        uint8_t break_next;
      } keyboard;
      struct {
      } mouse;
    };
    void (*send)(unsigned);
    void (*process)(struct port *port, int byte);
  } port1, port2;
} ps2;

INLINE void wait() {
  asm volatile ("pause");
}

INLINE int wait_to_read() {
  for (uint32_t i = 100; i && (inb(STATUS_REGISTER) & STATUS_OUTPUT_FULL) == 0; --i) wait();
  return inb(STATUS_REGISTER) & STATUS_OUTPUT_FULL;
}

INLINE void wait_to_send() {
  do wait();
  while (inb(STATUS_REGISTER) & STATUS_INPUT_FULL);
}

INLINE void send_command(unsigned cmd) {
  wait_to_send();
  outb(COMMAND_REGISTER, cmd);
}

INLINE int read_result() {
  if (wait_to_read()) {
    return inb(DATA_PORT);
  } else return -1;
}

INLINE void send_parameter(unsigned data) {
  wait_to_send();
  outb(DATA_PORT, data);
}

INLINE void flush_data() {
  while (wait_to_read()) {
    inb(DATA_PORT);
  }
}

static void send_data_1(unsigned data) {
  wait_to_send();
  outb(DATA_PORT, data);
}

static void send_data_2(unsigned data) {
  send_command(0xD4);
  wait_to_send();
  outb(DATA_PORT, data);
}

static int reset_device(struct port *port) {
  port->send(DEVICE_RESET);
  int reset = read_result();
  if (reset == DEVICE_TEST_PASSED) reset = read_result();
  if (reset != DEVICE_ACK) port->ok = 0;
  flush_data();
  return port->ok;
}

static int start_device(struct port *port) {
  port->send(DEVICE_START_SCAN);
  unsigned enable = read_result();
  if (enable != DEVICE_ACK) {
    port->ok = 0;
  }
  flush_data();
  return port->ok;
}

static uint8_t ps2_key_codes[256] = { 0 }; // TODO

static void process_keyboard(struct port *port, int byte) {
  char msg[128];
  sprintf(msg, "received byte %02x", byte);
  syscall_log("ps2", "keyboard", msg);

  switch (byte) {
  case 0:
    // spurious interrupt?
    break;
  case 0xE0:
    port->keyboard.escape_next = 1;
    break;
  case 0xF0:
    port->keyboard.break_next = 1;
    break;
  case DEVICE_ACK:
    break;
  default:
    if (port->keyboard.escape_next) {
      port->keyboard.escape_next = 0;
      byte |= 0x80;
    }

    input_event_t event;
    if (port->keyboard.break_next) {
      event.keyboard.type = INPUT_EVENT_KEYUP;
      port->keyboard.break_next = 0;
    } else {
      event.keyboard.type = INPUT_EVENT_KEYDOWN;
    }
    event.keyboard.relative_time = 0; // TODO: implement clock.
    event.keyboard.key_code = ps2_key_codes[byte];
    event.keyboard.flags = 0;
    // TODO: push event to input device.
    (void)event;
  }
}

static void process_standard_mouse(struct port *port, int byte) {
  char msg[128];
  sprintf(msg, "received byte %02x", byte);
  syscall_log("ps2", "mouse", msg);

  (void)port;
  (void)byte;
}

static void process_wheel_mouse(struct port *port, int byte) {
  char msg[128];
  sprintf(msg, "received byte %02x", byte);
  syscall_log("ps2", "mouse", msg);

  (void)port;
  (void)byte;
}

static void process_5button_mouse(struct port *port, int byte) {
  char msg[128];
  sprintf(msg, "received byte %02x", byte);
  syscall_log("ps2", "mouse", msg);

  (void)port;
  (void)byte;
}

static void ps2_thread(void *data) {
  syscall_log("ps2", "thread", "waiting for signals.");
  
  (void)data;
  // start waiting for PS2 interrupts
  syscall_set_signal_mask(~0ull);
  syscall_set_irq_signal(32+ 1, CORESRV_SIGNAL_PS2_PORT1); // port1 uses irq 1
  syscall_set_irq_signal(32+12, CORESRV_SIGNAL_PS2_PORT2); // port2 uses irq 12
  const uint64_t signal_mask = 
    (1ull<<CORESRV_SIGNAL_PS2_PORT1) |
    (1ull<<CORESRV_SIGNAL_PS2_PORT2);
  while (1) {
    switch (syscall_wait_signal(signal_mask)) {
    case CORESRV_SIGNAL_PS2_PORT1: 
      ps2.port1.process(&ps2.port1, inb(DATA_PORT));
      break;
    case CORESRV_SIGNAL_PS2_PORT2:
      ps2.port2.process(&ps2.port2, inb(DATA_PORT));
      break;
    }
  }
}

static void set_mouse_rate(struct port *port, int value) {
  port->send(DEVICE_SET_RATE);
  if (read_result() != DEVICE_ACK) { 
    port->ok = 0;
  } else {
    port->send(value);
    if (read_result() != DEVICE_ACK) { 
      port->ok = 0; 
    }
  }
}

static int identify_device(struct port *port, const char *msg) {
  while (port->ok) {
    port->send(DEVICE_ID_KEYBOARD);
    int identify = read_result();
    if (identify != DEVICE_ACK) {
      port->ok = 0;
      break;
    } else {
      int id0 = read_result();
      int id1 = id0 ? read_result() : -1;
      //printf("port1 identity: %x %x\n", id0, id1);
      if (id0 == 0xAB && id1 == 0x83) {
        port->mode = 1;
        port->process = process_keyboard;
        syscall_log("ps2", msg, "keyboard detected.");
        break;
      } else if (id0 == 0x00) {
        if (port->mode < 2) {
          port->mode = 2;
          set_mouse_rate(port, 200);
          set_mouse_rate(port, 100);
          set_mouse_rate(port, 80);
          continue;
        }
        port->process = process_standard_mouse;
        syscall_log("ps2", msg, "standard mouse detected.");
        break;
      } else if (id0 == 0x03) {
        if (port->mode < 3) {
          port->mode = 3;
          set_mouse_rate(port, 200);
          set_mouse_rate(port, 200);
          set_mouse_rate(port, 80);
          continue;
        }
        port->process = process_wheel_mouse;
        syscall_log("ps2", msg, "wheel mouse detected.");
      } else if (id0 == 0x04) {
        port->mode = 4;
        port->process = process_5button_mouse;
        syscall_log("ps2", msg, "5-button mouse detected.");
        break;
      }
    }
  }
  flush_data();
  return port->ok;
}

void ps2_init() {
  // TODO: initialise USB controllers and disable USB Legacy Support.
  // TODO: Determine if the PS/2 Controller Exists.

  // Disable Devices:
  send_command(COMMAND_DISABLE_1ST_PORT);
  send_command(COMMAND_DISABLE_2ND_PORT);

  // Flush The Output Buffer:
  flush_data();

  // Set the Controller Configuration Byte:
  send_command(COMMAND_READ_CONFIG_BYTE);
  int config = read_result();
  send_command(COMMAND_WRITE_CONFIG_BYTE);
  config &= ~(CONFIG_1ST_INT_ENABLED | CONFIG_2ND_INT_ENABLED | CONFIG_1ST_PORT_TRANSLATE);
  send_parameter(config);
  int dual_channel = config & CONFIG_2ND_CLK_DISABLED;

  // Perform Controller Self Test:
  send_command(COMMAND_TEST_CONTROLLER);
  int self_test = read_result();
  if (self_test != CONTROLLER_TEST_PASSED) {
    syscall_log("ps2", "init", "controller test failed.");
    return;
  }

  // Determine If There Are 2 Channels
  if (dual_channel) {
    send_command(COMMAND_ENABLE_2ND_PORT);
    send_command(COMMAND_READ_CONFIG_BYTE);
    int new_config = read_result();
    dual_channel = !(new_config & CONFIG_2ND_CLK_DISABLED);
    if (dual_channel) send_command(COMMAND_DISABLE_2ND_PORT);
  }

  // Perform Interface Tests:
  send_command(COMMAND_TEST_1ST_PORT);
  ps2.port1.ok = read_result() == PORT_TEST_PASSED;
  ps2.port2.ok = 0;
  if (dual_channel) {
    send_command(COMMAND_TEST_2ND_PORT);
    ps2.port2.ok = read_result() == PORT_TEST_PASSED;
  }
  if (!ps2.port1.ok) syscall_log("ps2", "port1", "port test failed.");
  if (!ps2.port2.ok) syscall_log("ps2", "port2", "port test failed.");

  // Enable Devices
  if (ps2.port1.ok) config |= CONFIG_1ST_INT_ENABLED;
  if (ps2.port2.ok) config |= CONFIG_2ND_INT_ENABLED;
  send_command(COMMAND_WRITE_CONFIG_BYTE);
  send_parameter(config); // interrupts first, then devices (otherwise QEmu bugs).
  if (ps2.port1.ok) send_command(COMMAND_ENABLE_1ST_PORT);
  if (ps2.port2.ok) send_command(COMMAND_ENABLE_2ND_PORT);

  // Reset Devices; may cause self-tests to run, thus we'll skip any 0xAA.
  ps2.port1.send = send_data_1;
  ps2.port2.send = send_data_2;
  if (ps2.port1.ok && !reset_device(&ps2.port1)) {
    syscall_log("ps2", "port1", "device reset failed.");
  }
  if (ps2.port2.ok && !reset_device(&ps2.port2)) {
    syscall_log("ps2", "port2", "device reset failed.");
  }

  // Prevent interference with identification.
  if (ps2.port1.ok) send_data_1(DEVICE_STOP_SCAN);
  if (ps2.port2.ok) send_data_2(DEVICE_STOP_SCAN);
  flush_data();

  // Identify devices
  if (ps2.port1.ok && !identify_device(&ps2.port1, "port1")) {
    syscall_log("ps2", "port1", "identify device failed.");
  }
  if (ps2.port2.ok && !identify_device(&ps2.port2, "port2")) {
    syscall_log("ps2", "port2", "identify device failed.");
  }

  // Start scanning
  if (ps2.port1.ok && !start_device(&ps2.port1)) {
    syscall_log("ps2", "port1", "failed to start scanning.");
  }
  if (ps2.port2.ok && !start_device(&ps2.port2)) {
    syscall_log("ps2", "port2", "failed to start scanning.");
  }
  flush_data();

  // Start receiving thread.
  if (ps2.port1.ok || ps2.port2.ok) {
    syscall_create_thread(ps2_thread, 0, 30);
  }
}
