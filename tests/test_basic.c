#include <stdio.h>
#include <stdlib.h>
#include "instrument.h"
#include "hw2.c"

void
init_task_list(int capacity)
{
  struct task_struct* t_list = (struct task_struct*)malloc(capacity*sizeof(struct task_struct));
  tasks.tasks = t_list;
  tasks.capacity = capacity;
  tasks.len = capacity;
  tasks.pid_counter = 0;

  struct task_struct* prev_task = NULL;
  for (int i = 0; i < capacity; i++) {
    tasks.tasks[i].pid = tasks.pid_counter;
    tasks.pid_counter++;

    tasks.tasks[i].real_parent = prev_task;
    prev_task = &tasks.tasks[i];

    tasks.tasks[i].clearance = 0;
    tasks.tasks[i].caps = 0;
  }
  tasks.tasks[0].caps = CAP_SYS_ADMIN;
  
  current = &tasks.tasks[capacity-1];
}

void
print_current()
{
  int ppid = (current->real_parent) ? current->real_parent->pid : -1;
  printf("----------------\n");
  printf("capability: %d  \n",current->caps);
  printf("clearance: %x   \n",current->clearance);
  printf("pid : %d        \n",current->pid);
  printf("ppid: %d        \n",ppid);
  printf("----------------\n");
}

int
main(int argc, char *argv[])
{
  int capacity = 4;
  init_task_list(capacity);
  printf("salam\n");
  sys_hello();

  current->caps = CAP_SYS_ADMIN;
  print_current();

  {
    long res = sys_set_sec(1,1,1,1,1);
    print_current();
  }
  
  char clrs[] = {'s','m','c','d','i'};
  int clrs_len = 5;
  for (int i = 0; i < clrs_len; ++i) {
    long res = sys_get_sec(clrs[i]);
    printf("%c->%ld\n",clrs[i],res);
  }
  return 0;
}
