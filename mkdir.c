#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define MAXLENGTH 128

int Create(int i, int argc, char *argv[])
{
    int mk = mkdir(argv[i], S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if(mk == -1)
    {
        if(errno == 2)
        {
            printf("mkdir: 无法创建目录\"%s\": 没有那个文件或目录\n", argv[i]);
        }
        else if(errno == 17)
        {
            printf("mkdir: 无法创建目录\"%s\": 文件已存在\n", argv[i]);
        }
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        if(strcmp(argv[1], "-p") == 0)
        {
            //
        }
        else if(strcmp(argv[1], "-v") == 0)
        {
            if(argc > 2)
            {
                for(int i = 2; i < argc; i++)
                {
                    if(Create(i, argc, argv) == 0)
                    {
                        printf("mkdir: 已创建目录 '%s'", argv[i]);
                    }
                }
            }
            else
            {
                printf("mkdir: 缺少操作数\nTry 'mkdir --help' for more information.\n");
            }
        }
        else if(strcmp(argv[1], "--help") == 0)
        {
            printf("       -p, --parents         需要时创建目标目录的上层目录，但即使这些目录已存在也不当作错误处理\n"
                   "       -v, --verbose         每次创建新目录都显示信息\n");
        }
        else if(argv[1][0] == '-' && strlen(argv[1]) != 1)
        {
            printf("mkdir: 不适用的选项 -- %c\nTry 'mkdir --help' for more information.\n", argv[1][0]);
        }
        else
        {
            for(int i = 1; i < argc; i++)
            {
                Create(i, argc, argv);
            }
        }
    }
    else
    {
        printf("mkdir: 缺少操作数\nTry 'mkdir --help' for more information.\n");
    }

    return 0;
}
