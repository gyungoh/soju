#ifndef __HAN_H
#define __HAN_H

#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))

int han_is_empty(void);
unsigned short han_flush(void);
int han_backspace(void);
unsigned short han_preedit(void);
unsigned short han_process(signed char key);

#endif
