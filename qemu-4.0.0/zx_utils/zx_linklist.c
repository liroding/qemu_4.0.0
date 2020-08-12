#include  <stdlib.h>
#include  <assert.h>
#include "zx_utils/zx_linklist.h"

void list_tail_insert(zx_plist_t *head, zx_hook_t data)
{
    zx_plist_t p_new = (zx_plist_t)malloc(sizeof(zx_list_t));
    p_new->data = data;          
    p_new->next = NULL;         
    zx_plist_t temp = *head;         

    if (NULL == *head)          
    {
        *head = p_new;
        p_new->next = NULL;
    } else {
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = p_new;     
    }
}


void delete_list(zx_plist_t *head, zx_hook_t data) {
    assert(head);
    zx_plist_t p = *head; 
    zx_plist_t pre = *head; 
    zx_hook_t d = (*head)->data;

    while(p) {
        d = p->data; 
        if(((d.addr_low) == (data.addr_low)) 
           && ((d.addr_high) == (data.addr_high))) {
            if(p == (*head)) {
                *head = (*head)->next;
            } else {
                pre->next = p->next;
            } 
        }

        pre = p;
        p = p->next;
    }
}


void destroy_list(zx_plist_t head)
{
    zx_plist_t node = head;
    while(node != NULL)
    {
        head = head->next;
        free(node);
        node = head;
    }

}
