#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define print_failure() { printf("sys_set_sec failed %d, line %d\n",counter++,__LINE__); }

void copy_clearance(int* clr_path) {
  for (int i = 0; i < tasks.len; ++i) {
    clr_path[i] = tasks.tasks[i].clearance;
  }
}
int check_clearance(const int* clr_path) {
  int s = 0;
  for (int i = 0; i < tasks.len; ++i) {
    if (clr_path[i] != tasks.tasks[i].clearance) {
      s++;
    }
  }
  return s;
}

char clr[5] = {SWORD_CLR,MIDNIGHT_CLR,CLAMP_CLR,DUTY_CLR,ISOLATE_CLR};
int main(int argc, char *argv[])
{
  int capacity = 7;
  init_task_list(capacity);
  current->caps = CAP_SYS_ADMIN;
  long res;

  res = sys_check_sec(tasks.tasks[0].pid,clr[0]);
  if (res != -EPERM) {
    print_failure();
  }
  
  res = sys_check_sec(tasks.tasks[0].pid,'z');
  if (res != -EINVAL) {
    print_failure();
  }

  res = sys_check_sec(current->pid + 1000, clr[1]);
  if (res != -ESRCH) {
    print_failure();
  }
  printf("counter %d passed checking permissions\n",counter);
  
  (void)sys_set_sec(1,1,1,1,1);

  for(int i = 0; i < capacity; i++) {
    struct task_struct* p = &tasks.tasks[i];
    if (p != current) {
      for (int j = 0; j < 5; j++) {
	res = sys_check_sec(p->pid,clr[j]);
	if (res != 0) {
	  print_failure();
	}
      }
    }
  }
  printf("counter %d passed checking other processes\n",counter);
  
  for (int j = 0; j < 5; j++) {
    res = sys_check_sec(current->pid,clr[j]);
    if (res != 1) {
      print_failure();
    }
  }
  printf("counter %d passed checking current\n",counter);

  for(int j = 0; j < 5; j++) {
    int parent_clr = sys_check_sec(current->real_parent->pid,clr[j]);
    int flipped_up = sys_flip_sec_branch(1,clr[j]);
    if (flipped_up != 1) {
      print_failure();
    }
    int parent_clr2 = sys_check_sec(current->real_parent->pid,clr[j]);
    if (parent_clr == parent_clr2) {
      printf("old %d, new %d\n",parent_clr,parent_clr2);
      print_failure();
    }
  }
  printf("counter %d passed flipping parent\n",counter);

  int* clr_path = (int*)malloc(capacity*sizeof(int));
  memset((void*)clr_path,0,sizeof(int)*capacity);
  copy_clearance(clr_path);
  
  int flipped_up = sys_flip_sec_branch(capacity*10,clr[0]);
  if (flipped_up > capacity - 1) {
    printf("flipped more that there is %d\n", flipped_up);
    print_failure();
  }
  printf("flipped up %d\n",flipped_up);
  int diff;
  diff = check_clearance(clr_path);
  if (diff != capacity - 1) {
    printf("some bit didn't flip\n");
    print_failure();
  }
  
  (void)sys_flip_sec_branch(capacity*10,clr[0]);
  diff = check_clearance(clr_path);
  if (diff > 0) {
    printf("flipping twice is not idempotent\n");
    print_failure();
  }
  printf("counter %d\n",counter);
  return 0;
}
