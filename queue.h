#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

#define MAX_ID_LEN 32

/* Непрозрачный тип очереди, т.е. объявляю тип queue_t, не раскрывая его структуру.
* Поля array_queue_t и list_queue_t опрелены в queue.calloс
* При этом код, который подключает queue.h, видит лишь указатель queue_t* и может вызывать
* функции queue_create, queue_enqueue и т. д., но не знает, как устроена сама структура queue.
*/
typedef struct queue queue_t;

/*
 * Создаёт новую очередь.
 * - Если при компиляции указан флаг -DUSE_ARRAY_QUEUE, будет использоваться
 *   кольцевой буфер фиксированной ёмкости = capacity.
 * - Иначе — односвязная версия, без переполнения.
 * Возвращает NULL при ошибке malloc/инициализации.
 */
queue_t* queue_create(size_t capacity);

/** Освобождает все ресурсы очереди, включая строки с id. */
void queue_destroy(queue_t* q);

/*
 * Добавляет в конец очереди пассажира с идентификатором passenger_id
 * и временем обслуживания service_time.
 * Возвращает 0 при успехе, -1 при ошибке (переполнение/malloc).
 */
int queue_enqueue(queue_t* q, const char* passenger_id, int service_time);

/*
 * Возвращает указатель на строку с id первого пассажира (или NULL, если пусто).
 * Строку нельзя освобождать извне — она принадлежит очереди.
 */
const char* queue_front_id(const queue_t* q);

/*
 * Возвращает время обслуживания первого пассажира (или -1, если пустая).
 */
int queue_front_service_time(const queue_t* q);

/*
 * Удаляет из очереди первого пассажира.
 * Возвращает 0 при успехе, -1 если очередь пуста.
 */
int queue_dequeue(queue_t* q);

/* Возвращает 1, если очередь пуста, иначе 0. */
int queue_empty(const queue_t* q);

/* Возвращает число элементов в очереди. */
size_t queue_size(const queue_t* q);

/*
 * Копирует id всех пассажиров из очереди в массив out (каждая строка —
 * не длиннее MAX_ID_LEN). Возвращает число элементов.
 * Не изменяет состояние очереди.
 */
size_t queue_dump_ids(const queue_t* q, char out[][MAX_ID_LEN]);

/*
 * Запускает всю симуляцию:
 * - Читает из stdin сначала целое N (число стоек),
 * - Затем строки вида id/ta/ts до EOF,
 * - Моделирует Power of Two Choices,
 * - Печатает в stdout табличный вывод или сообщения об ошибках в stderr.
 */
void run_simulation(void);

#endif // QUEUE_H
