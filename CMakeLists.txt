cmake_minimum_required(VERSION 3.10)
project(queue_lib C)

# опция выбора реализации - вектор или список
option(USE_ARRAY_QUEUE "Use array-based queue implementation" OFF)
# опция сборки - shared или static
option(BUILD_SHARED_LIBS "Build library as shared" OFF)

# подключаем наш исходник и дефайн
if(USE_ARRAY_QUEUE)
    add_definitions(-DUSE_ARRAY_QUEUE)
    set(QUEUE_IMPL queue_array.c) # в QUEUE_IMPL запишется имя файла queue_array.c
else()
    set(QUEUE_IMPL queue_list.c)
endif()

# по итогу тут все исходники очереди:
set(QUEUE_SRCS
    queue.c
    ${QUEUE_IMPL}
)

# создаём библиотеку, тип которой определяется BUILD_SHARED_LIBS
add_library(queue ${QUEUE_SRCS})
target_include_directories(queue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


