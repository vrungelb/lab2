cmake_minimum_required(VERSION 3.10)
project(queue_lib C)

# Опции
option(USE_ARRAY_QUEUE "Use array-based queue implementation" OFF)
option(BUILD_SHARED_LIBS "Build library as shared" OFF)

# Список исходников библиотеки queue.
# queue.c содержит обёртки queue_t, а файлы impl (array/list) определяют static-функции.
# Поскольку реализация защищена директивами #ifdef USE_ARRAY_QUEUE, можно всегда компилировать оба impl-файла:
set(QUEUE_SRCS
    queue.c
    queue_array.c
    queue_list.c
)

# Создаём библиотеку queue: STATIC или SHARED в зависимости от BUILD_SHARED_LIBS
if(BUILD_SHARED_LIBS)
    add_library(queue SHARED ${QUEUE_SRCS})
else()
    add_library(queue STATIC ${QUEUE_SRCS})
endif()

# Заголовки
target_include_directories(queue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Передаём дефайн USE_ARRAY_QUEUE, если опция включена
if(USE_ARRAY_QUEUE)
    target_compile_definitions(queue PRIVATE USE_ARRAY_QUEUE)
endif()

# Назовём исполняемый файл queue_app.
add_executable(queue_app main.c)

# Пути к заголовкам (текущий каталог поставим, чтобы попроще было)
target_include_directories(queue_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Линкуем с нашей библиотекой queue
target_link_libraries(queue_app PRIVATE queue)

# Чтобы при запуске исполняемого рядом искалась shared-библиотека:
if(BUILD_SHARED_LIBS)
    # Устанавливаем RPATH на $ORIGIN (текущий каталог с бинарником)
    set_target_properties(queue_app PROPERTIES
        INSTALL_RPATH "$ORIGIN"
    )
endif()
