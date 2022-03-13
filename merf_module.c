#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/moduleparam.h>

#include "strerr.h"
#include "merf.h"

#define pr_merf(msg...) pr_info("[MERF] " msg)

static int target_pid = -1;
module_param(target_pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

static unsigned long (*lookup_name)(const char *name);

static inline int lookup_init(void)
{
    struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name",
    };

    if (register_kprobe(&kp) < 0)
        return -EINVAL;
    lookup_name = (unsigned long (*)(const char *name))kp.addr;
    unregister_kprobe(&kp);
    return 0;
}

static DEFINE_MERF_SUBSYSTEM(pgtable, 3,
                             MERF_SPECIFIC_INFO(__field(unsigned long, flags)),
                             MERF_INFO("pgd_alloc"), MERF_INFO("pmd_alloc"),
                             MERF_INFO("__pte_alloc"));

static int pte_user_ret_handler(struct kretprobe_instance *ri,
                                struct pt_regs *regs)
{
    unsigned long retval = regs_return_value(regs);

    if (!retval && (current->pid == target_pid))
        atomic_add(4096, &MERF_SYSTEM(pgtable).table[2].byte_alloc);

    return 0;
}

static struct kretprobe kp_pte_user;

#define merf_subsystem_init(name)                                              \
    __merf_subsystem_init(&MERF_SYSTEM(name).table[0],                         \
                          ARRAY_SIZE(MERF_SYSTEM(name).table))
static int __merf_subsystem_init(struct watchpoint_info *wp, size_t nr)
{
    int ret;
    kp_pte_user.kp.symbol_name = MERF_SYSTEM(pgtable).table[2].func_name;
    kp_pte_user.handler = pte_user_ret_handler;
    kp_pte_user.maxactive = 2;

    ret = register_kretprobe(&kp_pte_user);
    if (ret < 0) {
        pr_info("ERROR=\"%s\" cannot register %s\n", get_error(ret),
                MERF_SYSTEM(pgtable).table[2].func_name);
        return -EINVAL;
    }
    pr_merf("kretprobe register %s pid=%d\n",
            MERF_SYSTEM(pgtable).table[2].func_name, target_pid);
    return 0;
}

#define merf_subsystem_exit(name)                                              \
    __merf_subsystem_exit(&MERF_SYSTEM(name).table[0],                         \
                          ARRAY_SIZE(MERF_SYSTEM(name).table))
static int __merf_subsystem_exit(struct watchpoint_info *wp, size_t nr)
{
    unregister_kretprobe(&kp_pte_user);
    pr_merf("%d\n", atomic_read(&wp[2].byte_alloc));
    pr_merf("kretprobe unregister\n");
    return 0;
}

static int __init merf_init(void)
{
    size_t index;
    struct watchpoint_info *wpp;

    if (target_pid == -1)
        return -1;

    pr_info("array size %ld\n", ARRAY_SIZE(MERF_SYSTEM(pgtable).table));

    for (wpp = &MERF_SYSTEM(pgtable).table[0], index = 0;
         index < ARRAY_SIZE(MERF_SYSTEM(pgtable).table); ++index, wpp += 1)
        pr_info("plain %s\n", wpp->func_name);

    for_each_merf_wp(pgtable, wpp, index) pr_merf("%s\n", wpp->func_name);

    merf_subsystem_init(pgtable);

    return 0;
}

static void __exit merf_exit(void)
{
    merf_subsystem_exit(pgtable);
}

module_init(merf_init);
module_exit(merf_exit);

MODULE_LICENSE("GPL");
