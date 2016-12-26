#ifndef __CONFIG_GCC_H_
#define __CONFIG_GCC_H_

#define __aligned(x)	__attribute__((__aligned__(x)))
#define __printf(a, b)  __attribute__((format(printf, (a), (b))))
#define __scanf(a, b)	__attribute__((format(scanf, (a), (b))))

#endif
