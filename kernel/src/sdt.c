#include "sdt.h"
#include "kernel.h"
#include "log.h"

struct __attribute__((packed)) rsdp1 {
  char signature[8];
  uint8_t checksum;
  char oem_id[6];
  uint8_t revision;
  uint32_t rsdt_address;
};

struct __attribute__((packed)) rsdp2 {
  uint32_t length;
  uint64_t xsdt_address;
  uint8_t extended_checksum;
  uint8_t reserved[3];
};

typedef struct __attribute__((packed)) rspd {
  struct rsdp1 rsdp1;
  struct rsdp2 rsdp2;
} rsdp_t;

struct __attribute__((packed)) rsdt {
  sdt_header_t header;
  uint32_t sdt_ptrs[1];
};

struct __attribute__((packed)) xsdt {
  sdt_header_t header;
  uint64_t sdt_ptrs[1];
};

struct {
  union {
    uint32_t *rsdt_ptrs;
    uint64_t *xsdt_ptrs;
  };
  unsigned n_sdts;
  sdt_header_t *(*find)(const char *signature);
} sdt;

//"RSD PTR "
#define RSDP_TOKEN 0x2052545020445352

static void check_sum(char *ptr, size_t size, const char *err_msg) {
  char sum = 0;
  for (size_t i = 0; i < size; ++i) sum += ptr[i];
  if (sum) log_error("sdt", "init", err_msg);
}

static rsdp_t *find_rsdp(uint64_t* ptr, int64_t bytes) {
  for (; bytes > 0; ptr += 2, bytes -= 16) {
    if (ptr[0] == RSDP_TOKEN) return (rsdp_t*)ptr;
  }
  return 0;
}

static sdt_header_t *find_r_sdt(const char *signature) {
  for (unsigned i = 0; i < sdt.n_sdts; ++i) {
    sdt_header_t *header = kernel_p2v(sdt.rsdt_ptrs[i]);
    if (strncmp(header->signature, signature, strlen(signature)) == 0) {
      return header;
    }
  }
  return 0;
}

static sdt_header_t *find_x_sdt(const char *signature) {
  for (unsigned i = 0; i < sdt.n_sdts; ++i) {
    sdt_header_t *header = kernel_p2v(sdt.xsdt_ptrs[i]);
    if (strncmp(header->signature, signature, strlen(signature)) == 0) {
      return header;
    }
  }
  return 0;
}

static void sdt_init() {
  // locate RSDP
  uint16_t *ebda_ptr = (uint16_t *)kernel_p2v(0x40E);
  uint32_t ebda_address = *ebda_ptr * 16;
  rsdp_t *rsdp = find_rsdp(kernel_p2v(ebda_address), 1024);
  if (!rsdp) rsdp = find_rsdp(kernel_p2v(0xE0000), 131072);
  if (!rsdp) log_error("acpi", "init", "could not locate RSDP");

  // check checksums, choose between RSDT and XSDT
  check_sum((char*)&rsdp->rsdp1, sizeof(struct rsdp1), "invalid RSDP v1 checksum");
  if (rsdp->rsdp1.revision) {
    check_sum((char*)&rsdp->rsdp2, sizeof(struct rsdp2), "invalid RSDP v2 checksum");
    struct xsdt *xsdt = kernel_p2v(rsdp->rsdp2.xsdt_address);
    check_sum((char*)&xsdt->header, sizeof(sdt_header_t), "invalid XSDT header");
    sdt.xsdt_ptrs = xsdt->sdt_ptrs;
    sdt.n_sdts = (xsdt->header.length - sizeof(xsdt->header)) / 8;
    sdt.find = find_x_sdt;

  } else {
    struct rsdt *rsdt = kernel_p2v(rsdp->rsdp1.rsdt_address);
    check_sum((char*)&rsdt->header, sizeof(sdt_header_t), "invalid RSDT header");
    sdt.rsdt_ptrs = rsdt->sdt_ptrs;
    sdt.n_sdts = (rsdt->header.length - sizeof(rsdt->header)) / 4;
    sdt.find = find_r_sdt;
  }
}

sdt_header_t *sdt_find(const char *signature) {
  if (!sdt.find) sdt_init();
  return sdt.find(signature);
}
