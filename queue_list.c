#include <stdlib.h>
#include <string.h>
#include "queue.h"

#ifndef USE_ARRAY_QUEUE

/**
 * Очередь на основе односвязного списка.
 */
typedef struct node {
    char*         id;            // строка-идентификатор пассажира
    int           service_time;  // время обслуживания пассажира
    struct node*  next;          // указатель на следующий узел
} node_t;

typedef struct {
    node_t* head; // первый в очереди
    node_t* tail; // последний в очереди
    size_t  size; // текущее число пассажиров
} list_queue_t;

/* Инициализация пустой очереди */
static int list_queue_init(list_queue_t* q) {
    q->head = q->tail = NULL;
    q->size = 0;
    return 0;
}

/* Освобождение памяти: удаление всех узлов и их ID */
static void list_queue_destroy(list_queue_t* q) {
    node_t* cur = q->head;
    while (cur) {
        node_t* tmp = cur->next;
        free(cur->id);  // освобождаем строку ID
        free(cur);      // освобождаем узел
        cur = tmp;
    }
}

/* Добавление пассажира в конец списка */
static int list_queue_enqueue(list_queue_t* q, const char* passenger_id, int service_time) {
    node_t* nd = malloc(sizeof(node_t));
    if (!nd) return -1;
    nd->id = malloc(strlen(passenger_id) + 1); // копируем ID
    if (!nd->id) { free(nd); return -1; }
    strcpy(nd->id, passenger_id);
    nd->service_time = service_time;
    nd->next = NULL;
    if (q->size == 0) {
        q->head = q->tail = nd; // первая запись
    } else {
        q->tail->next = nd;      // вешаем в конец
        q->tail = nd;
    }
    q->size++;
    return 0;
}

/* Получение ID первого пассажира или NULL, если очередь пуста */
static const char* list_queue_front_id(const list_queue_t* q) {
    return q->size ? q->head->id : NULL;
}

/* Получение времени обслуживания первого пассажира или -1 */
static int list_queue_front_service_time(const list_queue_t* q) {
    return q->size ? q->head->service_time : -1;
}

/* Удаление первого узла из списка */
static int list_queue_dequeue(list_queue_t* q) {
    if (!q->size) return -1; // пусто
    node_t* tmp = q->head;
    q->head = tmp->next;
    if (!q->head) q->tail = NULL; // очередь опустела
    free(tmp->id); // освобождаем ID
    free(tmp);     // освобождаем узел
    q->size--;
    return 0;
}

/* Проверка на пустоту очереди */
static int list_queue_empty(const list_queue_t* q) {
    return q->size == 0;
}

/* Текущее число элементов в очереди */
static size_t list_queue_size(const list_queue_t* q) {
    return q->size;
}

/* Копирование всех ID в массив out; возвращает число элементов */
static size_t list_queue_dump_ids(const list_queue_t* q, char out[][MAX_ID_LEN]) {
    size_t cnt = 0;
    for (node_t* cur = q->head; cur; cur = cur->next) {
        strncpy(out[cnt], cur->id, MAX_ID_LEN - 1);
        out[cnt][MAX_ID_LEN - 1] = '\0';
        cnt++;
    }
    return cnt;
}

#endif // USE_ARRAY_QUEUE
