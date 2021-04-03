// #include <stddef.h>

typedef struct LIST_T {
    struct LIST_T *prev;
    struct LIST_T *next;
    char data[];
} list_t;

#ifndef LIST_MALLOC
#define LIST_MALLOC(type) malloc(sizeof(list_t) + sizeof(type));
#endif

#ifndef LIST_GET_DATA
#define LIST_GET_DATA(dst, src) \
    if (src != NULL) { \
        dst = (typeof(dst) *)(src->data); \
    }
#endif

static inline int list_init(list_t *head)
{
    if (head == NULL) {
        return -1;
    }
    head->next = head;
    head->prev = head;
    return 0;
}

static inline int is_list_empty(list_t *head)
{
    if (head == NULL) {
        return -1;
    }
    return head->next == head;
}

static inline int list_push_back(list_t *head, list_t *node)
{
    if (head == NULL) {
        return -1;
    }
    if (node == NULL) {
        return -1;
    }

    node->prev = head->prev;
    node->next = head;
    head->prev->next = node;
    head->prev = node;
    return 0;
}

static inline int list_remove_tail(list_t *head) {
    if (head == NULL) {
        return -1;
    }
    list_t *target = head->prev;

    target->prev->next = head;
    head->prev = target->prev;
    free(target);
    return 0;
}

/* IF the current node is the last one, the function will malloc a new node and push it to the list and return it*/
static inline list_t *list_get_next(list_t *head, list_t *current, unsigned long type_size)
{
    if (head == NULL) {
        return NULL;
    }

    // If the current is the tail, then push a new node to the list and return
    if (current->next == head) {
        list_t *node = malloc(sizeof(list_t) + type_size);
        if (list_push_back(head, node) != 0) {
            return NULL;
        }
    }

    return current->next;
}