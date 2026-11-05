#include "dl_list.h"
#include <stdlib.h>

Errors DlListInit(dl_list_t* list, int start_num_of_elems);
Errors DlListPush(dl_list_t* list, int index, elem elem);
Errors DlListPop(dl_list_t* list, int index, elem* elem);
Errors DlListPushAfterPhys(dl_list_t* list, int node, elem elem);
Errors DlListPopAfterPhys(dl_list_t* list, int node, elem* elem);
Errors DlListVerify(dl_list_t* list);
Errors DlListDump(dl_list_t* list);
Errors DlListGraphDump(dl_list_t* list, const char* dot_filename);
Errors DlListRemove(dl_list_t* list);
Errors DlListGetIndex(dl_list_t* list, int node, int* index);

int main()
{
    dl_list_t list = {};
    int start_num_of_elems = 16;

    elem k = 0;

    Errors init_err = DlListInit(&list, start_num_of_elems);
    DlListDump(&list);
    DlListPush(&list, 0, 2.5);
    DlListDump(&list);
    DlListPush(&list, 1, 1.5);
    DlListPush(&list, 2, 15);
    DlListPush(&list, 3, -0.1);
    DlListDump(&list);

    DlListGraphDump(&list, "1.txt");

    DlListPop(&list, 1, &k);
    DlListDump(&list);

    DlListRemove(&list);
}

Errors DlListInit(dl_list_t* list, int start_num_of_elems)
{
    if (list == NULL) return ERR_NULL_POINTER;
    if (start_num_of_elems <= 0) return ERR_EMPTY_LIST;
    if (start_num_of_elems > MAX_CAPACITY - 1) return ERR_LENGTH_TOO_BIG;

    list->capacity = start_num_of_elems + 1;

    list->data_arr = (elem*)calloc((size_t)list->capacity, sizeof(elem));
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

    return OK;
}

Errors DlListPushAfterPhys(dl_list_t* list, int node, elem elem)
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

Errors DlListPopAfterPhys(dl_list_t* list, int node, elem* elem)
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

    return OK;
}

Errors DlListRemove(dl_list_t* list)
{
    Errors curr_err = OK;
    VERIFY_NULL_POINTER(list);

    if (list->capacity >= 1)
    {
        if (list->data_arr != NULL)
            for (int i = 1; i < list->capacity - 1; i++) {list->data_arr[i] = POISON;}
        else curr_err = ERR_DATA_NULL_POINTER;
        if (list->next_arr != NULL)
            for (int i = 1; i < list->capacity - 1; i++) {list->next_arr[i] = -1;}
        else curr_err = ERR_NEXT_NULL_POINTER;
        if (list->prev_arr != NULL)
            for (int i = 1; i < list->capacity - 1; i++) {list->prev_arr[i] = -1;}
        else curr_err = ERR_PREV_NULL_POINTER;
    }
    else
        curr_err = ERR_EMPTY_LIST;

    list->capacity = 0;
    list->first_free_point = 0;
    list->num_of_real_elems = 0;
    free(list->data_arr);
    free(list->next_arr);
    free(list->prev_arr);
    free(list);

    return curr_err;
}

Errors DlListGraphDump(dl_list_t* list, const char* dot_filename)
{
    if (dot_filename == NULL) {fprintf(stderr, "Error: program filename crashed\n"); return ERR_FILE_NOT_OPENED;}
    FILE* dot_file = fopen(dot_filename, "w");
    if (dot_file == NULL) {fprintf(stderr, "Error: no such file <%s>\n", dot_filename); return ERR_FILE_NOT_OPENED;}
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
    return OK;
}

Errors DlListGetIndex(dl_list_t* list, int node, int* index)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_NULL_POINTER(index);
    if (node < 0 || node >= list->num_of_real_elems) return ERR_INDEX_OUT_OF_RANGE;
    int curr_index = 0;
    for (int i = 0; i <= node; i++)
    {
        curr_index = list->next_arr[curr_index];
    }
    *index = curr_index;
    return OK;
}

Errors DlListPush(dl_list_t* list, int index, elem elem)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_ERROR(DlListVerify(list));
    int phys_index_before = 0;
    if (index == 0) phys_index_before = 0;
    else VERIFY_ERROR(DlListGetIndex(list, index - 1, &phys_index_before));

    VERIFY_ERROR(DlListPushAfterPhys(list, phys_index_before, elem));
    VERIFY_ERROR(DlListVerify(list));
    return OK;
}

Errors DlListPop(dl_list_t* list, int index, elem* elem)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(elem);

    if (list->num_of_real_elems == 0) return ERR_INDEX_OUT_OF_RANGE;
    if (index < 0 || index >= list->num_of_real_elems) return ERR_INDEX_OUT_OF_RANGE;

    int phys_index_before = 0;
    if (index == 0) phys_index_before = 0;
    else VERIFY_ERROR(DlListGetIndex(list, index - 1, &phys_index_before));

    VERIFY_ERROR(DlListPopAfterPhys(list, phys_index_before, elem));
    VERIFY_ERROR(DlListVerify(list));
    return OK;
}
