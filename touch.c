#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAXLENGTH 128

int main(int argc, char *argv[])
{
    char path[MAXLENGTH] = {0};
    getcwd(path, MAXLENGTH);

    if(argc > 1)
    {
        if(strcmp(argv[1], "-a") == 0)
        {
            /*if(utimensat(AT_FDCWD, argv[2], NULL, 0) == -1)
            {
                if(errno==2)
                {

                }
            }*/
        }
        else if(argv[1][0] == '-' && strlen(argv[1]) != 1)
        {
            printf("mkdir: 不适用的选项 -- %c\nTry 'mkdir --help' for more information.\n", argv[1][0]);
        }
        else
        {
            strcat(path, "/");
            strcat(path, argv[1]);
            FILE *fp;
            fp = fopen(path, "r");
            if(fp == NULL)
            {
                if(errno == 2)//没有文件
                {
                    fp = fopen(path, "w");
                    if(fp == NULL)
                    {
                        perror("");
                    }
                    else
                    {
                        fclose(fp);
                    }
                }
                else
                {
                    perror("");
                }
            }
            else
            {
                fclose(fp);

                utimensat(AT_FDCWD, path, NULL, 0);
            }
        }
    }
    else
    {
        printf("touch: 缺少了文件操作数\n");
    }

    return 0;
}
