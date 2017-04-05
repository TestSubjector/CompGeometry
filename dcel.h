struct half_edge;

struct vertex {
    struct half_edge *e;
}

struct face {
    struct half_edge *e;
}

struct half_edge {
    struct half_edge *prev;
    struct half_edge *next;
    struct half_edge *twin;
    struct vertex *tail;
    struct face *left; 
}
