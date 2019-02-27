//#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define MAXLENGTH 128

void PrintFileName( struct dirent **nameList, int num)
{
    char filePath[MAXLENGTH] = {"."}; //malloc
    strcat(filePath, "/");
    strcat(filePath, nameList[num]->d_name);

    struct stat st;
    lstat(filePath, &st);

    int type = 0;

    if(S_ISDIR(st.st_mode))
    {
        type = 1;
    }
    else if(S_ISLNK(st.st_mode))
    {
        type = 2;
    }
    else if((st.st_mode & S_IRWXU)&S_IXUSR)
    {
        type = 3;
    }

    switch(type)
    {
    case 0:
        printf("%s", nameList[num]->d_name);
        break;
    case 1:
        printf("\033[1;34m%s\033[0m", nameList[num]->d_name);
        break;
    case 2:
        printf("\033[1;36m%s\033[0m", nameList[num]->d_name);
        break;
    case 3:
        printf("\033[1;32m%s\033[0m", nameList[num]->d_name);
        break;
    }
}

void F(struct dirent **nameList, int num, int flag)
{
    char filePath[MAXLENGTH] = {"."}; //malloc
    strcat(filePath, "/");
    strcat(filePath, nameList[num]->d_name);

    struct stat st;
    lstat(filePath, &st);

    if(flag == 1)
    {
        printf("  ");
    }
    PrintFileName(nameList, num);
    if(S_ISDIR(st.st_mode))
    {
        printf("/  ");
    }
    else if(S_ISLNK(st.st_mode))
    {
        printf("@  ");
    }
    else if(S_ISFIFO(st.st_mode))
    {
        printf("|  ");
    }
    else if(S_ISSOCK(st.st_mode))
    {
        printf("=  ");
    }
    else if((st.st_mode & S_IRWXU)&S_IXUSR)
    {
        printf("*  ");
    }

}

void L(struct dirent **nameList, int num, int n, int flag, int *maxLink, int *maxPwname, int *maxGrname, int *maxSize)
{
    if(flag == 0)
    {
        struct stat countSt;

        int tmpSize = 0;
        int tmptotal = 0;
        int tmpLength = 0;

        for(int i = 0; i < n; i++)
        {
            if(nameList[i]->d_name[0] != '.')
            {
                char tmpPath[MAXLENGTH] = {"."};
                strcat(tmpPath, "/");
                strcat(tmpPath, nameList[i]->d_name);
                lstat(tmpPath, &countSt);

                tmptotal += countSt.st_blocks / 2;

                while(countSt.st_nlink != 0)//link
                {
                    countSt.st_nlink /= 10;
                    tmpLength++;
                }
                if(*maxLink < tmpLength)
                {
                    *maxLink = tmpLength;
                }
                tmpLength = 0;

                struct passwd *tmpPw = getpwuid(countSt.st_uid);//username
                if(*maxPwname < strlen(tmpPw->pw_name))
                {
                    *maxPwname = strlen(tmpPw->pw_name);
                }

                struct group *tmpGr = getgrgid(countSt.st_gid);//groupname
                if(*maxGrname < strlen(tmpGr->gr_name))
                {
                    *maxGrname = strlen(tmpGr->gr_name);
                }

                while(countSt.st_size != 0)//size
                {
                    countSt.st_size /= 10;
                    tmpSize++;
                }
                if(*maxSize < tmpSize)
                {
                    *maxSize = tmpSize;
                }
                tmpSize = 0;
            }
        }
        printf("总用量 %d\n", tmptotal);
    }

    char filePath[MAXLENGTH] = {"."};
    strcat(filePath, "/");
    strcat(filePath, nameList[num]->d_name);

    struct stat st;
    lstat(filePath, &st);

    if(S_ISSOCK(st.st_mode))
    {
        printf("s");
    }
    else if(S_ISDIR(st.st_mode))
    {
        printf("d");
    }
    else if(S_ISLNK(st.st_mode))
    {
        printf("l");
    }
    else if(S_ISBLK(st.st_mode))
    {
        printf("b");
    }
    else if(S_ISCHR(st.st_mode))
    {
        printf("c");
    }
    else if(S_ISFIFO(st.st_mode))
    {
        printf("p");
    }
    else if(S_ISREG(st.st_mode))
    {
        printf("-");
    }

    printf((st.st_mode & S_IRWXU)&S_IRUSR ? "r" : "-");
    printf((st.st_mode & S_IRWXU)&S_IWUSR ? "w" : "-");
    printf((st.st_mode & S_IRWXU)&S_IXUSR ? "x" : "-");
    printf((st.st_mode & S_IRWXG)&S_IRGRP ? "r" : "-");
    printf((st.st_mode & S_IRWXG)&S_IWGRP ? "w" : "-");
    printf((st.st_mode & S_IRWXG)&S_IXGRP ? "x" : "-");
    printf((st.st_mode & S_IRWXO)&S_IROTH ? "r" : "-");
    printf((st.st_mode & S_IRWXO)&S_IWOTH ? "w" : "-");
    printf((st.st_mode & S_IRWXO)&S_IXOTH ? "x " : "- ");

    printf("%*ld ", *maxLink, st.st_nlink);

    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    printf("%*s %*s ", *maxPwname, pw->pw_name, *maxGrname, gr->gr_name);

    printf("%*ld ", *maxSize, st.st_size);

    struct tm *t;
    t = gmtime(&st.st_mtime);
    char time[16] = {0};
    strftime(time, 16, "%m月 %d %H:%M", t);
    printf("%s ", time);

    PrintFileName(nameList, num);
    printf("\n");
}

