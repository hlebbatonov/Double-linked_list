#include "dl_list.h"
#include <stdlib.h>

#define VERIFY_NULL_POINTER(pointer) do           \
{                                                 \
    if (pointer == NULL) return DL_LIST_ERR_NULL_POINTER; \
} while(0)

#define VERIFY_ERROR(err) do {if (err != DL_LIST_ERR_OK) return err;} while(0)

#define $(x) #x

DlListErrors DlListInit(dl_list_t* list, int start_num_of_elems);
DlListErrors DlListPushAfterPhys(dl_list_t* list, int node, dl_list_elem_t elem);
DlListErrors DlListPopAfterPhys(dl_list_t* list, int node, dl_list_elem_t* elem);
DlListErrors DlListVerify(dl_list_t* list);
DlListErrors DlListDump(dl_list_t* list);
DlListErrors DlListGraphDump(dl_list_t* list, const char* dot_filename);
DlListErrors DlListRemove(dl_list_t* list);
DlListErrors DlListGetIndex(dl_list_t* list, int node, int* index);

DlListErrors DlListPushByIterator(dl_list_t* list, dl_list_iterator_t index, dl_list_elem_t elem);
DlListErrors DlListPopByIterator(dl_list_t* list, dl_list_iterator_t index, dl_list_elem_t* elem);
DlListErrors DlListInitIterator(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorGetItem(dl_list_t* list, dl_list_iterator_t index, dl_list_elem_t* elem);
DlListErrors DlListIteratorBegin(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorEnd(dl_list_t* list, dl_list_iterator_t* index);
DlListErrors DlListIteratorNext(dl_list_t* list, dl_list_iterator_t index, dl_list_iterator_t* index_next);
DlListErrors DlListIteratorPrev(dl_list_t* list, dl_list_iterator_t index, dl_list_iterator_t* index_prev);
DlListErrors DlListIteratorInc(dl_list_t* list, dl_list_iterator_t* index);

int main()
{
    dl_list_t list = {};
    dl_list_iterator_t it = {};
    int start_num_of_elems = 16;

    dl_list_elem_t k = 0;

    DlListErrors init_err = DlListInit(&list, start_num_of_elems);
    DlListErrors init_it_err = DlListInitIterator(&list, &it);
    DlListIteratorBegin(&list, &it);
    DlListDump(&list);
    DlListPushByIterator(&list, it, 2.5);
    DlListDump(&list);
    DlListPushByIterator(&list, (dl_list_iterator_t){1}, 1.5);
    DlListPushByIterator(&list, (dl_list_iterator_t){2}, -0.1);
    DlListDump(&list);

    DlListGraphDump(&list, "1.txt");

    DlListPopByIterator(&list, (dl_list_iterator_t){1}, &k);
    DlListDump(&list);

    DlListRemove(&list);
}

DlListErrors DlListInit(dl_list_t* list, int start_num_of_elems)
{
    if (list == NULL) return DL_LIST_ERR_NULL_POINTER;
    if (start_num_of_elems <= 0) return DL_LIST_ERR_EMPTY_LIST;
    if (start_num_of_elems > MAX_CAPACITY - 1) return DL_LIST_ERR_LENGTH_TOO_BIG;

    list->capacity = start_num_of_elems + 1;

    list->data_arr = (dl_list_elem_t*)calloc((size_t)list->capacity, sizeof(dl_list_elem_t));
    VERIFY_NULL_POINTER(list->data_arr);

    list->next_arr = (int*)calloc((size_t)list->capacity, sizeof(int));
    VERIFY_NULL_POINTER(list->next_arr);

    list->prev_arr = (int*)calloc((size_t)list->capacity, sizeof(int));
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

    return DL_LIST_ERR_OK;
}

DlListErrors DlListPushAfterPhys(dl_list_t* list, int node, dl_list_elem_t elem)
{
    VERIFY_ERROR(DlListVerify(list));

    if (list->num_of_real_elems >= list->capacity - 1) return DL_LIST_ERR_LIST_OVERFLOWED;
    int curr_first_free_point = list->first_free_point;
    int next_node = list->next_arr[node];

    list->first_free_point = list->next_arr[curr_first_free_point];

    list->data_arr[curr_first_free_point] = elem;
    list->next_arr[curr_first_free_point] = next_node;
    list->prev_arr[curr_first_free_point] = node;

    list->prev_arr[next_node] = curr_first_free_point;
    list->next_arr[node] = curr_first_free_point;

    list->num_of_real_elems++;

    return DL_LIST_ERR_OK;
}

DlListErrors DlListPopAfterPhys(dl_list_t* list, int node, dl_list_elem_t* elem)
{
    VERIFY_NULL_POINTER(elem);
    VERIFY_ERROR(DlListVerify(list));
    if (list->num_of_real_elems == 0) return DL_LIST_ERR_POP_FROM_EMPTY_LIST;
    if (list->next_arr[node] == 0) return DL_LIST_ERR_POP_FICTIVE_ELEM;
    if (list->num_of_real_elems > list->capacity - 1) return DL_LIST_ERR_LIST_OVERFLOWED;

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

    return DL_LIST_ERR_OK;
}

DlListErrors DlListVerify(dl_list_t* list)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_NULL_POINTER(list->data_arr);
    VERIFY_NULL_POINTER(list->next_arr);
    VERIFY_NULL_POINTER(list->prev_arr);
    if (list->capacity < 1) return DL_LIST_ERR_EMPTY_LIST;
    if (list->num_of_real_elems < 0) return DL_LIST_ERR_EMPTY_LIST;
    if (list->first_free_point < 0) return DL_LIST_ERR_EMPTY_LIST;
    if (list->num_of_real_elems >= list->capacity) return DL_LIST_ERR_LIST_OVERFLOWED;
    if (list->data_arr[0] != POISON) return DL_LIST_ERR_FICTIVE_ELEM_CRASHED;

    return DL_LIST_ERR_OK;
}

DlListErrors DlListDump(dl_list_t* list)
{
    printf("DL List dump:\n");
    if (list == NULL) {printf("List's pointer is NULL\n"); return DL_LIST_ERR_NULL_POINTER;}

    printf("List's <%s> = %d, <%s> = %d, <%s> = %d\n", $(list->capacity), list->capacity, \
    $(list->num_of_real_elems), list->num_of_real_elems, $(list->first_free_point), list->first_free_point);

    for (int i = 0; i < list->capacity; i++)
        printf("%6d", i);
    printf("\n");

    for (int i = 0; i < list->capacity; i++)
    {
        if (list->data_arr[i] == POISON) printf("%6s", "PZ");
        else printf("%6" PRINTING_SPEC, list->data_arr[i]);
    }
    printf("\n");
    for (int i = 0; i < list->capacity; i++)
    {
        printf("%6d", list->next_arr[i]);
    }
    printf("\n");
    for (int i = 0; i < list->capacity; i++)
    {
        printf("%6d", list->prev_arr[i]);
    }
    printf("\n");

    return DL_LIST_ERR_OK;
}

DlListErrors DlListRemove(dl_list_t* list)
{
    DlListErrors curr_err = DL_LIST_ERR_OK;
    VERIFY_NULL_POINTER(list);

    if (list->capacity >= 1)
    {
        if (list->data_arr != NULL)
            for (int i = 1; i < list->capacity - 1; i++) {list->data_arr[i] = POISON;}
        else curr_err = DL_LIST_ERR_DATA_NULL_POINTER;
        if (list->next_arr != NULL)
            for (int i = 1; i < list->capacity - 1; i++) {list->next_arr[i] = -1;}
        else curr_err = DL_LIST_ERR_NEXT_NULL_POINTER;
        if (list->prev_arr != NULL)
            for (int i = 1; i < list->capacity - 1; i++) {list->prev_arr[i] = -1;}
        else curr_err = DL_LIST_ERR_PREV_NULL_POINTER;
    }
    else
        curr_err = DL_LIST_ERR_EMPTY_LIST;

    list->capacity = 0;
    list->first_free_point = 0;
    list->num_of_real_elems = 0;
    free(list->data_arr);
    free(list->next_arr);
    free(list->prev_arr);
    free(list);

    return curr_err;
}

DlListErrors DlListGraphDump(dl_list_t* list, const char* dot_filename)
{
    if (list == NULL) {fprintf(stderr, "Error: got NULL list pointer\n"); return DL_LIST_ERR_NULL_POINTER;}
    if (dot_filename == NULL) {fprintf(stderr, "Error: program filename crashed\n"); return DL_LIST_ERR_FILE_NOT_OPENED;}
    FILE* dot_file = fopen(dot_filename, "w");
    if (dot_file == NULL) {fprintf(stderr, "Error: no such file <%s>\n", dot_filename); return DL_LIST_ERR_FILE_NOT_OPENED;}
    //Head
    fprintf(dot_file, "digraph List_Dump\n{\n");
    fprintf(dot_file, "rankdir=LR;\n");
    fprintf(dot_file, "splines=true;\n");
    fprintf(dot_file, "node [shape=record, style=\"filled,rounded\", fontname=\"Arial\", group=1];\n\n");
    //Vertexes
    for (int i = 0; i < list->capacity; i++)
    {
        if (i == 0)
            fprintf(dot_file, "node_%d [fillcolor=\"%s\", label=\"Index: %d | { <prev> prev: %d | data: " "PZ" "| <next> next: %d } \"];\n",
                i, "#ffce47", i, list->prev_arr[i], list->next_arr[i]);
        else if (list->data_arr[i] == POISON)
            fprintf(dot_file, "node_%d [fillcolor=\"%s\", label=\"Index: %d | { <prev> prev: %d | data: " "PZ" "| <next> next: %d } \"];\n",
                i, "#c8c8c8", i, list->prev_arr[i], list->next_arr[i]);
        else
            fprintf(dot_file, "node_%d [fillcolor=\"%s\", label=\"Index: %d | { <prev> prev: %d | data: %" PRINTING_SPEC "| <next> next: %d }\"];\n",
                i, "#3fffae", i, list->prev_arr[i], list->data_arr[i], list->next_arr[i]);
    }
    //FREE point
    fprintf(dot_file, "node_free [shape=circle, style=\"filled,rounded\", fillcolor=\"#8931a7\", \
        fontname=\"Arial\", fontcolor=\"white\", label=\"FREE\"];\n");

    if (list->first_free_point >= 0 && list->first_free_point < list->capacity)
    {
        fprintf(dot_file, "node_%d:n -> node_free [style=invis];\n", list->first_free_point);
        fprintf(dot_file, "node_free -> node_%d [color=\"#c8c8c8\", style=bold, \
            constraint=false];\n", list->first_free_point);
    }
    //Edges
    for (int i = 0; i < list->capacity; i++)
    {
        if (i < list->capacity - 1)
            fprintf(dot_file, "node_%d -> node_%d [style=invis];\n", i, i + 1);

        if (list->next_arr[i] >= 0 && list->next_arr[i] < list->capacity) {
            fprintf(dot_file, "node_%d:next:s -> node_%d:s" "[tailport=s, headport=s, color=\"%s\", \
            constraint=false];\n", i, list->next_arr[i], (list->data_arr[i] == POISON && \
            list->data_arr[list->next_arr[i]] == POISON) ? "#c8c8c8" : "#0288d1");
        }


        if (list->prev_arr[i] >= 0 && list->prev_arr[i] < list->capacity) {
            fprintf(dot_file, "node_%d:n -> node_%d:prev:n" "[tailport=n, headport=n, color=\"#d32f2f\", \
            dir=back, style=dashed, constraint=false];\n", list->prev_arr[i], i);
        }
    }
    fprintf(dot_file, "\n");


    fprintf(dot_file, "\n}");
    fclose(dot_file);
    return DL_LIST_ERR_OK;
}

DlListErrors DlListGetIndex(dl_list_t* list, int node, int* index)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_NULL_POINTER(index);
    if (node < 0 || node >= list->num_of_real_elems) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;
    int curr_index = 0;
    for (int i = 0; i <= node; i++)
    {
        curr_index = list->next_arr[curr_index];
    }
    *index = curr_index;
    return DL_LIST_ERR_OK;
}

