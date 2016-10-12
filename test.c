#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timer.h"

#define MAX_NUM 65536
#define TIMEOUT 10

typedef struct roc_node_s
{
    timer_node_t node;
    void *ctx;
    int slot;
}roc_node_t;

typedef struct roc_root_s
{
    timer_root_t root;
    roc_node_t data[MAX_NUM];
}roc_root_t;

//回收资源
void roc_timer_work(void *arg)
{
    roc_node_t *data = NULL;
    roc_root_t *handle = NULL;

    if(arg == NULL) return;

    data = (roc_node_t*)arg;
    handle = (roc_root_t*)container_of((void*)(data-data->slot), roc_root_t, data);

#if 0
    timer_set_expire(&data->node, time(NULL) + TIMEOUT);
    if(timer_get_expire(&data->node) > time(NULL))
    {
        timer_insert(&handle->root, &data->node);
        return;
    }
#endif

#ifdef _DEBUG
    fprintf(stdout, "expire - %ld: %s", data->node.expire, ctime(&data->node.expire));
#endif

    data->slot = -1;
    if(data->ctx)
    {
        free(data->ctx);
        data->ctx = NULL;
    }
    return;
}

int main(int argc, char *argv[])
{
    int loop;
    roc_root_t handle;
    roc_node_t *data = handle.data;

    timer_init(&handle.root);
    memset(data, 0, MAX_NUM*sizeof(roc_node_t));

    for(loop = 0; loop < MAX_NUM; loop ++)
    {
        timer_set_expire(&data[loop].node, time(NULL) + TIMEOUT);
        timer_insert(&handle.root, &data[loop].node);
        data[loop].node.cb = roc_timer_work;
        data[loop].node.data = (void*)&data[loop];
        data[loop].ctx = (void*)strdup("12345678");
        data[loop].slot = loop;

        sleep(1);
    }

    timer_dump(&handle.root);
    while(timer_sum(&handle.root))
    {
        timer_beat(&handle.root, time(NULL));
        sleep(1);
    }

    for(loop = 0; loop < MAX_NUM; loop ++)
    {
        data[loop].slot = -1;
        if(data[loop].ctx)
        {
            free(data[loop].ctx);
            data[loop].ctx = NULL;
        }
    }

#if 0
    for(loop = 0; loop < MAX_NUM; loop ++)
        timer_erase(&handle.root, timer_get_expire(&data[loop].node));
#endif

#if 0
    for(loop = 0; loop < MAX_NUM; loop ++)
        timer_remove(&handle.root, &data[loop].node);
#endif

    timer_exit(&handle.root);
    return 0;
}
