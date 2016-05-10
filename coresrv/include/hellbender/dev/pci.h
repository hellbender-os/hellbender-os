#ifndef __CORESRV_DEV_PCI_H__
#define __CORESRV_DEV_PCI_H__

#include "cdev.h"

typedef struct pci_op {
  int (*create)(vfs_node_t *node);
} pci_op_t;

typedef void (*pci_bind_t)(pci_op_t *pci_op);
#define PCI_ID "coresrv::dev::pci"
#define PCI_BIND ((pci_bind_t)BROKER_LOOKUP(PCI_ID))

#endif
