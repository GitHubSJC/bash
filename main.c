#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/utsname.h>

#define MAXLENGTH 128

char *splitCmd[10] = {0};

void PrintInf(FILE *fp_cd)
{
    struct passwd *pw = getpwuid(getuid());
    if(pw == NULL)
    {
        perror("");
    }

    struct utsname name;
    int un = uname(&name);
    if(un == -1)
    {
        perror("");
    }

    char buf[MAXLENGTH] = {0};
    getcwd(buf, MAXLENGTH);

    if(strncmp(buf, "/root", 5) == 0)
    {
        buf[0] = '~';
        for(int i = 1; buf[i] != '\0'; i++)
        {
            buf[i] = buf[i + 4];
        }
    }

    char symbol = '#';
    if(pw->pw_uid != 0)
    {
        symbol = '$';
    }

    printf("\033[1;31m%s@%s\033[0m:\033[1;34m%s\033[0m%c ", pw->pw_name, name.nodename, buf, symbol);
}

void GetCmd(char *cmd, FILE *fp_command)
{
    memset(cmd, 0, MAXLENGTH);
    fgets(cmd, MAXLENGTH, stdin);
    fputs(cmd, fp_command);
    cmd[strlen(cmd) - 1] = '\0';
}

void SplitCmd(char *cmd)
{
    splitCmd[0] = strtok(cmd, " ");
    int i = 1;
    char *tempCmd;
    while((tempCmd = strtok(NULL, " ")) != NULL)
    {
        splitCmd[i] = tempCmd;
        i++;
    }
}

void WriteCd(FILE *fp_cd)
{
    char cd[MAXLENGTH] = {0};
    FILE *fp_wc = fopen("/root/code/bash/bash/data/tmpcd.txt", "w");
    if(fp_wc == NULL)
    {
        perror("");
    }

    getcwd(cd, MAXLENGTH);
    fputs(cd, fp_wc);
    fputs("\n", fp_wc);
    fflush(fp_wc);

    rewind(fp_cd);
    while(fgets(cd, MAXLENGTH, fp_cd))
    {
        fputs(cd, fp_wc);
        fflush(fp_wc);
    }

    fclose(fp_wc);
    fclose(fp_cd);

    int r = rename("/root/code/bash/bash/data/tmpcd.txt", "/root/code/bash/bash/data/cd.txt");
    if(r == -1)
    {
        perror("");
    }

    fp_cd = fopen("/root/code/bash/bash/data/cd.txt", "a+");
    if(fp_cd == NULL)
    {
        perror("");
    }
}

void Cd(FILE *fp_cd)
{
    char cd[MAXLENGTH] = {0};
    if(splitCmd[1] == NULL || *splitCmd[1] == '~')
    {
        chdir(getenv("HOME"));
        WriteCd(fp_cd);
    }
    else if(*splitCmd[1] == '-')
    {
        fgets(cd, MAXLENGTH, fp_cd);
        if(fgets(cd, MAXLENGTH, fp_cd) == NULL)
        {
            printf("bash: cd: OLDPWD 未设定\n");
        }
        else
        {
            printf("%s", cd);
            cd[strlen(cd) - 1] = '\0';
            int ch = chdir(cd);
            if(ch == -1)
            {
                perror("");
            }
            WriteCd(fp_cd);
        }
    }
    else
    {
        if(chdir(splitCmd[1]) == -1)
        {
            printf("bash: cd: %s: 没有那个文件或目录\n", splitCmd[1]);
        }
        else
        {
            WriteCd(fp_cd);
        }
    }
}

int main()//上下
{
    char cmd[MAXLENGTH] = {0};
    char path[MAXLENGTH] = "/root/code/bash/bash/";

    FILE *fp_command = fopen("/root/code/bash/bash/data/command.txt", "w+");
    if(fp_command == NULL)
    {
        perror("command.txt");
        exit(1);
    }

    FILE *fp_cd = fopen("/root/code/bash/bash/data/cd.txt", "w+");
    if(fp_cd == NULL)
    {
        perror("cd.txt");
        exit(1);
    }

    fputs(path, fp_cd);
    fputs("\n", fp_cd);
    fflush(fp_cd);

    while(1)
    {
        memset(cmd, 0, MAXLENGTH);

        PrintInf(fp_cd);
        GetCmd(cmd, fp_command);
        SplitCmd(cmd);

        if(splitCmd[0] == NULL)
        {

        }
        else if(strcmp(splitCmd[0], "exit") == 0)
        {
            fclose(fp_command);
            fclose(fp_cd);
            exit(0);
        }
        else if(strcmp(splitCmd[0], "cd") == 0)
        {
            Cd(fp_cd);
        }
        else
        {
            pid_t pid = fork();
            if(pid == 0)
            {
                strcat(path, splitCmd[0]);
                if(execv(path, splitCmd) == -1)
                {
                    printf("bash: ");
                    fflush(stdout);
                    perror(cmd);
                }
                exit(0);
            }
            else
            {
                wait(NULL);
            }
        }
        memset(splitCmd, 0, sizeof(splitCmd));
    }
    return 0;
}
