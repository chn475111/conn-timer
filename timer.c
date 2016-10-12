#include "timer.h"

int timer_init(timer_root_t *root)
{
    if(root == NULL) return 0;

    root->rbroot = RB_ROOT;
    root->sum = 0;
    return 1;
}

int timer_insert(timer_root_t *root, timer_node_t *node)
{
    if(root == NULL || node == NULL || node->trigger == true) return 0;

    struct rb_node **rbnode = &root->rbroot.rb_node, *parent = NULL;
    while(*rbnode)
    {
        timer_node_t *pos = rb_entry(*rbnode, timer_node_t, rbnode);
        time_t result = node->expire - pos->expire;

        parent = *rbnode;
        if (result < 0)
            rbnode = &((*rbnode)->rb_left);
        else if (result > 0)
            rbnode = &((*rbnode)->rb_right);
        else
            return 0;
    }

    rb_link_node(&node->rbnode, parent, rbnode);
    rb_insert_color(&node->rbnode, &root->rbroot);
    node->trigger = true;
    root->sum ++;
    return 1;
}

timer_node_t* timer_search(timer_root_t *root, time_t expire)
{
    if(root == NULL) return NULL;

    struct rb_node *rbnode = root->rbroot.rb_node;
    while(rbnode)
    {
        timer_node_t *pos = rb_entry(rbnode, timer_node_t, rbnode);
        time_t result = expire - pos->expire;

        if (result < 0)
            rbnode = rbnode->rb_left;
        else if (result > 0)
            rbnode = rbnode->rb_right;
        else
            return pos;
    }
    return NULL;
}

void timer_erase(timer_root_t *root, time_t expire)
{
    if(root == NULL) return;

    timer_node_t *pos = timer_search(root, expire);
    if(pos)
    {
        rb_erase(&pos->rbnode, &root->rbroot);
        RB_CLEAR_NODE(&pos->rbnode);
        pos->trigger = false;
        root->sum --;
    }
}

int timer_remove(timer_root_t *root, timer_node_t *node)
{
    if(root == NULL || node == NULL || node->trigger == false) return 0;

    rb_erase(&node->rbnode, &root->rbroot);
    RB_CLEAR_NODE(&node->rbnode);
    node->trigger = false;
    root->sum --;
    return 1;
}

int timer_set_expire(timer_node_t *node, time_t expire)
{
    if(node == NULL) return 0;
    node->expire = expire;
    return 1;
}

time_t timer_get_expire(timer_node_t *node)
{
    if(node == NULL) return 0;
    return node->expire;
}

int timer_sum(timer_root_t *root)
{
    if(root == NULL) return 0;
    return root->sum;
}

void timer_beat(timer_root_t *root, time_t now)
{
    if(root == NULL) return;

    timer_node_t *pos = NULL;
    struct rb_node *rbnode = NULL;
    for(rbnode = rb_first(&root->rbroot); rbnode != NULL; rbnode = rb_next(rbnode))
    {
        pos = rb_entry(rbnode, timer_node_t, rbnode);
        if(pos->expire > now)               //升序排列
        {
        #ifdef _DEBUG
            fprintf(stdout, "now - %ld: %s", now, ctime(&now));
        #endif
            break;
        }

        timer_remove(root, pos);            //删除超时
        pos->cb(pos->data);                 //执行回调
    }
}

void timer_dump(timer_root_t *root)
{
    if(root == NULL) return;

    timer_node_t *pos = NULL;
    struct rb_node *rbnode = NULL;
    for(rbnode = rb_first(&root->rbroot); rbnode != NULL; rbnode = rb_next(rbnode))
    {
        pos = rb_entry(rbnode, timer_node_t, rbnode);
    #ifdef _DEBUG
        fprintf(stdout, "expire - %ld: %s", pos->expire, ctime(&pos->expire));
    #endif
    }
}

void timer_exit(timer_root_t *root)
{
    if(root == NULL) return;

    timer_node_t *pos = NULL;
    struct rb_node *rbnode = NULL;
    while((rbnode = rb_first(&root->rbroot)))
    {
        pos = rb_entry(rbnode, timer_node_t, rbnode);
    #ifdef _DEBUG
        fprintf(stdout, "expire - %ld: %s", pos->expire, ctime(&pos->expire));
    #endif
        timer_remove(root, pos);
    }
}
