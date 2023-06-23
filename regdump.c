#include "regdump.h"

#include <assert.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


void regdump_init(
    struct regdump_status* status
)
{
    assert(status);

    status->events = NULL;
    status->length = 0;
    status->index = 0;
    status->delay = 0;
}


void regdump_load(
    struct regdump_status* status,
    const void* data,
    size_t size
)
{
    uint8_t* ptr = (uint8_t*)data;
    uint32_t length_by_size;
    assert(status);

    status->events = NULL;
    status->length = 0;
    regdump_restart(status);

    length_by_size = (uint32_t)size;
    length_by_size /= sizeof(struct regdump_event);
    status->length = length_by_size;
    status->events = (const struct regdump_event*)(const void*)&ptr[0];
}


void regdump_restart(
    struct regdump_status* status
)
{
    assert(status);

    status->index = 0;
    status->delay = 0;
}


const struct regdump_cmd regdump_opl_tick(
    struct regdump_status* status
)
{
    const struct regdump_event* event = NULL;
    struct regdump_cmd cmd = { 0, 0, 1 };
    uint32_t delay;
    assert(status);
    assert(status->events);

    if (status->delay) {
        status->delay--;
    }
    if (!status->delay) {
        if (status->index < status->length) {
            event = &status->events[status->index++];
            if (event->address_hi & 0x80) {
                delay = (((uint32_t)(event->address_hi & 0x7F) << 16) |
                         ((uint32_t)event->address_lo << 8) | event->value);
                status->delay = delay;
                cmd.delaying = (uint8_t)(delay > 0);
            }
            else {
                cmd.address = (((uint16_t)event->address_hi << 8) | event->address_lo);
                cmd.value = event->value;
            }
        }
        else {
            cmd.delaying = 2;  // EOF
        }
    }
    return cmd;
}


#ifdef __cplusplus
}  // extern "C"
#endif
