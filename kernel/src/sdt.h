#ifndef __HELLBENDER_SDT_H__
#define __HELLBENDER_SDT_H__

#include <stdint.h>

#define SDT_MADT "APIC" // Multiple APIC Description Table (MADT)
#define SDT_BERT "BERT" // Boot Error Record Table (BERT)
#define SDT_CPEP "CPEP" // Corrected Platform Error Polling Table (CPEP)
#define SDT_DSDT "DSDT" // Differentiated System Description Table (DSDT)
#define SDT_ECDT "ECDT" // Embedded Controller Boot Resources Table (ECDT)
#define SDT_EINJ "EINJ" // Error Injection Table (EINJ)
#define SDT_ERST "ERST" // Error Record Serialization Table (ERST)
#define SDT_FADT "FACP" // Fixed ACPI Description Table (FADT)
#define SDT_FACS "FACS" // Firmware ACPPI Control Structure (FACS)
#define SDT_HEST "HEST" // Hardware Error Source Table (HEST)
#define SDT_MSCT "MSCT" // Maximum System Characteristics Table (MSCT)
#define SDT_MPST "MPST" // Memory Power State Table (MPST)
#define SDT_OEM  "OEM"  // OEM Specific Information Tables
#define SDT_PMTT "PMTT" // Platform Memory Topology Table (PMTT)
#define SDT_PSDT "PSDT" // Persistent System Description Table (PSDT)
#define SDT_RASF "RASF" // ACPI RAS FeatureTable (RASF)
#define SDT_RSDT "RSDT" // Root System Description Table
#define SDT_SBST "SBST" // Smart Battery Specification Table (SBST)
#define SDT_SLIT "SLIT" // System Locality System Information Table (SLIT)
#define SDT_SRAT "SRAT" // System Resource Affinity Table (SRAT)
#define SDT_SSDT "SSDT" // Secondary System Description Table (SSDT)
#define SDT_XSDT "XSDT" // Extended System Description Table (XSDT)

typedef struct __attribute__((packed)) sdt_header {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oem_id[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} sdt_header_t;

sdt_header_t *acpi_find_sdt(const char *signature);

#endif
