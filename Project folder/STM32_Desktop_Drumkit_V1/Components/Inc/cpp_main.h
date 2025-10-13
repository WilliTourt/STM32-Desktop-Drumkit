#ifndef CPP_MAIN_H_
#define CPP_MAIN_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "usart.h"
#include "adc.h"

int cpp_main();

#ifdef __cplusplus
}
#endif

extern void DBG(const char* str);
extern char dbg_buf[128];

#endif