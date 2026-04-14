#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t scancode = inl(KBD_ADDR);
  kbd->keydown = (scancode & KEYDOWN_MASK) ? true : false;
  kbd->keycode = scancode & ~KEYDOWN_MASK;
  if (kbd->keycode != AM_KEY_NONE) {
      printf("Bus Signal: 0x%08x | Down: %d | KeyCode: %d\n", scancode, kbd->keydown, kbd->keycode);
  }
}
