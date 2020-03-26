
static int addone(int src) {
  int dst;

  asm volatile(
      "mov %1, %0\n\t"
      "add 1, %0"
      : "=r"(dst)
      : "r"(src));

  return dst;
}

int bootmain() {
  int a = 1;
  int b = addone(a);

  return b;
}
