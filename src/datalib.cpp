#include "datalib.h"


st_node* create_node()
{
    st_node* p = new st_node();
    p->data = p->next = NULL;
    return p;
}

st_node* add_node(st_node* prev, void* data)
{
    prev->next = create_node();
    prev->next->data = data;
    return prev->next;
}

int get_node_num(st_node* head)
{
    if (head == NULL)
        return 0;
    st_node* tmp = head;
    int num = 0;
    while(tmp != NULL)
    {
        num += 1;
        tmp = tmp->next;
    }
    return num;
}
