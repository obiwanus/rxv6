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
}

void
release(Spinlock *lock)
{
  // TODO
}
