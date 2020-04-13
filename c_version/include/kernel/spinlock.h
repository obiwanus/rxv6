#ifndef XV6_SPINLOCK_H
#define XV6_SPINLOCK_H

#include "base.h"
#include "proc.h"

typedef struct Spinlock {
  bool locked;  // is the lock held?

  // For debugging:
  char *name;  // name of the lock
  CPU *cpu;    // which cpu is holding the lock
  u8 pcs[10];  // the call stack that locked the lock
} Spinlock;

void init_lock(Spinlock *lock, char *name);
void acquire(Spinlock *lock);
void release(Spinlock *lock);

#endif  // XV6_SPINLOCK_H
