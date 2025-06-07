#include <stdlib.h>
#include <string.h>
#include "queue.h"

#ifdef USE_ARRAY_QUEUE

/**
 * Очередь на основе кольцевого буфера.
 */
typedef struct {
    char**  data;       // массив строк-идентификаторов пассажиров
    int*    times;      // массив времен обслуживания пассажиров
    size_t  capacity;   // максимальная ёмкость буфера
    size_t  head;       // индекс первого элемента в буфере
    size_t  tail;       // индекс, куда будет помещён следующий элемент
    size_t  size;       // текущее число элементов в очереди
} array_queue_t;

/* Инициализация очереди: выделение памяти под буфер и установки параметров */
static int array_queue_init(array_queue_t* q, size_t capacity) {
    q->data     = malloc(capacity * sizeof(char*)); // выделяем массив указателей на строки
    q->times    = malloc(capacity * sizeof(int));   // выделяем массив времен обслуживания
    if (!q->data || !q->times) {
        free(q->data);
        free(q->times);
        return -1;
    }
    q->capacity = capacity;
    q->head = q->tail = q->size = 0; // очередь пуста
    return 0;
}

/* Освобождение памяти: удаление всех строк и буферов */
static void array_queue_destroy(array_queue_t* q) {
    for (size_t i = 0; i < q->size; i++) {
        free(q->data[(q->head + i) % q->capacity]); // освобождаем каждую строку ID
    }
    free(q->data);  // освобождаем сам буфер ID
    free(q->times); // освобождаем буфер времен
}

/* Добавление пассажира в очередь: копирование ID и сохранение времени */
static int array_queue_enqueue(array_queue_t* q, const char* passenger_id, int service_time) {
    if (q->size == q->capacity) return -1; // буфер полон
    char* copy = malloc(strlen(passenger_id) + 1); // копируем строку ID
    if (!copy) return -1;
    strcpy(copy, passenger_id);
    q->data[q->tail]  = copy;
    q->times[q->tail] = service_time;
    q->tail = (q->tail + 1) % q->capacity; // кольцевой переход
    q->size++;
    return 0;
}

/* Получение ID первого пассажира или NULL, если очередь пуста */
static const char* array_queue_front_id(const array_queue_t* q) {
    return q->size ? q->data[q->head] : NULL;
}

/* Получение времени обслуживания первого пассажира или -1, если пусто */
static int array_queue_front_service_time(const array_queue_t* q) {
    return q->size ? q->times[q->head] : -1;
}

/* Удаление первого пассажира из очереди и освобождение его ID */
static int array_queue_dequeue(array_queue_t* q) {
    if (!q->size) return -1; // очередь пуста
    free(q->data[q->head]); // освобождаем строку ID
    q->head = (q->head + 1) % q->capacity; // смещение головы
    q->size--;
    return 0;
}

/* Проверка: пуста ли очередь (1 — пуста, 0 — нет) */
static int array_queue_empty(const array_queue_t* q) {
    return q->size == 0;
}

/* Текущее число элементов в очереди */
static size_t array_queue_size(const array_queue_t* q) {
    return q->size;
}

/* Копирование всех ID в переданный массив out; возвращает количество элементов */
static size_t array_queue_dump_ids(const array_queue_t* q, char out[][MAX_ID_LEN]) {
    size_t cnt = q->size;
    for (size_t i = 0; i < cnt; i++) {
        size_t idx = (q->head + i) % q->capacity; // вычисляем индекс в буфере
        strncpy(out[i], q->data[idx], MAX_ID_LEN - 1);
        out[i][MAX_ID_LEN - 1] = '\0';
    }
    return cnt;
}

#endif // USE_ARRAY_QUEUE
