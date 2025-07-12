#include "../kernel/types.h"
#include "../user/user.h"


#define STACK_BYTES 4096
#define STACK_SIZE (STACK_BYTES / sizeof(int))
#define MAX_LOG_LINES 512
#define MAX_LINE_LENGTH 64

volatile int print_lock = 0;

char log_buffer[MAX_LOG_LINES][MAX_LINE_LENGTH];
int log_index = 0;

void lock() {
  while (__sync_lock_test_and_set(&print_lock, 1)) {
    // busy wait
  }
}

void unlock() {
  __sync_lock_release(&print_lock);
}


char* strcat(char *dest, const char *src) {
  char *d = dest + strlen(dest);
  while (*src) {
    *d++ = *src++;
  }
  *d = '\0';
  return dest;
}

void *my_thread(void *arg) {
  uint64 number = (uint64) arg;
  for (int i = 0; i < 100; ++i) {
    number++;

    char numbuf[32];
    uint64 n = number;
    int len = 0;

    // تبدیل عدد به رشته (برعکس)
    do {
      numbuf[len++] = '0' + (n % 10);
      n /= 10;
    } while(n > 0);
    numbuf[len] = '\0';

    // برعکس کردن رشته عدد
    for (int j = 0; j < len / 2; j++) {
      char tmp = numbuf[j];
      numbuf[j] = numbuf[len - j - 1];
      numbuf[len - j - 1] = tmp;
    }

    lock();
    if (log_index < MAX_LOG_LINES) {
      log_buffer[log_index][0] = 0;
      strcpy(log_buffer[log_index], "thread: ");
      strcat(log_buffer[log_index], numbuf);
      strcat(log_buffer[log_index], "\n");
      log_index++;
    }
    unlock();
  }

  return (void *) number;
}

int main(int argc, char *argv[]) {
  void *sp1 = malloc(STACK_BYTES);
  void *sp2 = malloc(STACK_BYTES);
  void *sp3 = malloc(STACK_BYTES);

  int ta = thread(my_thread, (int *)sp1 + STACK_SIZE, (void *) 100);
  printf("NEW THREAD CREATED %d\n", ta);

  int tb = thread(my_thread, (int *)sp2 + STACK_SIZE, (void *) 200);
  printf("NEW THREAD CREATED %d\n", tb);

  int tc = thread(my_thread, (int *)sp3 + STACK_SIZE, (void *) 300);
  printf("NEW THREAD CREATED %d\n", tc);

  jointhread(ta);
  jointhread(tb);
  jointhread(tc);

  for (int i = 0; i < log_index; i++) {
    printf("%s", log_buffer[i]);
  }

  printf("DONE\n");

  free(sp1);
  free(sp2);
  free(sp3);
  exit(0);
}
