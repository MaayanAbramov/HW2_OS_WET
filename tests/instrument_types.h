#ifndef INSTRUMENT_TYPE_H
#define INSTRUMENT_TYPE_H

struct task_struct {
  int pid;
  char clearance;
  int caps;
  struct task_struct* real_parent;
};
struct task_list {
  struct task_struct* tasks;
  int capacity;
  int len;
  int pid_counter;
};


#endif /* INSTRUMENT_TYPE_H */
