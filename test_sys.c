/*
 * test.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:  2016/12/26 13:10:49
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <stdio.h>


int main(int argc, char *argv[])
{

#ifdef __STRICT_ANSI__

    printf("__STRICT_ANSI__\n");

#endif // __STRICT_ANSI__

#ifdef _ISOC99_SOURCE

    printf("_ISOC99_SOURCE\n");

#endif // _ISOC99_SOURCE

#ifdef _ISOC11_SOURCE

    printf("_ISOC11_SOURCE\n");

#endif // _ISOC11_SOURCE

#ifdef _POSIX_SOURCE

    printf("_POSIX_SOURCE\n");

#endif // _POSIX_SOURCE

#ifdef _POSIX_C_SOURCE

    printf("_POSIX_C_SOURCE\n");

#endif // _POSIX_C_SOURCE

#ifdef _XOPEN_SOURCE

    printf("_XOPEN_SOURCE\n");

#endif // _XOPEN_SOURCE

#ifdef _XOPEN_SOURCE_EXTENDED

    printf("_XOPEN_SOURCE_EXTENDED\n");

#endif // _XOPEN_SOURCE_EXTENDED

#ifdef _LARGEFILE_SOURCE

    printf("_LARGEFILE_SOURCE\n");

#endif // _LARGEFILE_SOURCE

#ifdef _LARGEFILE64_SOURCE

    printf("_LARGEFILE64_SOURCE\n");

#endif // _LARGEFILE64_SOURCE

#ifdef _BSD_SOURCE

    printf("_BSD_SOURCE\n");

#endif // _BSD_SOURCE

#ifdef _SVID_SOURCE

    printf("_SVID_SOURCE\n");

#endif // _SVID_SOURCE

#ifdef _ATFILE_SOURCE

    printf("_ATFILE_SOURCE\n");

#endif // _ATFILE_SOURCE

#ifdef _GNU_SOURCE

    printf("_GNU_SOURCE\n");

#endif // _GNU_SOURCE

#ifdef _DEFAULT_SOURCE

    printf("_DEFUALT_SOURCE\n");

#endif // _DEFAULT_SOURCE

#ifdef _REENTRANT

    printf("_REENTRANT\n");

#endif // _REENTRANT

#ifdef _THREAD_SAFE

    printf("_THREAD_SAFE\n");

#endif // _THREAD_SAFE

#ifdef _FORTIFY_SOURCE

    printf("_FORTIFY_SOURCE\n");

#endif // _FORTIFY_SOURCE

#ifdef __USE_ISOC11

    printf("__USE_ISOC11\n");

#endif // __USE_ISOC11

#ifdef __USE_ISOC99

    printf("__USE_ISOC99\n");

#endif // __USE_ISOC99

#ifdef __USE_ISOC95

    printf("__USE_ISOC95\n");

#endif // __USE_ISOC95

#ifdef __USE_POSIX

    printf("__USE_POSIX\n");

#endif // __USE_POXIS

#ifdef __USE_POSIX2

    printf("__USE_POSIX2\n");

#endif // __USE_POSIX2

#ifdef __USE_POSIX199309

    printf("__USE_POSIX199309\n");

#endif // __USE_POSIX199309
#ifdef __USE_POSIX199506

    printf("__USE_POSIX199506\n");

#endif // __USE_POSIX199506

#ifdef __USE_XOPEN

    printf("__USE_XOPEN\n");

#endif // __USE_XOPEN

#ifdef __USE_XOPEN_EXTENDED

    printf("__USE_XOPEN_EXTENDED\n");

#endif // __USE_XOPEN_EXTENDED

#ifdef __USE_UNIX98

    printf("__USE_UNI98\n");

#endif // __USE_UNIX98

#ifdef __USE_XOPEN2K

    printf("__USE_XOPEN2K\n");

#endif // __USE_XOPEN2K

#ifdef __USE_XOPEN2KXSI

    printf("__USE_XOPEN2KXSI\n");

#endif // __USE_XOPEN2KXSI
#ifdef __USE_XOPEN2K8

    printf("__USE_XOPEN2K8\n");

#endif // __USE_XOPEN2K8

#ifdef __USE_XOPEN2K8XSI

    printf("__USE_XOPEN2K8XSI\n");

#endif // __USE_XOPEN2K8XSI

#ifdef __USE_LARGEFILE

    printf("__USE_LARGEFILE\n");

#endif // __USE_LARGEFILE

#ifdef __USE_LARGEFILE64

    printf("__USE_LARGEFILE64\n");

#endif // __USE_LARGEFILE64

#ifdef __USE_FILE_OFFSET64

    printf("__USE_FILE_OFFSET64\n");

#endif // __USE_FILE_OFFSET64

#ifdef __USE_BSD

    printf("__USE_BSD\n");

#endif // __USE_BSD

#ifdef __USE_SVID

    printf("__USE_SVID\n");

#endif // __USE_SVID

#ifdef __USE_MISC

    printf("__USE_MISC\n");

#endif // __USE_MISC
#ifdef __USE_ATFILE

    printf("__USE_ATFILE\n");

#endif // __USE_ATFILE
#ifdef __USE_GNU

    printf("__USE_GNU\n");

#endif // __USE_GNU
#ifdef __USE_REENTRANT

    printf("__USE_REENTRANT\n");

#endif // __USE_REENTRANT
#ifdef __USE_FORTIFY_LEVEL

    printf("__USE_FORTIFY_LEVEL\n");

#endif // __USE_FORTIFY_LEVEL


    return 0;
}
