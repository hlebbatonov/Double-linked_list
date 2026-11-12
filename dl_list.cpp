#include "dl_list.h"
#include <stdlib.h>

#define VERIFY_NULL_POINTER(pointer) do                   \
{                                                         \
    if (pointer == NULL) return DL_LIST_ERR_NULL_POINTER; \
} while(0)

#define VERIFY_ERROR(err) do {if (err != DL_LIST_ERR_OK) return err;} while(0)

#define $(x) #x

DlListErrors DlListPushAfterPhys(dl_list_t* list, int node, dl_list_elem_t elem);
DlListErrors DlListPopAfterPhys(dl_list_t* list, int node, dl_list_elem_t* elem);
DlListErrors DlListDumpToFile(dl_list_t* list, FILE* dump_file);
int DlListRunGraphviz(const char* dot_filename, const char* svg_filename);

DlListErrors DlListInit(dl_list_t* list, int start_capacity)
{
    if (list == NULL) return DL_LIST_ERR_NULL_POINTER;
    if (start_capacity <= 0) return DL_LIST_ERR_EMPTY_LIST;
    if (start_capacity > MAX_CAPACITY - 1) return DL_LIST_ERR_LENGTH_TOO_BIG;

    list->capacity = start_capacity + 1;

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
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_ERROR(DlListDumpToFile(list, stdout));

    return DL_LIST_ERR_OK;
}

DlListErrors DlListDumpToFile(dl_list_t* list, FILE* dump_file)
{
    VERIFY_NULL_POINTER(dump_file);

    fprintf(dump_file, "DL List dump:\n");
    if (list == NULL) {fprintf(dump_file, "List's pointer is NULL\n"); return DL_LIST_ERR_NULL_POINTER;}

    fprintf(dump_file, "List's [%s] = %d, [%s] = %d, [%s] = %d\n", $(list->capacity), list->capacity,
        $(list->num_of_real_elems), list->num_of_real_elems, $(list->first_free_point), list->first_free_point);

    for (int i = 0; i < list->capacity; i++)
        fprintf(dump_file, "%6d", i);
    fprintf(dump_file, "\n");

    for (int i = 0; i < list->capacity; i++)
    {
        if (list->data_arr[i] == POISON) fprintf(dump_file, "%6s", "PZ");
        else fprintf(dump_file, "%6" PRINTING_SPEC, list->data_arr[i]);
    }
    fprintf(dump_file, "\n");
    for (int i = 0; i < list->capacity; i++)
    {
        fprintf(dump_file, "%6d", list->next_arr[i]);
    }
    fprintf(dump_file, "\n");
    for (int i = 0; i < list->capacity; i++)
    {
        fprintf(dump_file, "%6d", list->prev_arr[i]);
    }
    fprintf(dump_file, "\n");
    fprintf(dump_file, "\n");
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
    if (dot_file == NULL) {fprintf(stderr, "Error: no such file [%s]\n", dot_filename); return DL_LIST_ERR_FILE_NOT_OPENED;}
    //Head
    fprintf(dot_file, "digraph List_Dump\n{\n");
    fprintf(dot_file, "rankdir=LR;\n");
    fprintf(dot_file, "splines=true;\n");
    fprintf(dot_file, "nodesep=1.0;\n");
    fprintf(dot_file, "ranksep=0.6;\n");
    fprintf(dot_file, "node [shape=record, style=\"filled,rounded\", fontname=\"Arial\", group=1];\n\n");
    //Vertexes
    for (int i = 0; i < list->capacity; i++)
    {
        if (i == 0)
            fprintf(dot_file, "node_%d [fillcolor=\"%s\", label=\"Index: %d | { <prev> prev: %d | data: " "PZ" "| <next> next: %d } \"];\n",
                i, "#ffce47", i, list->prev_arr[i], list->next_arr[i]);
        else if (i == list->first_free_point)
            fprintf(dot_file, "node_%d [fillcolor=\"#8931a7\", fontcolor=\"white\", label=\"FREE | Index: %d | { <prev> prev: %d | data: PZ | <next> next: %d }\"];\n",
                i, i, list->prev_arr[i], list->next_arr[i]);
        else if (list->data_arr[i] == POISON)
            fprintf(dot_file, "node_%d [fillcolor=\"%s\", label=\"Index: %d | { <prev> prev: %d | data: " "PZ" "| <next> next: %d } \"];\n",
                i, "#c8c8c8", i, list->prev_arr[i], list->next_arr[i]);
        else
            fprintf(dot_file, "node_%d [fillcolor=\"%s\", label=\"Index: %d | { <prev> prev: %d | data: %" PRINTING_SPEC "| <next> next: %d }\"];\n",
                i, "#3fffae", i, list->prev_arr[i], list->data_arr[i], list->next_arr[i]);
    }
    //Edges
    for (int i = 0; i < list->capacity; i++)
    {
        if (i < list->capacity - 1)
            fprintf(dot_file, "node_%d -> node_%d [style=invis];\n", i, i + 1);

        if (list->next_arr[i] >= 0 && list->next_arr[i] < list->capacity) {
            fprintf(dot_file, "node_%d:s -> node_%d:s" "[color=\"%s\", \
            constraint=false];\n", i, list->next_arr[i], (list->data_arr[i] == POISON && \
            list->data_arr[list->next_arr[i]] == POISON) ? "#c8c8c8" : "#0288d1");
        }

        if (list->prev_arr[i] >= 0 && list->prev_arr[i] < list->capacity) {
            fprintf(dot_file, "node_%d:n -> node_%d:n [color=\"#d32f2f\", style=dashed, dir=back, constraint=false];\n",
                list->prev_arr[i], i);
        }
    }
    fprintf(dot_file, "\n");


    fprintf(dot_file, "\n}");
    fclose(dot_file);
    return DL_LIST_ERR_OK;
}

