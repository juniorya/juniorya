/**
 * @file xml_loader.h
 * @brief Lightweight parser for importing EtherCAT drive descriptors from KEBA XML files.
 */

#ifndef XML_LOADER_H
#define XML_LOADER_H

#include <stdbool.h>

#include "ethcat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parse a KEBA-style EtherCAT drive description XML file.
 *
 * The parser extracts vendor/product identifiers and populates the drive descriptor list
 * inside @p cfg. Only a subset of the XML schema is required for commissioning, namely the
 * <VendorId>, <ProductCode>, <RevisionNo> and <Name> tags. The function is tolerant to
 * whitespace and both decimal and hexadecimal encodings.
 *
 * @param path Path to XML file.
 * @param cfg Master configuration to populate.
 * @return True when at least one drive was parsed.
 */
bool ethcat_load_drives_from_xml(const char *path, ethcat_master_config *cfg);

#ifdef __cplusplus
}
#endif

#endif
