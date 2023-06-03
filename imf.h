#ifndef include_imf_h_
#define include_imf_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)


enum imf_rate_std {
    imf_rate_280 = 280,
    imf_rate_duke_nukem_ii = imf_rate_280,

    imf_rate_560 = 560,
    imf_rate_bio_menace = imf_rate_560,
    imf_rate_commander_keen = imf_rate_560,
    imf_rate_cosmos_cosmic_adventures = imf_rate_560,
    imf_rate_monster_bash = imf_rate_560,
    imf_rate_major_stryker = imf_rate_560,

    imf_rate_700 = 700,
    imf_rate_blake_stone = imf_rate_700,
    imf_rate_operation_body_count = imf_rate_700,
    imf_rate_wolfenstein_3d = imf_rate_700,
    imf_rate_corridor_7 = imf_rate_700,

    imf_rate_default = imf_rate_560
};


struct imf_event {
    uint8_t address_lo;
    uint8_t value;
    uint8_t delay_lo;
    uint8_t delay_hi;
};


struct imf_status {
    const struct imf_event* events;
    uint32_t imf_rate;
    uint32_t opl_rate;
    uint32_t division;
    uint32_t divider;
    uint32_t length;
    uint32_t index;
    uint16_t delay;
    uint8_t type;
    uint8_t address_hi;
};


struct imf_cmd {
    uint16_t address;
    uint8_t value;
    uint8_t delaying;
};


uint8_t imf_guess_type(
    const void* data, 
    size_t size
);

void imf_init(
    struct imf_status* status,
    uint32_t imf_rate, 
    uint32_t opl_rate
);

void imf_load(
    struct imf_status* status, 
    const void* data, 
    size_t size,
    uint8_t type
);

const struct imf_cmd imf_opl_tick(
    struct imf_status* status
);


#pragma pack(pop)

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // include_imf_h_
