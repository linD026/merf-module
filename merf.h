#ifndef __MERF_H__

struct watchpoint_info {
    const char *func_name;
    atomic_t byte_alloc;
};

#define MERF_INFO(name)                                                        \
    {                                                                          \
        .func_name = name, .byte_alloc = ATOMIC_INIT(0)                        \
    }

#undef __field
#define __field(type, name) type name;

#define MERF_SPECIFIC_INFO(info...)                                            \
    struct {                                                                   \
        info                                                                   \
    }

#define DEFINE_MERF_SUBSYSTEM(subsystem, nr, specific_info, wpinfo...)         \
    struct merf_##subsystem {                                                  \
        specific_info;                                                         \
        struct watchpoint_info table[nr];                                      \
    } merf_##subsystem = { .table = { wpinfo } }

#define MERF_SYSTEM(name) merf_##name

#define for_each_merf_wp(name, wpp, index)                                     \
    for (wpp = &MERF_SYSTEM(name).table[0], index = 0;                         \
         index < ARRAY_SIZE(MERF_SYSTEM(name).table); ++index, wpp += 1)

#endif /* __MERF_H__ */
