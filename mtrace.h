#ifndef __MTRACE_H__

struct watchpoint_info {
    const char *func_name;
    atomic_t byte_alloc;
};

#define MTRACE_INFO(name)                                                      \
    {                                                                          \
        .func_name = name, .byte_alloc = ATOMIC_INIT(0)                        \
    }

#undef __field
#define __field(type, name) type name;

#define MTRACE_SPECIFIC_INFO(info...)                                          \
    struct {                                                                   \
        info                                                                   \
    }

#define DEFINE_MTRACE_SUBSYSTEM(subsystem, nr, specific_info, wpinfo...)       \
    struct mtrace_##subsystem {                                                \
        specific_info;                                                         \
        struct watchpoint_info table[nr];                                      \
    } mtrace_##subsystem = { .table = { wpinfo } }

#define MTRACE_SYSTEM(name) mtrace_##name

#define for_each_mtrace_wp(name, wpp, index)                                   \
    for (wpp = &MTRACE_SYSTEM(name).table[0], index = 0;                       \
         index < ARRAY_SIZE(MTRACE_SYSTEM(name).table); ++index, wpp += 1)

#endif /* __MTRACE_H__ */
