#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <threadpool.h>

void example_task(void* arg) {
    int* num = (int*)arg;
    printf("Processing task %d\n", *num);
    sleep(1);  // Simulate task work
    free(arg);
}

int main(int argc, char **argv) {
  threadpool_t pool;

  threadpool_init(&pool);
  for (int i = 0; i < 15; i++) {
      int* task_num = malloc(sizeof(int));
      *task_num = i;
      threadpool_add_task(&pool, example_task, task_num);
  }

  threadpool_destroy(&pool);
}
