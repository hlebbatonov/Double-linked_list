#include <stdio.h>

#define VERIFY_NULL_POINTER(pointer) do           \
{                                                 \
    if (pointer == NULL) return ERR_NULL_POINTER; \
} while(0)

#define VERIFY_ERROR(err) do {if (err != OK) return err;} while(0)

#define $(x) #x

enum Errors
{
    OK = 0, //info

    ERR_NULL_POINTER, //error
    ERR_DATA_NULL_POINTER, //error
    ERR_NEXT_NULL_POINTER, //error
    ERR_PREV_NULL_POINTER,
    ERR_LIST_NULL_POINTER, //error

    ERR_ZERO_LENGTH, //error

    ERR_LENGTH_TOO_BIG, //error
    ERR_LENS_ARE_NOT_EQUAL,

    ERR_DATA_CALLOC_ERROR, //fatal
    ERR_NEXT_CALLOC_ERROR,
    ERR_LIST_CALLOC_ERROR,
    ERR_LIST_EXISTS, //error

    ERR_LIST_OVERFLOWED, //fatal
    ERR_FREE_PART_CRASHED, //fatal

    ERR_EMPTY_LIST,
    ERR_POP_FROM_EMPTY_LIST,
    ERR_INVALID_INDEX,
    ERR_POP_FICTIVE_ELEM,
    ERR_FICTIVE_ELEM_CRASHED
};

typedef double elem;

struct dl_list_t
{
    elem* data_arr;
    int* next_arr;
    int* prev_arr;
    size_t capacity;
    size_t num_of_real_elems;
    int first_free_point;
};

const size_t MAX_CAPACITY = 256;
const elem POISON = 0xBEDA;
