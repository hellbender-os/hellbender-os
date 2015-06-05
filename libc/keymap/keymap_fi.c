#include <string.h>
#include <sys/keymap.h>
// backspace 66
// home ec
// pgup fd
// pgdw fa
// end  e9
// f1 5
// f2 6
// f3 4
// f4 c
// f5 3
// f6 b
// f7 83
// f8 a
// f9 1
// f10 9
// f11 78
// f12 7
// ins f0
// def f1
// tab d
// enter 5a
// caps 58
// up f5
// down f2
// left eb
// right f4
// space 29

keymap_t keymap_fi = {
// plain:
// 0000000000000000111111111111111122222222222222223333333333333333
// 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
  "              §      q1   zsaw2  cxde43   vftr5  nbhgy6   mju78 " //  0-3f
  " ,kio09  .-l p+           \n  '   <    \b               \033         " // 40-7f
  "                                                                " // 80-bf
  "                                                               " // c0-fe
  ,

// shift:
// 0000000000000000111111111111111122222222222222223333333333333333
// 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
  "                     Q!   ZSAW\"  CXDE #   VFTR%  NBHGY&   MJU/( " //  0-3f
  " ;KIO=)  :_L P?      `     ^ *   >                              " // 40-7f
  "                                                                " // 80-bf
  "                                                               " // c0-fe
  ,

// alt:
// 0000000000000000111111111111111122222222222222223333333333333333
// 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
  "                              @     ¤$£       ¤           µ  {[ " //  0-3f
  "     }]       \\            ~     |                              " // 40-7f
  "                                                                " // 80-bf
  "                                                               " // c0-fe
  ,

};
