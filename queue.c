#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "queue.h"

#include "queue_array.c"
#include "queue_list.c"

#define MAX_PASSENGERS 1000
#define INF_TIME 1000000000



// ----- Структуры и функции для симуляции ----- //

// Структура для хранения данных одного пассажира
typedef struct {
    char id[MAX_ID_LEN];
    int  ta;  // время прибытия
    int  ts;  // время обслуживания
} passenger_t;

/*
 * Компаратор для qsort: сравнивает по полю ta пассажиров по времени прибытия.
 */
static int cmp_arr(const void* a, const void* b) {
    const passenger_t* pa = (const passenger_t*)a;
    const passenger_t* pb = (const passenger_t*)b;
    return pa->ta - pb->ta;
}

/*
 * Проверяет, что все очереди в desks[0..N-1] пусты.
 * Возвращает 1, если все пусты, иначе 0.
 */
static int all_empty(queue_t** desks, int N) {
    for (int i = 0; i < N; i++) {
        if (!queue_empty(desks[i])) return 0;
    }
    return 1;
}




// ----- Реализация очереди queue_t с использованием array_queue_t или list_queue_t ----- //
// В зависимости от флага USE_ARRAY_QUEUE, будет использоваться либо массив, либо связный список.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "queue.h"

#define MAX_PASSENGERS 1000
#define INF_TIME 1000000000

#ifdef USE_ARRAY_QUEUE
extern int    array_queue_init(array_queue_t*, size_t);
extern void   array_queue_destroy(array_queue_t*);
extern int    array_queue_enqueue(array_queue_t*, const char*, int);
extern const char* array_queue_front_id(const array_queue_t*);
extern int    array_queue_front_service_time(const array_queue_t*);
extern int    array_queue_dequeue(array_queue_t*);
extern int    array_queue_empty(const array_queue_t*);
extern size_t array_queue_size(const array_queue_t*);
extern size_t array_queue_dump_ids(const array_queue_t*, char[][MAX_ID_LEN]);
#else
extern int    list_queue_init(list_queue_t*);
extern void   list_queue_destroy(list_queue_t*);
extern int    list_queue_enqueue(list_queue_t*, const char*, int);
extern const char* list_queue_front_id(const list_queue_t*);
extern int    list_queue_front_service_time(const list_queue_t*);
extern int    list_queue_dequeue(list_queue_t*);
extern int    list_queue_empty(const list_queue_t*);
extern size_t list_queue_size(const list_queue_t*);
extern size_t list_queue_dump_ids(const list_queue_t*, char[][MAX_ID_LEN]);
#endif

struct queue {
#ifdef USE_ARRAY_QUEUE
    array_queue_t impl;
#else
    list_queue_t  impl;
#endif
};

queue_t* queue_create(size_t capacity) {
    queue_t* q = malloc(sizeof(queue_t));
    if (!q) return NULL;
#ifdef USE_ARRAY_QUEUE
    if (array_queue_init(&q->impl, capacity) < 0) { free(q); return NULL; }
#else
    if (list_queue_init(&q->impl) < 0)        { free(q); return NULL; }
#endif
    return q;
}

void queue_destroy(queue_t* q) {
    if (!q) return;
#ifdef USE_ARRAY_QUEUE
    array_queue_destroy(&q->impl);
#else
    list_queue_destroy(&q->impl);
#endif
    free(q);
}

int queue_enqueue(queue_t* q, const char* id, int ts) {
#ifdef USE_ARRAY_QUEUE
    return array_queue_enqueue(&q->impl, id, ts);
#else
    return list_queue_enqueue(&q->impl, id, ts);
#endif
}

const char* queue_front_id(const queue_t* q) {
#ifdef USE_ARRAY_QUEUE
    return array_queue_front_id(&q->impl);
#else
    return list_queue_front_id(&q->impl);
#endif
}

int queue_front_service_time(const queue_t* q) {
#ifdef USE_ARRAY_QUEUE
    return array_queue_front_service_time(&q->impl);
#else
    return list_queue_front_service_time(&q->impl);
#endif
}

int queue_dequeue(queue_t* q) {
#ifdef USE_ARRAY_QUEUE
    return array_queue_dequeue(&q->impl);
#else
    return list_queue_dequeue(&q->impl);
#endif
}

int queue_empty(const queue_t* q) {
#ifdef USE_ARRAY_QUEUE
    return array_queue_empty(&q->impl);
#else
    return list_queue_empty(&q->impl);
#endif
}

size_t queue_size(const queue_t* q) {
#ifdef USE_ARRAY_QUEUE
    return array_queue_size(&q->impl);
#else
    return list_queue_size(&q->impl);
#endif
}

