#include <pthread.h>
#include "../inc/threadpool.h"

void *thread_function(void *arg) {
  threadpool_t *pool = (threadpool_t*)arg;

  while(1) {
    pthread_mutex_lock(&(pool->lock));

    while (!pool->stop && pool->queued == 0) {
      pthread_cond_wait(&(pool->notify), &(pool->lock));
    }

    if (pool->stop && pool->queued == 0) {
      pthread_mutex_unlock(&(pool->lock));
      return NULL;
    }

    task_t task = pool->tasks_queue[pool->queue_front];
    pool->queue_front = (pool->queue_front + 1) % QUEUE_SIZE;
    pool->queued -= 1;

    pthread_mutex_unlock(&(pool->lock));

    task.n(task.arg);
  }

  return NULL;
}

void threadpool_init(threadpool_t *pool) {
  if (pool == NULL) {
    return;
  }

  pool->queued = 0;
  pool->queue_front = 0;
  pool->queue_back = 0;
  pool->stop = 0;

  pthread_mutex_init(&(pool->lock), NULL);
  pthread_cond_init(&(pool->notify), NULL);

  for (int i = 0; i < THREADS; i++) {
    pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
  }
}

void threadpool_destroy(threadpool_t *pool) {
  if (pool == NULL) {
    return;
  }

  pthread_mutex_lock(&(pool->lock));
  pool->stop = 1;
  pthread_mutex_unlock(&(pool->lock));

  pthread_cond_broadcast(&(pool->notify));

  for (int i = 0; i < THREADS; i++) {
    pthread_join(pool->threads[i], NULL);
  }

  pthread_cond_destroy(&(pool->notify));
  pthread_mutex_destroy(&(pool->lock));
}

void threadpool_add_task(threadpool_t* pool, void (*function)(void*), void* arg) {
  if (pool == NULL || function == NULL) {
    return;
  }

  pthread_mutex_lock(&(pool->lock));

  if (pool->stop) {
    pthread_mutex_unlock(&(pool->lock));
    return;
  }

  if (pool->queued == QUEUE_SIZE) {
    pthread_mutex_unlock(&(pool->lock));
    return;
  }

  pool->tasks_queue[pool->queue_back].n = function;
  pool->tasks_queue[pool->queue_back].arg = arg;
  pool->queue_back = (pool->queue_back + 1) % QUEUE_SIZE;
  pool->queued += 1;

  pthread_mutex_unlock(&(pool->lock));

  pthread_cond_signal(&(pool->notify));
}
