#include <hellbender/dev/pci.h>
#include <hellbender/fs/vfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>
#include <hellbender/io.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static struct pci {
  struct pci_op pci_op;
  struct vfs_node_op node_op;
  struct vfs_file_op file_op;
} pci;

struct __attribute__((packed)) pci_header {
  union {
    uint32_t registers[8];
    struct {
      uint16_t vendor_id;
      uint16_t device_id;
      uint16_t command;
      uint16_t status;
      uint8_t revision_id;
      uint8_t prog_if;
      uint8_t sub_class;
      uint8_t base_class;
      uint8_t cache_line_size;
      uint8_t latency_timer;
      uint8_t header_type;
      uint8_t bist;
      union {
        struct {
          uint32_t bar0;
          uint32_t bar1;
          uint8_t primary_bus_number;
          uint8_t secondary_bus_number;
          uint8_t subordinate_bus_number;
          uint8_t secondary_latency_timer;
        } header1;
      };
    };
  };
};

struct pci_instance {
  dev_t dev;
  ino_t next_ino;
  struct pci_entry {
    struct pci_entry *children;
    struct pci_entry *sibling;
    ino_t ino;
    char name[8];
    mode_t mode; // directory or file
    struct pci_header header;
  } root;
};

struct pci_impl {
  struct pci_instance *instance;
  struct pci_entry *node_entry;
  struct vfs_node *file_node;
};

#define IMPL(n) ((struct pci_impl*)&((n)->impl))

/*static void devfs_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  }*/

int pci_open(vfs_node_t *this, int flags, vfs_file_t *file) {
  (void)flags;
  file->op = pci.file_op;
  file->impl = this->impl;
  IMPL(file)->file_node = this;
  return 0;
}

int pci_query(vfs_node_t *this, int iface_id, void **iface) {
  (void)this;
  (void)iface_id;
  (void)iface;
  return ENOSYS;
}

static int pci_read(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  // TODO if bus, dev, fun set: read parameter block.
  (void)this;
  (void)buf;
  (void)nbytes_;
  (void)offset_;
  return -1;
}

static int pci_write(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  // TODO if bus, dev, fun set: write parameter block.
  (void)this;
  (void)buf;
  (void)nbytes_;
  (void)offset_;
  return -1;
}

static int pci_close(vfs_file_t *this) {
  (void)this;
  return 0;
}

int pci_enqueue(vfs_file_t *this, vfs_io_t *io) {
  int errno;
  switch (io->opcode) {
  case VFS_OPCODE_CLOSE: 
    errno = pci_close(this);
    break;
  case VFS_OPCODE_FLUSH: 
    return 0;
  case VFS_OPCODE_READ: 
    errno = pci_read(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_WRITE:
    errno = pci_write(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_READDIR: return ENOSYS;
  case VFS_OPCODE_MKNOD: return ENOSYS;
  case VFS_OPCODE_UNLINK: return ENOSYS;
  case VFS_OPCODE_FIND: return ENOSYS;
  default: return EINVAL;
  }
  if (!errno) io->onsuccess(io); else io->onfailure(io, errno);
  return 0;
}

int pci_cancel(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  (void)io;
  return VFS_CANCEL_ALLDONE;
}

int pci_create(vfs_node_t *node) {
  struct pci_instance *instance = calloc(1, sizeof(*instance));
  node->op = pci.node_op;
  IMPL(node)->instance = instance;
  IMPL(node)->node_entry = &instance->root;
  return 0;
}

void pci_bind(pci_op_t *pci_op) {
  *pci_op = pci.pci_op;
}

static void read_header(uint32_t bus, uint32_t device, uint32_t function,
                        struct pci_header *header) {
  uint32_t address = (bus << 16) | (device << 11) | (function << 8) | (uint32_t)0x80000000;
  for (uint32_t reg = 0; reg < 8; ++reg) {
    outl(0xCF8, address + 4*reg);
    header->registers[reg] = inl(0xCFC);
  }
}

static void addEntry(uint32_t bus, uint32_t device, uint32_t function,
                     struct pci_header *header) {
  char message[128];
  sprintf(message, "bus=%02x slot=%02x function=%02x class=%02x subclass=%02x iface=%02x vendor=%04x device=%04x",
          bus, device, function, header->base_class, header->sub_class, header->prog_if, header->vendor_id, header->device_id);
  syscall_log("coresrc", "pci", message);
}

static void checkBus(uint32_t bus);

static void checkDevice(uint32_t bus, uint32_t device) {
  struct pci_header header;
  for (uint32_t function = 0; function < 8; function++) {
    read_header(bus, device, function, &header);
    if (header.vendor_id != 0xFFFF) {
      if ((header.base_class == 0x06) && (header.sub_class == 0x04)) {
        checkBus(header.header1.secondary_bus_number);
      }
      addEntry(bus, device, function, &header);
    } else if (function == 0) break;
    if (function == 0 && (header.header_type & 0x80) == 0) break;
  }
}

static void checkBus(uint32_t bus) {
  for (uint32_t device = 0; device < 32; device++) {
    checkDevice(bus, device);
  }
}

static void checkAllBuses() {
  struct pci_header header;
  for (uint32_t function = 0; function < 8; function++) {
    read_header(0, 0, function, &header);
    if (header.vendor_id == 0xFFFF) break;
    checkBus(function);
    if (function == 0 && (header.header_type & 0x80) == 0) break;
  }
}

void pci_init() {
  BROKER_REGISTER(PCI_ID, REGISTER_SERVICE(pci_bind));
  pci.pci_op.create = REGISTER_SERVICE(pci_create);
  pci.node_op.open      = REGISTER_SERVICE(pci_open);
  pci.node_op.query     = REGISTER_SERVICE(pci_query);
  pci.file_op.enqueue   = REGISTER_SERVICE(pci_enqueue);
  pci.file_op.cancel    = REGISTER_SERVICE(pci_cancel);

  checkAllBuses();
}
