__attribute__((__noreturn__))
void khalt(void) {
  while (1) { asm("sti; hlt"); }
  __builtin_unreachable();
}
