#define _DEFAULT_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

#define LENGTH 128

void Loop(char *path)
{
    char curPath[LENGTH];
    DIR *odir = opendir(path);
    struct dirent *dir;
    struct stat st;
    while((dir = readdir(odir)) != NULL)
    {
        if(strcmp(dir->d_name, "." ) != 0 && strcmp(dir->d_name, "..") != 0)
        {
            strcpy(curPath, path);
            strcat(curPath, "/");
            strcat(curPath, dir->d_name);
            lstat(curPath, &st);
            if(S_ISDIR(st.st_mode))
            {
                Loop(curPath);
            }
            else
            {
                unlink(curPath);
            }
        }
    }
    rmdir(path);
}

void R(char *argv[], int i)
{
    char path[LENGTH] = {0};
    struct stat st;
    strcat(path, argv[i]);
    lstat(path, &st);
    if(S_ISDIR(st.st_mode))
    {
        Loop(path);
    }
    else
    {
        unlink(argv[i]);
    }
}

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        if(strcmp(argv[1], "-f") == 0)
        {
            if(argc > 2)
            {
                for(int i = 2; i < argc; i++)
                {
                    if(unlink(argv[i]) == -1)
                    {
                        if(errno == 21)
                        {
                            printf("rm: 无法删除'%s': 是一个目录\n", argv[i]);
                        }
                    }
                }
            }
            else
            {
                printf("mkdir: 缺少操作数\nTry 'mkdir --help' for more information.\n");
            }
        }
        else if(strcmp(argv[1], "-r") == 0)
        {
            if(argc > 2)
            {
                for(int i = 2; i < argc; i++)
                {
                    R(argv, i);
                }
            }
            else
            {
                printf("rm: 缺少操作数\nTry 'rm --help' for more information.\n");
            }
        }
        else if(strcmp(argv[1], "--help") == 0)
        {
            printf("       -f, --force           强制删除。忽略不存在的文件，不提示确认\n"
                   "       -r, -R, --recursive   递归删除目录及其内容");
        }
        else if(argv[1][0] == '-' && strlen(argv[1]) != 1)
        {
            printf("mkdir: 不适用的选项 -- %c\nTry 'mkdir --help' for more information.\n", argv[1][0]);
        }
        else
        {
            for(int i = 1; i < argc; i++)
            {
                if(unlink(argv[i]) == -1)
                {
                    if(errno == 2)
                    {
                        printf("rm: 无法删除'%s': 没有那个文件或目录\n", argv[i]);
                    }
                    if(errno == 21)
                    {
                        printf("rm: 无法删除'%s': 是一个目录\n", argv[i]);
                    }
                }
            }
        }
    }
    else
    {
        printf("rm: 缺少操作数\nTry 'rm --help' for more information.\n");
    }

    return 0;
}
