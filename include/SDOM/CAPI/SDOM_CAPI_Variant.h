#pragma once
// Auto-generated SDOM C API module: Variant

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Provisional ABI view of SDOM::Variant; layout may change.
typedef struct SDOM_Variant {
    uint8_t type;                                   ///< Variant kind tag; mirrors SDOM_VariantType.
    uint8_t reserved[3];                            ///< Reserved bytes; must be zero when passed across the ABI.
    uint32_t flags;                                 ///< Future use bitmask; currently must be zero.
    uint64_t data;                                  ///< Inline payload or handle depending on type/flags; provisional semantics.
} SDOM_Variant;

#ifdef __cplusplus
} // extern "C"
#endif
