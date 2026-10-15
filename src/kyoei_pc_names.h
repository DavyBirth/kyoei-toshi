/* Long display names persist beside the saves; guest fields remain four units. */
#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int kyoei_pc_name_lookup(const uint16_t *token,size_t length,uint16_t output[25]);
int kyoei_pc_name_store(const uint16_t *name,size_t length,uint16_t token[5]);
int kyoei_pc_names_enabled(void);
#ifdef __cplusplus
}
#endif
