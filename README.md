# Double-linked list

A compact array-backed doubly linked list implemented in C/C++. It stores `double` values and maintains `next` and `prev` links in parallel arrays. The list has a fixed maximum capacity of 256 slots, including its sentinel slot.

## Design

The list descriptor (`dl_list_t`) can be declared in a function's stack frame, so creating the descriptor itself does not require a separate allocation. Its backing arrays are allocated dynamically. The implementation also includes verification routines and error codes for reporting invalid pointers, invalid indexes, and other list errors.

> Stack-allocating the descriptor does not put the list's backing storage on the stack; the arrays are allocated separately.

## Graph visualization

`DlListGraphDump` writes a Graphviz DOT description of the list, including its nodes, forward and backward links, and free-slot chain. For example, pass a filename ending in `.dot` to generate the description, then render it with Graphviz:

```sh
dot -Tpng list.dot -o list.png
```

The DOT file can also be opened or converted with other Graphviz-compatible tools.
