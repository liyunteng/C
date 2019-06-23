/*
 * Description: 3-5 set fl with fcntl
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/24 00:39:58>
 */

#include <fcntl.h>
#include "ourhdr.h"


int main(void)
{
    set_fl(0, O_RDWR);
    return 0;
}