size_t queue_dump_ids(const queue_t* q, char out[][MAX_ID_LEN]) {
#ifdef USE_ARRAY_QUEUE
    return array_queue_dump_ids(&q->impl, out);
#else
    return list_queue_dump_ids(&q->impl, out);
#endif
}



 // ----- SIMULATION SIMULATION SIMULATION SIMULATION SIMULATION SIMULATION SIMULATION SIMULATION ----- //

void run_simulation(void) {
    int N;
    if (scanf("%d", &N) != 1) {
        fprintf(stderr, "Error: failed to read number of desks\n");
        return;
    }
    if (N < 2) {
        fprintf(stderr, "Error: at least 2 desks are required, but N=%d\n", N);
        return;
    }

    // 1) Создаём N очередей
    queue_t** desks = malloc(N * sizeof(queue_t*));
    if (!desks) {
        fprintf(stderr, "Error: malloc failed for desks array\n");
        return;
    }
    for (int i = 0; i < N; i++) {
        desks[i] = queue_create(MAX_PASSENGERS);
        if (!desks[i]) {
            fprintf(stderr, "Error: failed to create queue %d\n", i);
            for (int k = 0; k < i; k++) queue_destroy(desks[k]);
            free(desks);
            return;
        }
    }

    // 2) Читаем пассажиров из stdin (id/ta/ts) до EOF
    passenger_t arrivals[MAX_PASSENGERS];
    int total = 0;
    char buf[64];
    while (total < MAX_PASSENGERS && scanf("%63s", buf) == 1) {
        // buf = "id/ta/ts"
        char* tok = strtok(buf, "/");
        if (!tok) continue;
        strncpy(arrivals[total].id, tok, MAX_ID_LEN - 1);
        arrivals[total].id[MAX_ID_LEN - 1] = '\0';

        tok = strtok(NULL, "/");
        if (!tok) continue;
        arrivals[total].ta = atoi(tok);

        tok = strtok(NULL, "/");
        if (!tok) continue;
        arrivals[total].ts = atoi(tok);

        total++;
    }

    // 3) Сортируем arrivals по возрастанию ta
    qsort(arrivals, total, sizeof(passenger_t), cmp_arr);

    // 4) next_finish[i] = INF (пока никто не обслуживается)
    int* next_finish = malloc(N * sizeof(int));
    if (!next_finish) {
        fprintf(stderr, "Error: malloc failed for next_finish array\n");
        for (int i = 0; i < N; i++) queue_destroy(desks[i]);
        free(desks);
        return;
    }
    for (int i = 0; i < N; i++) {
        next_finish[i] = INF_TIME;
    }

    // 5) Массивы для моментов времени и снимков очередей
    int times[MAX_PASSENGERS * 2];
    int times_count = 0;

    char*** snapshots = malloc(N * sizeof(char**));
    size_t* snapcount = malloc(N * sizeof(size_t));
    if (!snapshots || !snapcount) {
        fprintf(stderr, "Error: malloc failed for snapshots structures\n");
        free(next_finish);
        for (int i = 0; i < N; i++) queue_destroy(desks[i]);
        free(desks);
        return;
    }
    for (int i = 0; i < N; i++) {
        snapcount[i] = 0;
        snapshots[i] = malloc((MAX_PASSENGERS * 2) * sizeof(char*));
        if (!snapshots[i]) {
            fprintf(stderr, "Error: malloc failed for snapshots[%d]\n", i);
            for (int k = 0; k < i; k++) free(snapshots[k]);
            free(snapshots);
            free(snapcount);
            free(next_finish);
            for (int k = 0; k < N; k++) queue_destroy(desks[k]);
            free(desks);
            return;
        }
    }

    int i_arr = 0;

    // ─── Сохраняем состояние в момент 0 ───
    times[times_count++] = 0;
    for (int i = 0; i < N; i++) {
        char* snapshot = malloc(2);
        if (!snapshot) {
            fprintf(stderr, "Error: malloc failed for initial snapshot\n");
            return;
        }
        snapshot[0] = '-';
        snapshot[1] = '\0';
        snapshots[i][snapcount[i]++] = snapshot;
    }

    // ─── Цикл обработки событий ───
    while (i_arr < total || !all_empty(desks, N)) {
        int time_next_arr = (i_arr < total ? arrivals[i_arr].ta : INF_TIME);
        int time_next_fin = INF_TIME;
        for (int j = 0; j < N; j++) {
            if (next_finish[j] < time_next_fin) {
                time_next_fin = next_finish[j];
            }
        }
        int t = (time_next_arr < time_next_fin ? time_next_arr : time_next_fin);

        int changed = 0;

        // 6) Завершения в момент t
        for (int j = 0; j < N; j++) {
            if (next_finish[j] == t) {
                queue_dequeue(desks[j]);
                if (queue_empty(desks[j])) {
                    next_finish[j] = INF_TIME;
                } else {
                    int s = queue_front_service_time(desks[j]);
                    next_finish[j] = t + s;
                }
                changed = 1;
            }
        }

        // 7) Приходы в момент t
        while (i_arr < total && arrivals[i_arr].ta == t) {
            passenger_t* p = &arrivals[i_arr++];
            int x = rand() % N;
            int y;
            do {
                y = rand() % N;
            } while (y == x);
            int chosen = (queue_size(desks[x]) <= queue_size(desks[y]) ? x : y);
            queue_enqueue(desks[chosen], p->id, p->ts);
            if (queue_size(desks[chosen]) == 1) {
                next_finish[chosen] = t + p->ts;
            }
            changed = 1;
        }

        // 8) Если что-то изменилось, сохраняем t и снимки N очередей
        if (changed) {
            times[times_count++] = t;
            for (int i = 0; i < N; i++) {
                // Копируем все id-ы из текущей очереди во временный буфер
                char temp_ids[MAX_PASSENGERS][MAX_ID_LEN];
                size_t cnt = queue_dump_ids(desks[i], temp_ids);

                // Вычисляем, сколько байт нужно выделить
                size_t needed;
                if (cnt == 0) {
                    // Пустая очередь => строка "-" и '\0'
                    needed = 2;
                } else {
                    // Сумма длин всех id + пробелы + '\0'
                    needed = 1;  // для завершающего '\0'
                    for (size_t k = 0; k < cnt; k++) {
                        needed += strlen(temp_ids[k]) + 1;
                    }
                }

                char* snapshot = malloc(needed);
                if (!snapshot) {
                    // Если malloc провалился, гарантированно выделяем 2 байта и ставим "-"
                    snapshot = malloc(2);
                    if (!snapshot) {
                        fprintf(stderr, "Error: malloc failed for snapshot\n");
                        return;
                    }
                    snapshot[0] = '-';
                    snapshot[1] = '\0';
                } else {
                    if (cnt == 0) {
                        // Если очередь пуста, строка = "-"
                        snapshot[0] = '-';
                        snapshot[1] = '\0';
                    } else {
                        // Собираем строку из id через пробел
                        snapshot[0] = '\0';
                        for (size_t k = 0; k < cnt; k++) {
                            strcat(snapshot, temp_ids[k]);
                            if (k + 1 < cnt) strcat(snapshot, " ");
                        }
                    }
                }
                snapshots[i][snapcount[i]++] = snapshot;
            }
        }
    }

    // ─── Форматированный вывод ───

    // 1) label_width = max длина "№X" + 2 пробела
    int label_width = 0;
    for (int i = 1; i <= N; i++) {
        char tmp[16];
        int len = snprintf(tmp, sizeof(tmp), "№%d", i);
        if (len > label_width) label_width = len;
    }
    label_width += 2;

    // 2) col_width = максимум из:
    //    - длина times[k] как строки
    //    - длина snapshot[i][k]
    //    плюс 2 пробела
    int col_width = 0;
    for (int k = 0; k < times_count; k++) {
        char tmp[32];
        int len = snprintf(tmp, sizeof(tmp), "%d", times[k]);
        if (len > col_width) col_width = len;
    }
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < snapcount[i]; k++) {
            int len = (int)strlen(snapshots[i][k]);
            if (len > col_width) col_width = len;
        }
    }
    col_width += 2;

    // Первая строка: отступ label_width - 2, потом все times[k]
    for (int i = 0; i < label_width - 2; i++) putchar(' ');
    for (int k = 0; k < times_count; k++) {
        printf("%-*d", col_width, times[k]);
    }
    putchar('\n');

    // Далее N строк: "№i" + состояние очереди i во все моменты
    for (int i = 0; i < N; i++) {
        char label[16];
        snprintf(label, sizeof(label), "№%d", i + 1);
        printf("%-*s", label_width, label);
        for (int k = 0; k < snapcount[i]; k++) {
            printf("%-*s", col_width, snapshots[i][k]);
        }
        putchar('\n');
    }

    // Освобождаем всё
    for (int i = 0; i < N; i++) {
        for (size_t k = 0; k < snapcount[i]; k++) {
            free(snapshots[i][k]);
        }
        free(snapshots[i]);
        queue_destroy(desks[i]);
    }
    free(snapshots);
    free(snapcount);
    free(next_finish);
    free(desks);
}
