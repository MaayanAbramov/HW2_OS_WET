#include <stdbool.h>
#include <linux/capability.h>
#include <stdio.h>
#include <stdlib.h>
#include "instrument_types.h"


struct task_list tasks;
struct task_struct* current;
bool capable(int cap) {
  return (current->caps == cap);
}

void printk(const char* format) {
  fputs(format,stderr);
}

struct task_struct* find_task_by_vpid(int pid) {
  for(int i = 0; i < tasks.len; i++) {
    if (pid == tasks.tasks[i].pid) {
      return &tasks.tasks[i];
    }
  }
  return NULL;
}
