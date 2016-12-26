/*
 * =====================================================================================
 *
 *       Filename:  a.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/14/2015 07:02:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  liyunteng (), li_yunteng@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
typedef enum {
	STATE1 = 1,
	STATE2,
	STATE3,
	STATE4,
	STATE5,	 //password pass
}STATE;

typedef enum {
	INPUT1 = '1',
	INPUT2 = '2',
	INPUT3 = '3',
	INPUT4 = '4',
}INPUT;

typedef struct {
	STATE cur_state;
	INPUT input;
	STATE next_state;
}STATE_TRANS;
