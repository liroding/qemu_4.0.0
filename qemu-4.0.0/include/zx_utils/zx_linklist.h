#ifndef _H_ZX_LINKLIST_H_
#define _H_ZX_LINKLIST_H_
#include<stdio.h>

typedef struct 
{
    unsigned long addr_low;
    unsigned long addr_high;
} zx_hook_t;

typedef struct linklist
{
    zx_hook_t       data;
    struct linklist *next;
}zx_list_t, *zx_plist_t;

void list_tail_insert(zx_plist_t *head, zx_hook_t data);
void destroy_list(zx_plist_t head);

#endif