void R(char *path, int flag)
{
    struct dirent **nameList;
    struct stat countSt;
    int n = scandir(".", &nameList, 0, alphasort);

    for(int i = 0; i < n; i++)
    {
        if(nameList[i]->d_name[0] != '.')
        {
            if(flag == 1)
            {
                printf("  ");
            }
            flag = 1;
            PrintFileName(nameList, i);
        }
    }
    printf("\n");

    for(int i = 0; i < n; i++)
    {
        if(nameList[i]->d_name[0] != '.')
        {
            char tmpPath[MAXLENGTH] = {"."};
            strcat(tmpPath, "/");
            strcat(tmpPath, nameList[i]->d_name);
            lstat(tmpPath, &countSt);

            if(S_ISDIR(countSt.st_mode))
            {
                char curPath[MAXLENGTH] = {0};
                chdir(nameList[i]->d_name);
                getcwd(curPath, MAXLENGTH);

                printf("\n.");
                for(int j = strlen(path); j < strlen(curPath); j++)
                {
                    printf("%c", curPath[j]);
                }
                printf(":\n");

                R(path, flag);
                chdir("..");
            }
        }
    }
}

void S(struct dirent **nameList, int num, int n, int flag)
{
    if(flag == 0)
    {
        char tmpPath[MAXLENGTH] = {"."};
        struct stat countSt;
        int tmptotal = 0;

        for(int i = 0; i < n; i++)
        {
            if(nameList[i]->d_name[0] != '.')
            {
                strcat(tmpPath, "/");
                strcat(tmpPath, nameList[i]->d_name);
                lstat(tmpPath, &countSt);

                tmptotal += countSt.st_blocks / 2;
            }
        }
        printf("总用量 %d\n", tmptotal);
    }

    char filePath[MAXLENGTH] = {"."}; //malloc
    strcat(filePath, "/");
    strcat(filePath, nameList[num]->d_name);

    struct stat st;
    lstat(filePath, &st);

    if(flag == 1)
    {
        printf("   ");
    }
    printf("%ld ", st.st_blocks / 2);
    PrintFileName(nameList, num);
}

int main(int argc, char *argv[])//排序
{
    char path[MAXLENGTH] = {0};
    getcwd(path, MAXLENGTH);

    int flag = 0;
    int nFlag = 0;
    int maxLink = 0;
    int maxPwname = 0;
    int maxGrname = 0;
    int maxSize = 0;

    if(argc > 1 && strcmp(argv[1], "-R") == 0)
    {
        nFlag = 1;
        printf(".:\n");
        R(path, flag);
    }
    else
    {
        struct dirent **nameList;
        int n = scandir(".", &nameList, 0, alphasort);
        for(int i = 0; i < n; i++)
        {
            if(argc > 1)
            {
                if(strcmp(argv[1], "-a") == 0)
                {
                    if(flag == 1)
                    {
                        printf("  ");
                    }
                    flag = 1;
                    PrintFileName(nameList, i);
                }
                else if(strcmp(argv[1], "-A") == 0)
                {
                    if(strcmp(nameList[i]->d_name, ".") != 0 && strcmp(nameList[i]->d_name, "..") != 0)
                    {
                        if(flag == 1)
                        {
                            printf("  ");
                        }
                        flag = 1;
                        PrintFileName(nameList, i);
                    }
                }
                else if(strcmp(argv[1], "-F") == 0)
                {
                    if(nameList[i]->d_name[0] != '.')
                    {
                        F(nameList, i, flag);
                        flag = 1;
                    }
                }
                else if(strcmp(argv[1], "-l") == 0)
                {
                    nFlag = 1;
                    if(nameList[i]->d_name[0] != '.')
                    {
                        L(nameList, i, n, flag, &maxLink, &maxPwname, &maxGrname, &maxSize);
                        flag = 1;
                    }
                }

                else if(strcmp(argv[1], "-s") == 0)
                {
                    if(nameList[i]->d_name[0] != '.')
                    {
                        S(nameList, i, n, flag);
                        flag = 1;
                    }
                }
                else if(strcmp(argv[1], "--help") == 0)
                {
                    printf("       -a, --all                     不隐藏任何以. 开始的项目\n"
                           "       -A, --almost-all              列出除. 及.. 以外的任何项目\n"
                           "       -F, --classify                加上文件类型的指示符号(*/=@| 其中一个\n"
                           "       -l                            使用较长格式列出信息\n"
                           "       -R, --recursive               递归显示子目录\n"
                           "       -s, --size                    以块数形式显示每个文件分配的尺寸\n");
                    break;
                }
                else//输入错误
                {
                    printf("ls: 不适用的选项 -- V\nTry 'ls --help' for more information.\n");
                    exit(1);
                }
            }
            else
            {
                if(nameList[i]->d_name[0] != '.')
                {
                    if(flag == 1)
                    {
                        printf("  ");
                    }
                    flag = 1;
                    PrintFileName(nameList, i);
                }
            }
            free(nameList[i]);
        }
        free(nameList);
    }

    if(nFlag == 0)
    {
        printf("\n");
    }

    return 0;
}
