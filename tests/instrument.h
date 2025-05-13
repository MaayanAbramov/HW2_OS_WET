#ifndef INSTRUMENT_H
#define INSTRUMENT_H
#include <stdbool.h>
#include "instrument_types.h"

bool capable(int cap);
void printk(const char* format);




extern struct task_struct* current;
extern struct task_list tasks;

struct task_struct* find_task_by_vpid(int pid);

#endif /* INSTRUMENT_H */
