#include <pthread.h>
#include <threadpool.h>

__thread task_t task;

void *thread_function(void *arg) {
  threadpool_t *pool = (threadpool_t*)arg;

  while(!pool->stop) {
    pthread_cond_wait(&(pool->notify), &(pool->lock));

    task = pool->tasks_queue[pool->queue_front];
    pool->queue_front += 1;

    pthread_mutex_unlock(&(pool->lock));

    task.n(task.arg);
  }

  return NULL;
}

void threadpool_init(threadpool_t *pool) {
  pool->queue_front = 0;
  pool->queue_back = 0;
  pool->stop = false;

  pthread_mutex_init(&(pool->lock), NULL);
  pthread_cond_init(&(pool->notify), NULL);

  for (int i = 0; i < THREADS; i++) {
    pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
  }
}

void threadpool_destroy(threadpool_t *pool) {
  for (int i = 0; i < THREADS; i++) {
    pthread_join(pool->threads[i], NULL);
  }

  pthread_cond_destroy(&(pool->notify));
  pthread_mutex_destroy(&(pool->lock));
}

void threadpool_add_task(threadpool_t* pool, void (*function)(void*), void* arg) {
  pthread_mutex_lock(&(pool->lock));

  int next_item = (pool->queue_back + 1) % QUEUE_SIZE;
  if (next_item == pool->queue_front) {
    pthread_mutex_unlock(&(pool->lock));
    return;
  }

  pool->tasks_queue[pool->queue_back].n = function;
  pool->tasks_queue[pool->queue_back].arg = arg;
  pool->queue_back = next_item;

  pthread_mutex_unlock(&(pool->lock));

  pthread_cond_signal(&(pool->notify));
}
