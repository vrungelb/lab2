#include <stdlib.h>
#include <string.h>
#include "queue.h"

#ifndef USE_ARRAY_QUEUE

typedef struct node {
    char*         id;
    int           service_time;
    struct node*  next;
} node_t;

typedef struct {
    node_t* head;
    node_t* tail;
    size_t  size;
} list_queue_t;

static int list_queue_init(list_queue_t* q) {
    q->head = q->tail = NULL;
    q->size = 0;
    return 0;
}

static void list_queue_destroy(list_queue_t* q) {
    node_t* cur = q->head;
    while (cur) {
        node_t* tmp = cur->next;
        free(cur->id);
        free(cur);
        cur = tmp;
    }
}

static int list_queue_enqueue(list_queue_t* q, const char* passenger_id, int service_time) {
    node_t* nd = malloc(sizeof(node_t));
    if (!nd) return -1;
    nd->id = malloc(strlen(passenger_id) + 1);
    if (!nd->id) { free(nd); return -1; }
    strcpy(nd->id, passenger_id);
    nd->service_time = service_time;
    nd->next = NULL;
    if (q->size == 0) {
        q->head = q->tail = nd;
    } else {
        q->tail->next = nd;
        q->tail = nd;
    }
    q->size++;
    return 0;
}

static const char* list_queue_front_id(const list_queue_t* q) {
    return q->size ? q->head->id : NULL;
}

static int list_queue_front_service_time(const list_queue_t* q) {
    return q->size ? q->head->service_time : -1;
}

static int list_queue_dequeue(list_queue_t* q) {
    if (!q->size) return -1;
    node_t* tmp = q->head;
    q->head = tmp->next;
    if (!q->head) q->tail = NULL;
    free(tmp->id);
    free(tmp);
    q->size--;
    return 0;
}

static int list_queue_empty(const list_queue_t* q) {
    return q->size == 0;
}

static size_t list_queue_size(const list_queue_t* q) {
    return q->size;
}

static size_t list_queue_dump_ids(const list_queue_t* q, char out[][MAX_ID_LEN]) {
    size_t cnt = 0;
    for (node_t* cur = q->head; cur; cur = cur->next) {
        strncpy(out[cnt], cur->id, MAX_ID_LEN-1);
        out[cnt][MAX_ID_LEN-1] = '\0';
        cnt++;
    }
    return cnt;
}

#endif // !USE_ARRAY_QUEUE
