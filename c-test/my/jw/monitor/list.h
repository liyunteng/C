#ifndef LIB_DEVICE_MAPPER_H
#define LIB_DEVICE_MAPPER_H

#include <inttypes.h>
#include <stdarg.h>
#include <sys/types.h>

#ifdef linux
#	include <linux/types.h>
#endif

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef __GNUC__
#	define __typeof__ typeof
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct list {
	struct list *n, *p;
};

#define LIST_INIT(name) struct list name = {&(name), &(name)}
void list_init(struct list *head);

//将elem加到head前面
void list_add(struct list *head, struct list *elem);

//将elem加到head后面
void list_add_h(struct list *head, struct list *elem);

//删除elem
void list_del(struct list *elem);

//将elem移动到head前面
void list_move(struct list *head, struct list *elem);

//是否为空
int list_empty(const struct list *head);

//elem是否为第一个元素
int list_start(const struct list *head, const struct list *elem);

//elem是否为最后一个元素
int list_end(const struct list *head, const struct list *elem);

//返回链表的第一个元素
struct list *list_first(const struct list *head);

//返回链表最后一个元素
struct list *list_last(const struct list *head);

//返回elem前面的元素，如果elem是第一个返回NULL
struct list *list_prev(const struct list *head, const struct list *elem);

//返回elem后面的元素， 如果elem是最后一个返回NULL
struct list *list_next(const struct list *head, const struct list *elem);

//v为给定的list的地址，返回包含list的结构t的地址
#define list_struct_base(v, t, head)	\
	((t *)((const char *)(v) - (const char *)&((t *)0)->head))

#define list_item(v, t) list_struct_base((v), (t), list)

//v为给定的结构t中元素e的地址，返回结构t中f的地址
#define struct_field(v, t, e, f)	\
	(((t *)((uintptr_t)(v) - (uintptr_t)&((t *)0)->e))->f)

#define list_head(v, t, e) struct_field(v, t, e, list)

//顺序遍历
#define list_iterate(v, head)	\
	for (v = (head)->n; v != head; v = v->n);

//逆序从start到head遍历
#define list_uniterate(v, head, start)	\
	for (v = (start)->p; v != head; v = v->p);

//安全的顺序遍历
#define list_iterate_safe(v, t, head)	\
	for (v = (head)->n, t = v->n;  v != head; v = t, t = v->n)

#define list_iterate_items_gen(v, head, field)	\
	for (v=list_struct_base((head)->n, __typeof__(*v), field);	\
			&v->field != (head);	\
			v=list_struct_base(v->field.n, __typeof__(*v), field))

unsigned int list_size(const struct list *head);

//将head1加到head的末尾，并将head1置为空
void list_splice(struct list *head, struct list *head1);

#ifdef __cplusplus
}
#endif


#endif
