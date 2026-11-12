# Double-linked list

A compact, array-backed doubly linked list implemented in C++. It stores `double` values and keeps each node's `next` and `prev` links in parallel arrays. The list has a fixed maximum capacity of 256 slots, including the sentinel slot.

## Design

The `dl_list_t` descriptor can be declared in a function's stack frame, while its `data_arr`, `next_arr`, and `prev_arr` backing arrays are allocated dynamically by `DlListInit`. The list uses slot `0` as a sentinel; `first_free_point` tracks the head of the free-slot chain. Iterators refer to nodes by their logical position in the list.

> Stack-allocating the descriptor does not put its backing arrays on the stack.

## Graph visualization

`DlListGraphDump` writes a Graphviz DOT description of the list, including its nodes, forward and backward links, and free-slot chain. For example:

```cpp
DlListGraphDump(&list, "list.dot");
```

Convert the DOT file to a PNG if you want a separate image file:

```sh
dot -Tpng list.dot -o list.png
```

### Put an SVG graphic directly in HTML

You can include the SVG markup inside the HTML document itself, so there is no separate `.svg` picture file on disk. Generate SVG markup from the DOT file and print it to the terminal:

```sh
dot -Tsvg list.dot
```

Copy the resulting `<svg>...</svg>` markup into the HTML body, for example:

```html
<body>
  <h1>List visualization</h1>
  <!-- Paste the SVG markup produced by `dot -Tsvg list.dot` here. -->
  <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 40">
    <!-- Replace this example with the generated SVG elements. -->
    <text x="5" y="25">List visualization</text>
  </svg>
</body>
```

This embeds the graphic in the HTML rather than saving a separate `.svg` image. The DOT source file is still written by `DlListGraphDump`; `dot -Tsvg list.dot` writes SVG markup to standard output and does not create an SVG file. The DOT file can also be opened or converted with other Graphviz-compatible tools.
