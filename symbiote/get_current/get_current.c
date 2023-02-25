#include <stdio.h>
#include <stdlib.h>

#include "LINF/sym_all.h"

#include "../../oot_module/hello.h"

typedef void *(*bpf_get_current_task_t)();

void* get_current(){

    bpf_get_current_task_t gct = NULL;

    if (gct == NULL) {
        gct = (bpf_get_current_task_t) sym_get_fn_address("bpf_get_current_task");
        if (gct == NULL) {
            fprintf(stderr, "failed to find __fdget() \n");
            exit(-1);
        }
    }
    fprintf(stderr, "got __fdget() at %p \n", gct);

    sym_elevate();

    return gct();
}

int main(int argc, char *argv[])
{
    void *task = get_current();
    printf("task at %p\n", task);

    printf("get pid using task struct: %d\n", get_pid_from_task(task));

    printf("get pid using getpid(): %d\n", getpid());

    return 0;
}