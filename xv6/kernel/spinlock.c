#include "kernel/spinlock.h"

void
init_lock(Spinlock *lock, char *name)
{
  lock->locked = false;
  lock->name = name;
  lock->cpu = NULL;
}

void
acquire(Spinlock *lock)
{
  // TODO
  // - maybe use C atomics
  // - when implemented, check whether avoiding locking
  //   is necessary in the initial setup of kernel memory
}

void
release(Spinlock *lock)
{
  // TODO
}
