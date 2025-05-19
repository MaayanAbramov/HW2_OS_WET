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

int counter = 0;
void print_failure() {
  printf("sys_set_sec failed %d\n",counter);
  counter++;
}

char clr[5] = {SWORD_CLR,MIDNIGHT_CLR,CLAMP_CLR,DUTY_CLR,ISOLATE_CLR};
int main(int argc,char* argv[]) {
  int capacity = 5;
  init_task_list(capacity);

  long res;


  res = sys_set_sec(1,1,1,1,1);
  if (res != -EPERM) {
    print_failure();
  }
  printf("counter %d passed root privilage\n",counter);
  
  current->caps = CAP_SYS_ADMIN;
  for(int i = 0; i< (1<<5); i++) {
    int flags[5] = {0};
    for(int j = 0; j<5; j++) {
      flags[j] = (i >> j) & 0b1;
    }
    res = sys_set_sec(flags[0],flags[1],flags[2],flags[3],flags[4]);
    if (res != 0) {
      printf("%d%d%d%d%d\n",flags[0],flags[1],flags[2],flags[3],flags[4]);
      print_failure();
    }

    for(int j = 0; j<5; j++) {
      if (flags[j] != sys_get_sec(clr[j])) {
	printf("failed %c\n",clr[j]);
	print_failure();
      }
    }
  }
  printf("counter %d passed set correct flags\n",counter);
  
  for(int i = 0; i< (1<<5); i++) {
    int flags[5] = {0};
    for(int j = 0; j<5; j++) {
      flags[j] = (i >> j) & 0b1;
      
    }
    int f = random()%5;
    flags[f] = -5;
    res = sys_set_sec(flags[0],flags[1],flags[2],flags[3],flags[4]);
    if (res != -EINVAL) {
      printf("%d%d%d%d%d\n",flags[0],flags[1],flags[2],flags[3],flags[4]);
      print_failure();
    }
  }
  printf("counter %d passed set neg values\n",counter);

  for(char c = 0; c < (1<<sizeof(char)); c++) {
    bool valid = false;
    for(int j = 0; j < 5; j++) {
      if(c == clr[j]) {
	valid = true;
      }
    }

    res = sys_get_sec(c);
    if (valid == (res == -EINVAL)) {
      printf("failed %c\n",c);
      print_failure();
    }
  }
}
