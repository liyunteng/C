/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-14 10:20
 * Filename : list.c
 * Description :
 * *****************************************************************************/

#include "list.h"
#include <assert.h>

void
list_init(struct list *head)
{
    head->n = head->p = head;
}

void
list_add(struct list *head, struct list *elem)
{
    assert(head->n);

    elem->n = head;
    elem->p = head->p;

    head->p->n = elem;
    head->p    = elem;
}

void
list_add_h(struct list *head, struct list *elem)
{
    assert(head->n);

    elem->n = head->n;
    elem->p = head;

    head->n->p = elem;
    head->n    = elem;
}

void
list_del(struct list *elem)
{
    elem->n->p = elem->p;
    elem->p->n = elem->n;
}

void
list_move(struct list *head, struct list *elem)
{
    list_del(elem);
    list_add(head, elem);
}

int
list_empty(const struct list *head)
{
    return head->n == head;
}

int
list_start(const struct list *head, const struct list *elem)
{
    return elem->p == head;
}

int
list_end(const struct list *head, const struct list *elem)
{
    return elem->n == head;
}

struct list *
list_first(const struct list *head)
{
    return (list_empty(head) ? NULL : head->n);
}

struct list *
list_last(const struct list *head)
{
    return (list_empty(head) ? NULL : head->p);
}

struct list *
list_prev(const struct list *head, const struct list *elem)
{
    return (list_start(head, elem) ? NULL : elem->p);
}

struct list *
list_next(const struct list *head, const struct list *elem)
{
    return (list_end(head, elem) ? NULL : elem->n);
}

unsigned int
list_size(const struct list *head)
{
    unsigned int       s = 0;
    const struct list *v;

    list_iterate(v, head) s++;

    return s;
}

void
list_splice(struct list *head, struct list *head1)
{
    assert(head->n);
    assert(head1->n);

    if (list_empty(head1))
        return;

    head1->p->n = head;
    head1->n->p = head->p;

    head->p->n = head1->n;
    head->p    = head1->p;

    list_init(head1);
}
