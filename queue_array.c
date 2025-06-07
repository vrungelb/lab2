#include <stdlib.h>
#include <string.h>
#include "queue.h"

#ifdef USE_ARRAY_QUEUE

typedef struct {
    char**  data;
    int*    times;
    size_t  capacity;
    size_t  head, tail;
    size_t  size;
} array_queue_t;

static int array_queue_init(array_queue_t* q, size_t capacity) {
    q->data     = malloc(capacity * sizeof(char*));
    q->times    = malloc(capacity * sizeof(int));
    if (!q->data || !q->times) {
        free(q->data);
        free(q->times);
        return -1;
    }
    q->capacity = capacity;
    q->head = q->tail = q->size = 0;
    return 0;
}

static void array_queue_destroy(array_queue_t* q) {
    for (size_t i = 0; i < q->size; i++) {
        free(q->data[(q->head + i) % q->capacity]);
    }
    free(q->data);
    free(q->times);
}

static int array_queue_enqueue(array_queue_t* q, const char* passenger_id, int service_time) {
    if (q->size == q->capacity) return -1;
    char* copy = malloc(strlen(passenger_id) + 1);
    if (!copy) return -1;
    strcpy(copy, passenger_id);
    q->data[q->tail]  = copy;
    q->times[q->tail] = service_time;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
    return 0;
}

static const char* array_queue_front_id(const array_queue_t* q) {
    return q->size ? q->data[q->head] : NULL;
}

static int array_queue_front_service_time(const array_queue_t* q) {
    return q->size ? q->times[q->head] : -1;
}

static int array_queue_dequeue(array_queue_t* q) {
    if (!q->size) return -1;
    free(q->data[q->head]);
    q->head = (q->head + 1) % q->capacity;
    q->size--;
    return 0;
}

static int array_queue_empty(const array_queue_t* q) {
    return q->size == 0;
}

static size_t array_queue_size(const array_queue_t* q) {
    return q->size;
}

static size_t array_queue_dump_ids(const array_queue_t* q, char out[][MAX_ID_LEN]) {
    size_t cnt = q->size;
    for (size_t i = 0; i < cnt; i++) {
        size_t idx = (q->head + i) % q->capacity;
        strncpy(out[i], q->data[idx], MAX_ID_LEN-1);
        out[i][MAX_ID_LEN-1] = '\0';
    }
    return cnt;
}

#endif // USE_ARRAY_QUEUE