DlListErrors DlListHTMLDump(dl_list_t* list, const char* html_filename)
{
    if (list == NULL) {fprintf(stderr, "Error: got NULL list pointer\n"); return DL_LIST_ERR_NULL_POINTER;}
    if (html_filename == NULL) {fprintf(stderr, "Error: program filename crashed\n"); return DL_LIST_ERR_FILE_NOT_OPENED;}
    FILE* html_file = fopen(html_filename, "a");
    if (html_file == NULL) {fprintf(stderr, "Error: no such file [%s]\n", html_filename); return DL_LIST_ERR_FILE_NOT_OPENED;}

    fprintf(html_file, "<pre>\n");
    DlListErrors err = DlListDumpToFile(list, html_file);
    if (err != DL_LIST_ERR_OK) {fclose(html_file); return err;}
    fprintf(html_file, "</pre>\n");

    err = DlListGraphDump(list, "temp.txt");
    if (err != DL_LIST_ERR_OK) {fclose(html_file); return err;}
    if (DlListRunGraphviz("temp.txt", "temp.svg") != 0) return ERR_GRAPHVIZ_FAILED;

    FILE* svg_file = fopen("temp.svg", "r");
    if (svg_file != NULL)
    {
        int sym = 0;
        while ((sym = fgetc(svg_file)) != EOF) fputc(sym, html_file);
    }
    fclose(svg_file);
    remove("temp.svg");


    fclose(html_file);
    return DL_LIST_ERR_OK;

}

int DlListRunGraphviz(const char* dot_filename, const char* svg_filename)
{
    VERIFY_NULL_POINTER(dot_filename);
    VERIFY_NULL_POINTER(svg_filename);

    char cmd[SIZE_OF_CMD] = {0};

    int len = snprintf(cmd, sizeof(cmd), "dot -Tsvg \"%s\" -o \"%s\"", dot_filename, svg_filename);
    if (len < 0 || (size_t)len >= sizeof(cmd)) {fprintf(stderr, "Error: command string is too long\n"); return -1;}
    int status = system(cmd);
    if (status != 0) {fprintf(stderr, "Error: failed to execute <%s>, code: %d\n", cmd, status); return status;}

    return 0;
}


DlListErrors DlListPushByIterator(dl_list_t* list, dl_list_iterator_t* index, dl_list_elem_t elem)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_ERROR(DlListVerify(list));

    if (index->node < 0 || index->node >= list->capacity) return DL_LIST_ERR_INVALID_ITERATOR;

    VERIFY_ERROR(DlListPushAfterPhys(list, list->prev_arr[index->node], elem));
    index->node = list->prev_arr[index->node];
    VERIFY_ERROR(DlListVerify(list));
    return DL_LIST_ERR_OK;
}

DlListErrors DlListPopByIterator(dl_list_t* list, dl_list_iterator_t* index, dl_list_elem_t* elem)
{
    VERIFY_NULL_POINTER(list);
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(elem);

    if (list->num_of_real_elems == 0) return DL_LIST_ERR_EMPTY_LIST;
    if (index->node < 0 || index->node >= list->capacity) return DL_LIST_ERR_INVALID_ITERATOR;
    if (list->data_arr[index->node] == POISON) return DL_LIST_ERR_INVALID_ITERATOR;

    int next_node = list->next_arr[index->node];
    VERIFY_ERROR(DlListPopAfterPhys(list, list->prev_arr[index->node], elem));
    index->node = next_node;

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

DlListErrors DlListIteratorInc(dl_list_t* list, dl_list_iterator_t* index)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index);

    dl_list_iterator_t it_end = {};
    VERIFY_ERROR(DlListIteratorEnd(list, &it_end));
    if (index->node >= list->capacity || index->node < 0) return DL_LIST_ERR_INVALID_ITERATOR;
    if (index->node == it_end.node) return DL_LIST_ERR_END_OF_LIST;
    if (list->data_arr[index->node] == POISON) return DL_LIST_ERR_INVALID_ITERATOR;
    index->node = list->next_arr[index->node];
    return DL_LIST_ERR_OK;
}

DlListErrors DlListIteratorDec(dl_list_t* list, dl_list_iterator_t* index)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index);

    dl_list_iterator_t it_begin = {};
    dl_list_iterator_t it_end = {};
    VERIFY_ERROR(DlListIteratorBegin(list, &it_begin));
    VERIFY_ERROR(DlListIteratorEnd(list, &it_end));
    if (index->node > list->capacity || index->node < 0) return DL_LIST_ERR_INVALID_ITERATOR;
    if (index->node == it_begin.node) return DL_LIST_ERR_BEGINNING_OF_LIST;
    if (index->node != it_end.node && list->data_arr[index->node] == POISON) return DL_LIST_ERR_INVALID_ITERATOR;
    index->node = list->prev_arr[index->node];
    return DL_LIST_ERR_OK;
}

DlListErrors DlListIteratorMoveTo(dl_list_t* list, dl_list_iterator_t* index, int position)
{
    VERIFY_ERROR(DlListVerify(list));
    VERIFY_NULL_POINTER(index);

    dl_list_iterator_t it_begin = {};
    VERIFY_ERROR(DlListIteratorBegin(list, &it_begin));

    if (index->node >= list->capacity || index->node < 0) return DL_LIST_ERR_INVALID_ITERATOR;
    if (position > list->num_of_real_elems || position < 0) return DL_LIST_ERR_INDEX_OUT_OF_RANGE;

    index->node = it_begin.node;
    for (int curr_pos = 0; curr_pos < position; curr_pos++) index->node = list->next_arr[index->node];

    return DL_LIST_ERR_OK;
}
