#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define LENGTH 128

void Get(char *downLoadPath)
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("连接初始化失败\n");
        exit(1);
    }

    struct sockaddr_in data;
    data.sin_family = AF_INET;
    data.sin_port = htons(21);
    data.sin_addr.s_addr = inet_addr("127.0.0.1");

    int con = connect(sockfd, (struct sockaddr*)&data, sizeof(data));
    if(con == -1)
    {
        printf("服务器连接失败\n");
        exit(1);
    }

    char buf[4096] = {0};

    FILE *fp = fopen(downLoadPath, "w");
    while(1)
    {
        ssize_t rec = recv(sockfd, buf, 4096, 0);
        if(rec < 0)
        {
            printf("获取文件失败\n");
            fclose(fp);
            exit(1);
        }
        else if(rec == 0)
        {
            printf("success\n");
            break;
        }

        if(strlen(buf) < 4096)
        {
            fwrite(buf, strlen(buf), 1, fp);
        }
        else
        {
            fwrite(buf, 4096, 1, fp);
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("服务器连接初始化失败\n");
        exit(1);
    }

    struct sockaddr_in ser;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(20);
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");

    int con = connect(sockfd, (struct sockaddr*)&ser, sizeof(ser));
    if(con == -1)
    {
        printf("connect: 服务器连接失败\n");
        exit(1);
    }

    char buf[1] = {0};

    ssize_t sen = send(sockfd, argv[1], strlen(argv[1]), 0);
    if(sen == -1)
    {
        printf("send: 服务器连接失败\n");
        exit(1);
    }

    ssize_t rec = recv(sockfd, buf, 1, 0);
    if(rec == -1)
    {
        printf("recv: 服务器连接失败\n");
        exit(1);
    }

    if(strncmp(buf, "1", 1) == 0)
    {
        char fileName[20] = {0};
        int i = strlen(argv[1]) - 1;
        printf("0\n");
        for(; argv[1][i] != '/'; i--) {}
        printf("1\n");
        for(int j = 0; argv[1][i] != '\0'; j++)
        {
            fileName[j] = argv[1][i];
            i++;
        }
        printf("2\n");

        char downLoadPath[LENGTH] = {"/root/下载/"};
        strcat(downLoadPath, fileName);

        Get(downLoadPath);
    }
    else
    {
        printf("%s不存在\n", argv[1]);
    }

    close(sockfd);

    return 0;
}
