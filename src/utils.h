#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define define_da_named(type, name) \
    typedef struct { \
        int length; \
        int capacity; \
        type *data; \
    } name; \
    name name##_init(int capacity) { \
        type *data = (type *)malloc(capacity * sizeof(type)); \
        return (name) { \
            .length = 0, \
            .capacity= capacity, \
            .data = data, \
        };\
    }\
    void name##_append(name *da, type val) {\
        da->data = da_append(da->data, &da->length, &da->capacity, &val, sizeof(val)); \
    }

#define define_da(type) define_da_named(type, type##_da)

void *da_append(void *data, int *length, int *capacity, void *element, size_t element_size) {
    if(*length >= *capacity) {
        *capacity *= 2;
        data = realloc(data, *capacity * element_size);
    }
    uint8_t *dst = (uint8_t *)data + *length * element_size;
    memcpy(dst, element, element_size);
    *length = *length + 1;
    return data;
}


#endif  // UTILS_H