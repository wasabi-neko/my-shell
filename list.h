// #include <stddef.h>
#ifndef LIST_HEADER
#define LIST_HEADER

typedef struct LIST_T {
    struct LIST_T *prev;
    struct LIST_T *next;
    char data[];
} list_t;

#ifndef LIST_NEW_NODE
#define LIST_NEW_NODE(type) \
    _new_list_node_type(sizeof(type))
#endif

#ifndef LIST_GET_DATA
#define LIST_GET_DATA(dst, src) \
    if (src != NULL) { \
        dst = (typeof(dst) *)(src->data); \
    }
#endif

#ifndef LIST_DATA_PTR
#define LIST_DATA_PTR(head, type) ((type *)head->data)
#endif

#ifndef LIST_FOREACH
#define LIST_FOREACH(ptr, head) \
    for (list_t *ptr = head->next; ptr != head; ptr = ptr->next)
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

static inline list_t *_new_list_node_type(unsigned long type_size) {
    list_t *tmp = malloc(sizeof(list_t) + type_size);
    if (tmp == NULL) {
        perror("malloc failed");
        return NULL;
    }
    if (list_init(tmp) != 0) {
        return NULL;
    }
    return tmp;
}

static inline list_t *new_list(void)
{
    list_t *target = malloc(sizeof(list_t));
    if (list_init(target) != 0) {
        return NULL;
    }

    return target;
}

static inline int is_list_init(list_t *head)
{
    return head->next != NULL && head->prev != NULL;
}

static inline int is_list_empty(list_t *head)
{
    if (head == NULL) {
        head = malloc(sizeof(list_t));
        if (head == NULL) {
            return -1;
        }
    }
    return head->next == head;
}

/**
 * Free The from node; but the node to will not be freed
 */
static inline int list_free(list_t *from, list_t *to, void (*free_func_ptr)(void*))
{
    if (from == to) {
        // Nothing to free
        return 0;
    }

    if (from == NULL || to == NULL) {
        return -1;
    }

    list_t *from_prev = from->prev;
    list_t *cur = from;
    while (cur != NULL && cur != to) {
        cur = cur->next;
        if (free_func_ptr == NULL) {
            free(cur->prev);
        } else {
            /* free the pointer inside the data buf */
            free_func_ptr(cur->prev->data);
            free(cur->prev);
        }
    }

    if (cur == NULL) {
        return -1;
    }

    from_prev->next = to;
    to->prev = from_prev;
    return 0;
}

static inline int list_push_back(list_t *head, list_t *node)
{
    if (head == NULL || !is_list_init(head)) {
        perror("head failed");
        return -1;
    }
    if (node == NULL) {
        perror("node is NULL");
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
        list_t *node = _new_list_node_type(type_size);
        if (node == NULL) {
            return NULL;
        }

        if (list_push_back(head, node) != 0) {
            return NULL;
        }
    }

    return current->next;
}

#endif