DlListErrors DlListPushByIterator(dl_list_t* list, dl_list_iterator_t index, dl_list_elem_t elem)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_ERROR(DlListVerify(list));
    int phys_index_before = 0;
    if (index.node == 0) phys_index_before = 0;
    else VERIFY_ERROR(DlListGetIndex(list, index.node - 1, &phys_index_before));

    VERIFY_ERROR(DlListPushAfterPhys(list, phys_index_before, elem));
    VERIFY_ERROR(DlListVerify(list));
    return DL_LIST_ERR_OK;
}

DlListErrors DlListPopByIterator(dl_list_t* list, dl_list_iterator_t index, dl_list_elem_t* elem)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(elem);

    if (list->num_of_real_elems == 0) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;
    if (index.node < 0 || index.node >= list->num_of_real_elems) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;

    int phys_index_before = 0;
    if (index.node == 0) phys_index_before = 0;
    else VERIFY_ERROR(DlListGetIndex(list, index.node - 1, &phys_index_before));

    VERIFY_ERROR(DlListPopAfterPhys(list, phys_index_before, elem));
    VERIFY_ERROR(DlListVerify(list));
    return DL_LIST_ERR_OK;
}

DlListErrors DlListInitIterator(dl_list_t* list, dl_list_iterator_t* index)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index);

    index->node = list->next_arr[0];

    return DL_LIST_ERR_OK;
}

