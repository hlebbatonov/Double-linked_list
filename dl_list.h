#include <stdio.h>

#define PRINTING_SPEC "g"

enum DlListErrors
{
    DL_LIST_ERR_OK = 0,

    DL_LIST_ERR_NULL_POINTER,
    DL_LIST_ERR_DATA_NULL_POINTER,
    DL_LIST_ERR_NEXT_NULL_POINTER,
    DL_LIST_ERR_PREV_NULL_POINTER,
    DL_LIST_ERR_LIST_NULL_POINTER,

    DL_LIST_ERR_ZERO_LENGTH,

    DL_LIST_ERR_LENGTH_TOO_BIG,
    DL_LIST_ERR_LENS_ARE_NOT_EQUAL,

    DL_LIST_ERR_DATA_CALLOC_ERROR,
    DL_LIST_ERR_NEXT_CALLOC_ERROR,
    DL_LIST_ERR_LIST_CALLOC_ERROR,
    DL_LIST_ERR_LIST_EXISTS,

    DL_LIST_ERR_LIST_OVERFLOWED,
    DL_LIST_ERR_FREE_PART_CRASHED,

    DL_LIST_ERR_EMPTY_LIST,
    DL_LIST_ERR_POP_FROM_EMPTY_LIST,
    DL_LIST_ERR_INVALID_INDEX,
    DL_LIST_ERR_INVALID_ITERATOR,
    DL_LIST_ERR_END_OF_LIST,
    DL_LIST_ERR_BEGINNING_OF_LIST,
    DL_LIST_ERR_POP_FICTIVE_ELEM,
    DL_LIST_ERR_FILE_NOT_OPENED,
    DL_LIST_ERR_INDEX_OUT_OF_RANGE,
    DL_LIST_ERR_FICTIVE_ELEM_CRASHED,
    ERR_CMD_TOO_LONG,
    ERR_GRAPHVIZ_FAILED
};

typedef double dl_list_elem_t;

struct dl_list_t
{
    dl_list_elem_t* data_arr;
    int* next_arr;
    int* prev_arr;
    int capacity;
    int num_of_real_elems;
    int first_free_point;
};

struct dl_list_iterator_t
{
    int node;
};

const int MAX_CAPACITY = 256;
const dl_list_elem_t POISON = 0xBEDA;
const int SIZE_OF_CMD = 128;

DlListErrors DlListInit(dl_list_t* list, int start_num_of_elems);
DlListErrors DlListVerify(dl_list_t* list);
DlListErrors DlListDump(dl_list_t* list);
DlListErrors DlListGraphDump(dl_list_t* list, const char* dot_filename);
DlListErrors DlListHTMLDump(dl_list_t* list, const char* html_filename);
DlListErrors DlListRemove(dl_list_t* list);


DlListErrors DlListPushByIterator(dl_list_t* list, dl_list_iterator_t* index, dl_list_elem_t elem);
DlListErrors DlListPopByIterator(dl_list_t* list, dl_list_iterator_t* index, dl_list_elem_t* elem);
DlListErrors DlListInitIterator(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorGetItem(dl_list_t* list, dl_list_iterator_t index, dl_list_elem_t* elem);
DlListErrors DlListIteratorBegin(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorEnd(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorNext(dl_list_t* list, dl_list_iterator_t index, dl_list_iterator_t* index_next);
DlListErrors DlListIteratorPrev(dl_list_t* list, dl_list_iterator_t index, dl_list_iterator_t* index_prev);
DlListErrors DlListIteratorInc(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorDec(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorMoveTo(dl_list_t* list, dl_list_iterator_t* index, int position);

