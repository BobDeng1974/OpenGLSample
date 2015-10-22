#ifndef __DATA_LIB_H__
#define __DATA_LIB_H__

struct st_node
{
    union{
        void* data; // data for node
        int num;    // count for head node
    };

    struct st_node * next;
};

st_node* create_node();
int get_node_num(st_node* head);
st_node* add_node(st_node* prev, void* data);
void remove_nodes(st_node* head);

#endif //__DATA_LIB_H__
