#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAXLENGTH 128

int main(int argc, char *argv[])
{
    char path[MAXLENGTH] = {0};
    getcwd(path, MAXLENGTH);
    strcat(path, "/");
    strcat(path, argv[1]);

    int mk = rmdir(path);
    if(mk == -1)
    {
        printf("rmdir: 删除 'a' 失败: 目录非空\n");
    }

    return 0;
}
