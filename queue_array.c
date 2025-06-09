#include <stdlib.h>
#include <string.h>
#include "queue.h"

#ifdef USE_ARRAY_QUEUE

/**
 * Очередь на основе кольцевого буфера с буфером отказанных пассажиров.
 */
typedef struct {
    char**  data;       // массив строк-идентификаторов пассажиров
    int*    times;      // массив времен обслуживания пассажиров
    size_t  capacity;   // максимальная ёмкость буфера
    size_t  head;       // индекс первого элемента в буфере
    size_t  tail;       // индекс для следующего элемента
    size_t  size;       // текущее число элементов

    char**  dropped;    // массив ID пассажиров, не поместившихся в очередь
    size_t  drop_size;  // число отказанных пассажиров
    size_t  drop_cap;   // ёмкость массива dropped
} array_queue_t;

/**
 * Инициализация очереди и буфера отказанных
 * - выделяет основной буфер data/times для capacity элементов
 * - создаёт буфер dropped для хранения отказанных пассажиров
 */
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

    q->drop_cap = capacity / 2 + 1;  // начальный размер буфера отказанных
    q->dropped = malloc(q->drop_cap * sizeof(char*));
    if (!q->dropped) {
        free(q->data);
        free(q->times);
        return -1;
    }
    q->drop_size = 0;
    return 0;
}

/**
 * Освобождение ресурсов очереди и буфера отказанных
 * - очищает и освобождает все строки в data и dropped
 * - освобождает сами массивы
 */
static void array_queue_destroy(array_queue_t* q) {
    // основной буфер
    for (size_t i = 0; i < q->size; i++) {
        free(q->data[(q->head + i) % q->capacity]);
    }
    free(q->data);
    free(q->times);
    // буфер отказанных
    for (size_t i = 0; i < q->drop_size; i++) {
        free(q->dropped[i]);
    }
    free(q->dropped);
}

/**
 * Добавление пассажира:
 * - если в основной очереди есть место, копирует ID и ts в data/times
 * - иначе сохраняет копию ID в dropped для последующей обработки
 * Возвращает 0 при успешном enqueue, -1 при переполнении или ошибке malloc.
 */
static int array_queue_enqueue(array_queue_t* q, const char* passenger_id, int service_time) {
    if (q->size == q->capacity) {
        // основной буфер полон -> сохраняем в dropped
        if (q->drop_size == q->drop_cap) {
            size_t newcap = q->drop_cap * 2;
            char** tmp = realloc(q->dropped, newcap * sizeof(char*));
            if (!tmp) return -1;
            q->dropped = tmp;
            q->drop_cap = newcap;
        }
        q->dropped[q->drop_size] = malloc(strlen(passenger_id) + 1);
        if (!q->dropped[q->drop_size]) return -1;
        strcpy(q->dropped[q->drop_size], passenger_id);
        q->drop_size++;
        return -1;
    }
    // enqueue в основной буфер
    char* copy = malloc(strlen(passenger_id) + 1);
    if (!copy) return -1;
    strcpy(copy, passenger_id);
    q->data[q->tail] = copy;
    q->times[q->tail] = service_time;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
    return 0;
}

/**
 * Получение ID первого пассажира
 * Возвращает NULL, если очередь пуста
 */
static const char* array_queue_front_id(const array_queue_t* q) {
    return q->size ? q->data[q->head] : NULL;
}

/**
 * Получение времени обслуживания первого пассажира
 * Возвращает -1, если очередь пуста
 */
static int array_queue_front_service_time(const array_queue_t* q) {
    return q->size ? q->times[q->head] : -1;
}

/**
 * Удаление первого пассажира из очереди
 * - освобождает строку ID
 * - сдвигает head и уменьшает размер
 * Возвращает 0 при успехе, -1 если очередь пуста
 */
static int array_queue_dequeue(array_queue_t* q) {
    if (!q->size) return -1;
    free(q->data[q->head]);
    q->head = (q->head + 1) % q->capacity;
    q->size--;
    return 0;
}

/**
 * Проверка пустоты очереди
 * Возвращает 1 если size==0, иначе 0
 */
static int array_queue_empty(const array_queue_t* q) {
    return q->size == 0;
}

/**
 * Получение текущего числа элементов в очереди
 */
static size_t array_queue_size(const array_queue_t* q) {
    return q->size;
}

/**
 * Копирование всех ID из очереди в массив out
 * Используется для формирования снимков состояния
 * Возвращает число скопированных элементов
 */
static size_t array_queue_dump_ids(const array_queue_t* q, char out[][MAX_ID_LEN]) {
    size_t cnt = q->size;
    for (size_t i = 0; i < cnt; i++) {
        size_t idx = (q->head + i) % q->capacity;
        strncpy(out[i], q->data[idx], MAX_ID_LEN - 1);
        out[i][MAX_ID_LEN - 1] = '\0';
    }
    return cnt;
}

/**
 * Обработка ранее отказанных пассажиров (dropped):
 * проходит по всему dropped и пытается enqueue в основной буфер
 * - вставляет, если есть место, и освобождает dropped[i]
 * - иначе сохраняет для следующей попытки
 */
static void array_queue_process_dropped(array_queue_t* q) {
    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < q->drop_size; ++read_idx) {
        char* pid = q->dropped[read_idx];
        if (q->size < q->capacity) {
            // вставляем из dropped в очередь
            char* copy = malloc(strlen(pid) + 1);
            strcpy(copy, pid);
            q->data[q->tail] = copy;
            q->times[q->tail] = 0;  // время обслуживания хранить отдельно при необходимости
            q->tail = (q->tail + 1) % q->capacity;
            q->size++;
            free(pid);
        } else {
            // оставляем в dropped для следующих попыток
            q->dropped[write_idx++] = pid;
        }
    }
    q->drop_size = write_idx;
}

#endif // USE_ARRAY_QUEUE
