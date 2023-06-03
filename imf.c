#include "imf.h"

#include <assert.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


// See: https://moddingwiki.shikadi.net/wiki/IMF_Format
uint8_t imf_guess_type(
    const void* data,
    size_t size
)
{
    const uint8_t* ptr = (const uint8_t *)data;
    uint16_t word;
    uint16_t i = 42;
    uint32_t sum1 = 0;
    uint32_t sum2 = 0;
    assert(data);

    if (size < 2) {
        return 0;
    }

    word = (ptr[0] | ((uint16_t)ptr[1] << 8));
    ptr += 2;
    if (!word || (word & 3)) {
        return 0;
    }

    while ((size >= 4) && i--)
    {
        word = (ptr[0] | ((uint16_t)ptr[1] << 8));
        ptr += 2;
        sum1 += word;

        word = (ptr[0] | ((uint16_t)ptr[1] << 8));
        ptr += 2;
        sum2 += word;
    }
    return (uint8_t)(sum1 > sum2);
}


void imf_init(
    struct imf_status* status, 
    uint32_t imf_rate, 
    uint32_t opl_rate
)
{
    uint32_t division;
    assert(status);
    assert(opl_rate);
    assert(imf_rate);

    division = (opl_rate / imf_rate);  // TODO: improve resolution via fixed point 24.8
    assert(division);

    status->events = NULL;
    status->imf_rate = imf_rate;
    status->opl_rate = opl_rate;
    status->division = division;
    status->divider = 0;
    status->length = 0;
    status->index = 0;
    status->delay = 0;
    status->type = 0;
    status->address_hi = 0;
}


void imf_load(
    struct imf_status* status,
    const void* data,
    size_t size,
    uint8_t type
)
{
    uint8_t* ptr = (uint8_t*)data;
    uint32_t length_by_header;
    uint32_t length_by_size;
    assert(status);

    status->events = NULL;
    status->divider = 0;
    status->length = 0;
    status->index = 0;
    status->delay = 0;
    status->type = 0;
    status->address_hi = 0;

    if (type) {
        length_by_header = (ptr[0] | ((uint16_t)ptr[1] << 8));
        length_by_header /= sizeof(struct imf_event);
        status->length = length_by_header;
        status->events = (const struct imf_event*)(const void*)&ptr[2];

        length_by_size = (uint32_t)(size - 2);
        length_by_size /= sizeof(struct imf_event);
        if (status->length > length_by_size) {
            status->length = length_by_size;
        }
    }
    else {
        length_by_size = (uint32_t)size;
        length_by_size /= sizeof(struct imf_event);
        status->length = length_by_size;
        status->events = (const struct imf_event*)(const void*)&ptr[0];
    }
}


const struct imf_cmd imf_opl_tick(
    struct imf_status* status
)
{
    const struct imf_event* event = NULL;
    struct imf_cmd cmd = { 0, 0, 1 };
    uint16_t delay;
    assert(status);
    assert(status->events);

    if (status->divider) {
        status->divider--;
    }
    if (!status->divider) {
        status->divider = status->division;
        if (status->delay) {
            status->delay--;
        }
        if (!status->delay) {
            if (status->index < status->length) {
                event = &status->events[status->index++];
                delay = (((uint16_t)event->delay_hi << 8) | event->delay_lo);
                status->delay = delay;
                cmd.delaying = (uint8_t)(delay > 0);

                // Override virtual register 0x05 to extend the address range for OPL3
                if (event->address_lo == 0x05) {
                    status->address_hi = (event->value & 0x01);
                }
                else {
                    cmd.address = ((uint16_t)(status->address_hi << 8) | event->address_lo);
                    cmd.value = event->value;
                }
            }
            else {
                cmd.delaying = 2;  // EOF
            }
        }
    }
    return cmd;
}


#ifdef __cplusplus
}  // extern "C"
#endif