DlListErrors DlListIteratorGetItem(dl_list_t* list, dl_list_iterator_t index, dl_list_elem_t* elem)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(elem);

    if (list->capacity <= index.node) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;
    if (index.node == 0) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;

    *elem = list->data_arr[index.node];

    return DL_LIST_ERR_OK;
}

DlListErrors DlListIteratorBegin(dl_list_t* list, dl_list_iterator_t* index)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index);

    index->node = list->next_arr[0];

    return DL_LIST_ERR_OK;
}

DlListErrors DlListIteratorEnd(dl_list_t* list, dl_list_iterator_t* index)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index);

    index->node = 0;

    return DL_LIST_ERR_OK;
}

DlListErrors DlListIteratorNext(dl_list_t* list, dl_list_iterator_t index, dl_list_iterator_t* index_next)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index_next);

    if (list->capacity <= index.node) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;
    if (index.node == 0) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;

    index_next->node = list->next_arr[index.node];

    return DL_LIST_ERR_OK;
}
DlListErrors DlListIteratorPrev(dl_list_t* list, dl_list_iterator_t index, dl_list_iterator_t* index_prev)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index_prev);

    if (list->capacity <= index.node) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;
    if (index.node == 0) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;

    index_prev->node = list->prev_arr[index.node];

    return DL_LIST_ERR_OK;
}
