#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __GNUC__
#    include "config-gcc.h"
#endif

#ifndef __aligned
#    define __aligned(x)
#endif

#ifndef __always_inline
#    define __always_inline inline
#endif

#ifndef offsetof
#    ifdef __compiler_offsetof
#        define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE, MEMBER)
#    else
#        define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#    endif
#endif

#ifndef container_of
#    define container_of(ptr, type, memeber)                                   \
        ((type *)((char *)ptr - offsetof(type, memeber)))
#endif

#ifndef __BEGIN_DECLS
#    ifdef __END_DECLS
#        undef __END_DELCS
#    endif
#    ifdef __cplusplus
#        define __BEGIN_DECLS extern "C" {
#        define __END_DECLS }
#    else
#        define __BEGIN_DECLS
#        define _END_DECLS
#    endif
#endif

#endif
