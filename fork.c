#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    getchar();
    sleep(1);
    fork();
    return 0;
}
