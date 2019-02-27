#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if(argc == 1 || strcmp(argv[1], "-L") == 0)//链接为解决
    {
        char buf[128] = {0};
        getcwd(buf, 128);
        printf("%s\n", buf);
    }
    else if(strcmp(argv[1], "-P") == 0)
    {
        char buf[128] = {0};
        getcwd(buf, 128);
        printf("%s\n", buf);
    }
    else
    {
        printf("bash: pwd: %s: 无效选项\npwd: 用法:pwd [-LP]\n", argv[1]);
    }

    return 0;
}
