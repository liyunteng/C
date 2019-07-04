/*
 * =====================================================================================
 *
 *       Filename:  a.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  05/14/2015 07:04:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  liyunteng (), li_yunteng@163.com
 *   Organization:
 *
 * =====================================================================================
 */

/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2015-05-14 07:04
 * Filename : a.c
 * Description :
 * *****************************************************************************/
#include <stdio.h>
#include "a.h"

STATE_TRANS state_trans_arry[] = {
    {STATE1, INPUT1, STATE2},
    {STATE2, INPUT2, STATE3},
    {STATE3, INPUT3, STATE4},
    {STATE4, INPUT4, STATE5},
};

#define STATE_TRANS_CNT (sizeof(state_trans_arry))/sizeof(state_trans_arry[0])

int main(int argc, const char *argv[])
{
    int i;
    char ch;

    STATE state_machine = STATE1;

    while (ch != 'e') {
        ch = getchar();
        if ((ch >= '0') && (ch <= '9')) {
            for (i = 0; i < STATE_TRANS_CNT; i++) {
                if ((ch == state_trans_arry[i].input)
                    && (state_machine == state_trans_arry[i].cur_state)) {
                    state_machine = state_trans_arry[i].next_state;
                    printf("ch:%c i:%d next\n", ch, i);
                    continue;
                } else if (i == (STATE_TRANS_CNT)) {
                    state_machine = STATE1;
                }

            }
            if (state_machine == STATE5)
                printf("Password correct, state transfer machine pass!\n");
        }
    }
    return 0;
}
