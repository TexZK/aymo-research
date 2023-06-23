#ifndef include_regdump_h_
#define include_regdump_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)


struct regdump_event {
    uint8_t address_hi;
    uint8_t address_lo;
    uint8_t value;
};


struct regdump_status {
    const struct regdump_event* events;
    uint32_t length;
    uint32_t index;
    uint32_t delay;
};


struct regdump_cmd {
    uint16_t address;
    uint8_t value;
    uint8_t delaying;
};


void regdump_init(
    struct regdump_status* status
);

void regdump_load(
    struct regdump_status* status, 
    const void* data, 
    size_t size
);

void regdump_restart(
    struct regdump_status* status
);

const struct regdump_cmd regdump_opl_tick(
    struct regdump_status* status
);


#pragma pack(pop)

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // include_regdump_h_
