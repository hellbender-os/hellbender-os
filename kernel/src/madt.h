#ifndef __HELLBENDER_MADT_H__
#define __HELLBENDER_MADT_H__

#include "sdt.h"

struct __attribute__((packed)) madt {
  sdt_header_t h;
  uint32_t LocalCoqntrollerAddress;
  uint32_t Flags; // (1 = Dual 8259 Legacy PICs Installed)
};

struct __attribute__((packed)) madt_header {
  uint8_t EntryType;
  uint8_t RecordLength;
};

#define MADT_ENTRY_TYPE_LAPIC 0 // Processor Local APIC
struct __attribute__((packed)) madt_lapic {
  struct madt_header header;
  uint8_t AcpiProcessorId;
  uint8_t ApicId;
  uint32_t Flags; // (1 = Processor Enabled)
};

#define MADT_ENTRY_TYPE_IOAPIC 1 // This type represents a I/O APIC.
struct __attribute__((packed)) madt_lapic {
  struct madt_header header;
  uint8_t IoApicId;
  uint8_t Reserved;
  uint32_t IoApicAddress;
  uint32_t GlobalSystemInterruptBase;
};

#define MADT_ENTRY_TYPE_ISO 2 // Interrupt Source Override
// This entry type contains the data for an Interrupt Source Override (see ACPI spec.).
struct __attribute__((packed)) madt_lapic {
  struct madt_header header;
  uint8_t BusSource;
  uint8_t IrqSource;
  uint8_t GlobalSystemInterrupt;
  uint16_t Flags;
};

#endif
