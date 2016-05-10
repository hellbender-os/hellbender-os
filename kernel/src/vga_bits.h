static uint32_t vga_bits24[16][3] = {
  {0x00000000, 0x00000000, 0x00000000}, // 0000
  {0x00000000, 0x00000000, 0xffffff00}, // 0001
  {0x00000000, 0xffff0000, 0x000000ff}, // 0010
  {0x00000000, 0xffff0000, 0xffffffff}, // 0011
  {0xff000000, 0x0000ffff, 0x00000000}, // 0100
  {0xff000000, 0x0000ffff, 0xffffff00}, // 0101
  {0xff000000, 0xffffffff, 0x000000ff}, // 0110
  {0xff000000, 0xffffffff, 0xffffffff}, // 0111
  {0x00ffffff, 0x00000000, 0x00000000}, // 1000
  {0x00ffffff, 0x00000000, 0xffffff00}, // 1001
  {0x00ffffff, 0xffff0000, 0x000000ff}, // 1010
  {0x00ffffff, 0xffff0000, 0xffffffff}, // 1011
  {0xffffffff, 0x0000ffff, 0x00000000}, // 1100
  {0xffffffff, 0x0000ffff, 0xffffff00}, // 1101
  {0xffffffff, 0xffffffff, 0x000000ff}, // 1110
  {0xffffffff, 0xffffffff, 0xffffffff}, // 1111
};

static uint64_t vga_bits32[16][2] = {
  {0x0000000000000000, 0x0000000000000000}, // 0000
  {0x0000000000000000, 0x00ffffff00000000}, // 0001
  {0x0000000000000000, 0x0000000000ffffff}, // 0010
  {0x0000000000000000, 0x00ffffff00ffffff}, // 0011
  {0x00ffffff00000000, 0x0000000000000000}, // 0100
  {0x00ffffff00000000, 0x00ffffff00000000}, // 0101
  {0x00ffffff00000000, 0x0000000000ffffff}, // 0110
  {0x00ffffff00000000, 0x00ffffff00ffffff}, // 0111
  {0x0000000000ffffff, 0x0000000000000000}, // 1000
  {0x0000000000ffffff, 0x00ffffff00000000}, // 1001
  {0x0000000000ffffff, 0x0000000000ffffff}, // 1010
  {0x0000000000ffffff, 0x00ffffff00ffffff}, // 1011
  {0x00ffffff00ffffff, 0x0000000000000000}, // 1100
  {0x00ffffff00ffffff, 0x00ffffff00000000}, // 1101
  {0x00ffffff00ffffff, 0x0000000000ffffff}, // 1110
  {0x00ffffff00ffffff, 0x00ffffff00ffffff}, // 1111
};
