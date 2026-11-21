#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("F54");
MODULE_DESCRIPTION("Silk 120Hz & Unreal Engine Thread Optimizer");

static int is_priority_thread(const char *comm) {
    if (strstr(comm, "RenderThread") || strstr(comm, "RHIThread") ||
        strstr(comm, "GameThread") || strstr(comm, "surfaceflinger") ||
        strstr(comm, "RenderEngine") || strstr(comm, "UnityMain")) {
        return 1;
    }
    return 0;
}

static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
    struct task_struct *task = (struct task_struct *)regs->regs[0];
    if (task && !IS_ERR(task) && is_priority_thread(task->comm)) {
        if (task->prio > 105)
            task->prio = 105;
    }
    return 0;
}

static struct kprobe kp = {
    .symbol_name = "try_to_wake_up",
    .pre_handler = handler_pre,
};

static int __init silk_init(void) {
    return register_kprobe(&kp);
}

static void __exit silk_exit(void) {
    unregister_kprobe(&kp);
}

module_init(silk_init);
module_exit(silk_exit);
