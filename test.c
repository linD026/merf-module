#include "mtrace.h"

static DEFINE_MTRACE_SUBSYSTEM(pgtable, 3,
    MTRACE_SPECIFIC_INFO(
        __field(unsigned long, flags)
    ),
    MTRACE_INFO("pgd_alloc"),
    MTRACE_INFO("pmd_alloc"),
    MTRACE_INFO("__pte_alloc_one")
);

int main(void)
{
    return 0;
}
