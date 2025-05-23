#       include <linux/capability.h>
#ifndef INST
#       include <linux/kernel.h>
#       include <linux/list.h>
#       include <linux/module.h>
#       include <linux/sched.h>
#       include <linux/sched/mm.h>
#       include <linux/sched/task.h>
#       include <linux/mm.h>
#       include <asm/tlbflush.h>
#else
#       include <errno.h>
#       include <sys/types.h>
#       include "instrument.h"
#define asmlinkage 
#endif

#define SWORD_MAGIC 0x80
#define MIDNIGHT_MAGIC 0x40
#define CLAMP_MAGIC 0x20
#define DUTY_MAGIC 0x10
#define ISOLATE_MAGIC 0x8
#define SWORD_CLR 's'
#define MIDNIGHT_CLR 'm'
#define CLAMP_CLR 'c'
#define DUTY_CLR 'd'
#define ISOLATE_CLR 'i'

asmlinkage long sys_hello(void) {
    printk("Hello, World!\n"); // hello
    return 0;
}
asmlinkage long sys_set_sec(int sword, int midnight, int clamp, int duty, int isolate) {
    if (sword < 0 || midnight < 0 || clamp < 0 || duty < 0 || isolate < 0) {
        return -EINVAL;
    }
    if (!capable(CAP_SYS_ADMIN)) {
      return -EPERM;
    }
    int is_sword = (sword > 0);
    int is_midnight = (midnight>0);
    int is_clamp = (clamp >0);
    int is_duty = (duty > 0 );
    int is_isolate = (isolate > 0);
    char new_clearance = 0;
    if(is_sword){
/* #ifndef NDEBUG */
/*         printk("is_sword!\n"); */
/* #endif */
        new_clearance = new_clearance | SWORD_MAGIC;
    }
    if(is_midnight){
/* #ifndef NDEBUG */
/*       printk("is_midnight!\n"); */
/* #endif */
        new_clearance = new_clearance | MIDNIGHT_MAGIC;
    }
    if(is_clamp){
/* #ifndef NDEBUG */
/*       printk("is_clamp!\n"); */
/* #endif */
        new_clearance = new_clearance | CLAMP_MAGIC;
    }

    if(is_duty){
/* #ifndef NDEBUG */
/*       printk("is_duty!\n"); */
/* #endif */
        new_clearance = new_clearance | DUTY_MAGIC;
    }
    if(is_isolate){
/* #ifndef NDEBUG */
/*       printk("is_isolate!\n"); */
/* #endif */
        new_clearance = new_clearance | ISOLATE_MAGIC;
    }
    current->clearance = new_clearance;
    return 0;
}
asmlinkage long sys_get_sec(char clr) {
    int is_sword_clr = (clr == SWORD_CLR);
    int is_midnight_clr = (clr == MIDNIGHT_CLR);
    int is_clmap_clr = (clr == CLAMP_CLR);
    int is_duty_clr = ( clr == DUTY_CLR);
    int is_isolate_clr = (clr == ISOLATE_CLR);
    int is_valid = (is_sword_clr|| is_midnight_clr|| is_clmap_clr || is_duty_clr ||is_isolate_clr );
    if (!is_valid)
    {
        return -EINVAL;
    }
    if(is_sword_clr){
        return ((current->clearance & SWORD_MAGIC)>0)? 1 : 0 ;
    }
      if(is_midnight_clr){
        return ((current->clearance & MIDNIGHT_MAGIC)>0)? 1 : 0 ;
    }
      if(is_clmap_clr){
        return ((current->clearance & CLAMP_MAGIC)>0)? 1 : 0 ;
    }
      if(is_duty_clr){
        return ((current->clearance & DUTY_MAGIC)>0)? 1 : 0 ;
    }
      if(is_isolate_clr){
        return ((current->clearance & ISOLATE_MAGIC)>0)? 1 : 0 ;
    }
/* #ifndef NDEBUG */
/*       printk("problem in sys_get_sec, none of the conditions above got checked... doesnt make sense\n"); */
/* #endif */
    return -1;
}
asmlinkage long sys_check_sec(pid_t pid, char clr) {
    if (clr != SWORD_CLR && clr != MIDNIGHT_CLR && clr != CLAMP_CLR && clr != ISOLATE_CLR && clr != DUTY_CLR) {
        return -EINVAL;
    }
    struct task_struct* p = find_task_by_vpid(pid);
    if (!p) {
        return -ESRCH;
    }
    int is_sword_clr = (clr == SWORD_CLR);
    int is_midnight_clr = (clr == MIDNIGHT_CLR);
    int is_clmap_clr = (clr == CLAMP_CLR);
    int is_duty_clr = ( clr == DUTY_CLR);
    int is_isolate_clr = (clr == ISOLATE_CLR); 
    if (is_sword_clr) {
        if((current->clearance & SWORD_MAGIC ) == 0){
            return -EPERM;
        }
        return (p->clearance & SWORD_MAGIC) ? 1 : 0;
    }
    if (is_midnight_clr) {
        if((current->clearance & MIDNIGHT_MAGIC ) == 0){
            return -EPERM;
        }
        return (p->clearance & MIDNIGHT_MAGIC) ? 1 : 0;
    }
        if (is_clmap_clr) {
        if((current->clearance & CLAMP_MAGIC ) == 0){
            return -EPERM;
        }
        return (p->clearance & CLAMP_MAGIC) ? 1 : 0;
    }
        if (is_duty_clr) {
        if((current->clearance & DUTY_MAGIC ) == 0){
            return -EPERM;
        }
        return (p->clearance & DUTY_MAGIC) ? 1 : 0;
    }
        if (is_isolate_clr) {
        if((current->clearance & ISOLATE_MAGIC ) == 0){
            return -EPERM;
        }
        return (p->clearance & ISOLATE_MAGIC) ? 1 : 0;
    }
    printk("error on sys_check_sec, shouldn't happen\n");
    return -1; //success
}
int parents_clr_converter(int height, char MAGIC) {
    struct task_struct* temp = current;
    
    int sum_true_clr=0;
    while (height > 0) {
        temp = temp->real_parent;
        if (!temp) {
            break;
        }
        if(temp->clearance & MAGIC) {
            temp->clearance = temp->clearance ^ MAGIC;
        }
        else {
            temp->clearance = temp->clearance | MAGIC;
            sum_true_clr +=1;
        }
        height--;
    }
    return sum_true_clr;
}
asmlinkage long sys_flip_sec_branch(int height, char clr) {
    if (clr != SWORD_CLR && clr != MIDNIGHT_CLR && clr != CLAMP_CLR && clr != ISOLATE_CLR && clr != DUTY_CLR) {
        return -EINVAL;
    }
    if (height <= 0) {
      return -EINVAL;
    }
    int is_sword_clr = (clr == SWORD_CLR);
    int is_midnight_clr = (clr == MIDNIGHT_CLR);
    int is_clmap_clr = (clr == CLAMP_CLR);
    int is_duty_clr = ( clr == DUTY_CLR);
    int is_isolate_clr = (clr == ISOLATE_CLR); 
    if (is_sword_clr) {
        if ( !(current->clearance & SWORD_MAGIC)) {
            return -EPERM;
        }
        return parents_clr_converter( height, SWORD_MAGIC);
    }
        if (is_midnight_clr) {
        if ( !(current->clearance & MIDNIGHT_MAGIC)) {
            return -EPERM;
        }
        return parents_clr_converter( height, MIDNIGHT_MAGIC);
    }
        if (is_clmap_clr) {
        if ( !(current->clearance & CLAMP_MAGIC)) {
            return -EPERM;
        }
        return parents_clr_converter( height, CLAMP_MAGIC);
    }
        if (is_duty_clr) {
        if ( !(current->clearance & DUTY_MAGIC)) {
            return -EPERM;
        }
        return parents_clr_converter( height, DUTY_MAGIC);
    }
        if (is_isolate_clr) {
        if ( !(current->clearance & ISOLATE_MAGIC)) {
            return -EPERM;
        }
        return parents_clr_converter( height, ISOLATE_MAGIC);
    }
    return 0;
}
