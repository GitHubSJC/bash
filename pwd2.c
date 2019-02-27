#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

void Read(long inode)
{
    DIR *odir = opendir(".");
    struct dirent *rdir;
    while((rdir = readdir(odir)) != NULL)
    {
        if(strcmp(rdir->d_name, ".") == 0)
        {
            inode = rdir->d_ino;
            int c = chdir("..");
            assert(c != -1);
        }
        if(rdir->d_ino == inode)
        {
            Read(inode);
        }
        if(rdir->d_ino == inode)
        {
            printf("/%s", rdir->d_name);
        }
    }
}

int main()
{
    long inode = -1;
    Read(inode);

    return 0;
}
