#include <stdio.h>
#include <unistd.h>
#include <threadpool.h>

void example_task(void *arg) {
  printf("Hi\n");
}

int main(int argc, char **argv) {
  threadpool_t pool;

  threadpool_init(&pool);
  sleep(1);
  threadpool_add_task(&pool, &example_task, NULL);

  threadpool_destroy(&pool);
}
