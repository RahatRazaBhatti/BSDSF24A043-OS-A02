/*
 * Programming Assignment 02: ls utility
 * Version 1.1.0
 * Features: Basic listing and long listing (-l)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>

void do_ls(const char *dir);
void long_listing(const char *dir);

int main(int argc, char *argv[])
{
    int option;
    int long_format = 0;

    while ((option = getopt(argc, argv, "l")) != -1)
    {
        switch (option)
        {
            case 'l':
                long_format = 1;
                break;

            default:
                fprintf(stderr, "Usage: %s [-l] [directory ...]\n",
                        argv[0]);
                return 1;
        }
    }

    if (optind == argc)
    {
        if (long_format)
            long_listing(".");
        else
            do_ls(".");
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);

            if (long_format)
                long_listing(argv[i]);
            else
                do_ls(argv[i]);

            puts("");
        }
    }

    return 0;
}

void do_ls(const char *dir)
{
    DIR *dp;
    struct dirent *entry;

    dp = opendir(dir);

    if (dp == NULL)
    {
        perror(dir);
        return;
    }

    errno = 0;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        printf("%s\n", entry->d_name);
        errno = 0;
    }

    if (errno != 0)
        perror("readdir");

    closedir(dp);
}

void long_listing(const char *dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat file_stat;
    char path[PATH_MAX];

    dp = opendir(dir);

    if (dp == NULL)
    {
        perror(dir);
        return;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        struct passwd *owner;
        struct group *group;
        struct tm *time_info;
        char time_string[64];
        char permissions[11];
        const char *name = entry->d_name;

        if (name[0] == '.')
            continue;

        if (snprintf(path, sizeof(path), "%s/%s", dir, name)
                >= (int)sizeof(path))
        {
            fprintf(stderr, "Path too long: %s/%s\n", dir, name);
            continue;
        }

        if (stat(path, &file_stat) == -1)
        {
            perror(path);
            continue;
        }

        /* File type */
        if (S_ISDIR(file_stat.st_mode))
            permissions[0] = 'd';
        else if (S_ISLNK(file_stat.st_mode))
            permissions[0] = 'l';
        else if (S_ISCHR(file_stat.st_mode))
            permissions[0] = 'c';
        else if (S_ISBLK(file_stat.st_mode))
            permissions[0] = 'b';
        else if (S_ISFIFO(file_stat.st_mode))
            permissions[0] = 'p';
        else if (S_ISSOCK(file_stat.st_mode))
            permissions[0] = 's';
        else
            permissions[0] = '-';

        /* Owner permissions */
        permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
        permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
        permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';

        /* Group permissions */
        permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
        permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
        permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';

        /* Other users' permissions */
        permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
        permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
        permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
        permissions[10] = '\0';

        owner = getpwuid(file_stat.st_uid);
        group = getgrgid(file_stat.st_gid);

        time_info = localtime(&file_stat.st_mtime);

        if (time_info != NULL)
            strftime(time_string, sizeof(time_string),
                     "%b %d %H:%M", time_info);
        else
            strcpy(time_string, "Unknown time");

        printf("%s %3lu %-8s %-8s %8ld %s %s\n",
               permissions,
               (unsigned long)file_stat.st_nlink,
               owner ? owner->pw_name : "unknown",
               group ? group->gr_name : "unknown",
               (long)file_stat.st_size,
               time_string,
               name);
    }

    closedir(dp);
}
