#include "dl_list.h"
#include <stdlib.h>

Errors DlListInit(dl_list_t* list, size_t start_capacity);
Errors DlListPushAfter(dl_list_t* list, int node, elem elem);
Errors DlListPopAfter(dl_list_t* list, int node, elem* elem);
Errors DlListVerify(dl_list_t* list);
Errors DlListDump(dl_list_t* list);

int main()
{
    dl_list_t list = {};
    size_t start_num_of_elems = 16;

    elem k = 0;

    Errors init_err = DlListInit(&list, start_num_of_elems);
    DlListPushAfter(&list, 0, 1.5);
    DlListPushAfter(&list, 1, 1.5);
    DlListPopAfter(&list, 0, &k);
    DlListDump(&list);
}

Errors DlListInit(dl_list_t* list, size_t start_num_of_elems)
{
    if (list == NULL) return ERR_NULL_POINTER;
    if (start_num_of_elems <= 0) return ERR_EMPTY_LIST;
    if (start_num_of_elems > MAX_CAPACITY - 1) return ERR_LENGTH_TOO_BIG;

    list->capacity = start_num_of_elems + 1;

    list->data_arr = (elem*)calloc(list->capacity, sizeof(elem));
    VERIFY_NULL_POINTER(list->data_arr);

    list->next_arr = (int*)calloc(list->capacity, sizeof(int));
    VERIFY_NULL_POINTER(list->next_arr);

    list->prev_arr = (int*)calloc(list->capacity, sizeof(int));
    VERIFY_NULL_POINTER(list->prev_arr);

    list->data_arr[0] = POISON;
    list->next_arr[0] = 0;
    list->prev_arr[0] = 0;
    list->num_of_real_elems = 0;
    list->first_free_point = 1;

    for (int i = 1; i < list->capacity - 1; i++)
    {
        list->data_arr[i] = POISON;
        list->next_arr[i] = i + 1;
        list->prev_arr[i] = -1;
    }
    list->data_arr[list->capacity - 1] = POISON;
    list->next_arr[list->capacity - 1] = 0;
    list->prev_arr[list->capacity - 1] = -1;

    return OK;
}

Errors DlListPushAfter(dl_list_t* list, int node, elem elem)
{
    VERIFY_ERROR(DlListVerify(list));
    if (list->num_of_real_elems >= list->capacity - 1) return ERR_LIST_OVERFLOWED;
    int curr_first_free_point = list->first_free_point;
    int next_node = list->next_arr[node];

    list->first_free_point = list->next_arr[curr_first_free_point];

    list->data_arr[curr_first_free_point] = elem;
    list->next_arr[curr_first_free_point] = next_node;
    list->prev_arr[curr_first_free_point] = node;

    list->prev_arr[next_node] = curr_first_free_point;
    list->next_arr[node] = curr_first_free_point;

    list->num_of_real_elems++;

    return OK;
}

Errors DlListPopAfter(dl_list_t* list, int node, elem* elem)
{
    VERIFY_NULL_POINTER(elem);
    VERIFY_ERROR(DlListVerify(list));
    if (list->num_of_real_elems == 0) return ERR_POP_FROM_EMPTY_LIST;
    if (list->next_arr[node] == 0) return ERR_POP_FICTIVE_ELEM;
    if (list->num_of_real_elems > list->capacity - 1) return ERR_LIST_OVERFLOWED;

    int node_to_pop = list->next_arr[node];
    int node_after = list->next_arr[node_to_pop];

    *elem = list->data_arr[node_to_pop];

    list->next_arr[node] = node_after;
    list->prev_arr[node_after] = node;

    list->data_arr[node_to_pop] = POISON;

    list->next_arr[node_to_pop] = list->first_free_point;
    list->prev_arr[node_to_pop] = -1;

    list->first_free_point = node_to_pop;

    list->num_of_real_elems--;

    return OK;
}

Errors DlListVerify(dl_list_t* list)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_NULL_POINTER(list->data_arr);
    VERIFY_NULL_POINTER(list->next_arr);
    VERIFY_NULL_POINTER(list->prev_arr);
    if (list->capacity < 1) return ERR_EMPTY_LIST;
    if (list->num_of_real_elems < 0) return ERR_EMPTY_LIST;
    if (list->first_free_point < 0) return ERR_EMPTY_LIST;
    if (list->num_of_real_elems >= list->capacity) return ERR_LIST_OVERFLOWED;
    if (list->data_arr[0] != POISON) return ERR_FICTIVE_ELEM_CRASHED;

    return OK;
}

Errors DlListDump(dl_list_t* list)
{
    printf("DL List dump:\n");
    if (list == NULL) {printf("List's pointer is NULL\n"); return ERR_NULL_POINTER;}

    printf("List's <%s> = %zu, <%s> = %zu, <%s> = %d\n", $(list->capacity), list->capacity, \
    $(list->num_of_real_elems), list->num_of_real_elems, $(list->first_free_point), list->first_free_point);

    for (int i = 0; i < list->capacity; i++)
    {
        if (list->data_arr[i] == POISON) printf("%4s", "PZ");
        else printf("%4g", list->data_arr[i]);
    }
    printf("\n");
    for (int i = 0; i < list->capacity; i++)
    {
        printf("%4d", list->next_arr[i]);
    }
    printf("\n");
    for (int i = 0; i < list->capacity; i++)
    {
        printf("%4d", list->prev_arr[i]);
    }
    printf("\n");

    return OK;
}

