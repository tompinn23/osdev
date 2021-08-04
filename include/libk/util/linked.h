#pragma once

/*
 * Linked list data structure for use in kernel code.
 */

struct list_node {
    struct list_node* prev;
    struct list_node* next;
};


struct list_header {
    struct list_node* next;
};
