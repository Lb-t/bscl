#include <windows.h>
#include "bscl_os.h"

#if defined(__GNUC__)
struct bscl_os_thread_win32_t_ {
    HANDLE handle;
    void *arg;
    bscl_os_thread_routine_t routine;
};

unsigned long thread_proc(void *arg)
{
    struct bscl_os_thread_win32_t_ *tsk = (struct bscl_os_thread_win32_t_ *)arg;
    tsk->routine(tsk->arg);
    return 0;
}

int bscl_os_thread_create(bscl_os_thread_t *tid, struct bscl_os_thread_attr *attr,
                   bscl_os_thread_routine_t routine, void *arg)
{
    *tid = malloc(sizeof(struct bscl_os_thread_win32_t_));
    if (!(*tid)) {
        return -1;
    }
    (*tid)->routine = routine;
    (*tid)->handle = CreateThread(NULL, 0, thread_proc, &tid, 0, NULL);
    (*tid)->arg = arg;
    if ((*tid)->handle == INVALID_HANDLE_VALUE) {
        return -1;
    }
    return 0;
}

bscl_os_thread_t bscl_os_thread_self(void)
{
    return NULL;
}

int bscl_os_thread_join(bscl_os_thread_t tid, void **retval)
{
    return 0;
}


#endif
