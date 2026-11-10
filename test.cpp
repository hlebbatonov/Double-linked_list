#include "dl_list.h"

#define VERIFY_AND_PRINT_ERROR(error) do {if(error != DL_LIST_ERR_OK) \
    fprintf(stderr, "ERROR code: %d\n", error);} while(0)

int main()
{
    dl_list_t list = {};
    dl_list_iterator_t it = {};
    int start_capacity = 16;

    dl_list_elem_t k = 0;

    DlListErrors err = DlListInit(&list, start_capacity);
    VERIFY_AND_PRINT_ERROR(err);
    err = DlListInitIterator(&list, &it);
    VERIFY_AND_PRINT_ERROR(err);
    err = DlListIteratorBegin(&list, &it);
    VERIFY_AND_PRINT_ERROR(err);
    DlListDump(&list);

    err = DlListPushByIterator(&list, &it, 2.5);
    VERIFY_AND_PRINT_ERROR(err);
    DlListDump(&list);
    err = DlListIteratorInc(&list, &it);
    VERIFY_AND_PRINT_ERROR(err);
    err = DlListPushByIterator(&list, &it, 1.5);
    VERIFY_AND_PRINT_ERROR(err);
    DlListDump(&list);
    err = DlListIteratorInc(&list, &it);
    VERIFY_AND_PRINT_ERROR(err);
    err = DlListPushByIterator(&list, &it, -0.1);
    VERIFY_AND_PRINT_ERROR(err);
    DlListDump(&list);
    err = DlListIteratorMoveTo(&list, &it, 1);
    VERIFY_AND_PRINT_ERROR(err);
    DlListPopByIterator(&list, &it, &k);
    DlListDump(&list);

    DlListGraphDump(&list, "1.txt");

    DlListRemove(&list);
}
