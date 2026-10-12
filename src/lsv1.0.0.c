/*
 * Operating Systems Programming Assignment 02
 * Features: Basic Listing, Long Listing (-l),
 * Column Display, Horizontal Display (-x)
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
#include <sys/ioctl.h>

#define DEFAULT_WIDTH 80
#define COLUMN_SPACING 2

void do_ls(const char *dir, int horizontal);
void long_listing(const char *dir);
int get_terminal_width(void);

int main(int argc, char *argv[])
{
    int option;
    int long_format = 0;
    int horizontal = 0;

    while ((option = getopt(argc, argv, "lx")) != -1)
    {
        switch (option)
        {
            case 'l':
                long_format = 1;
                break;
            case 'x':
                horizontal = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-x] [directory ...]\n",
                        argv[0]);
                return 1;
        }
    }

    if (optind == argc)
    {
        if (long_format)
            long_listing(".");
        else
            do_ls(".", horizontal);
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);

            if (long_format)
                long_listing(argv[i]);
            else
                do_ls(argv[i], horizontal);

            puts("");
        }
    }

    return 0;
}

int get_terminal_width(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
        ws.ws_col > 0)
        return ws.ws_col;

    return DEFAULT_WIDTH;
}

void do_ls(const char *dir, int horizontal)
{
    DIR *dp;
    struct dirent *entry;
    char **names;
    size_t count = 0, capacity = 16, max_length = 0;
    size_t rows, columns, column_width;
    int terminal_width;
    char **temp;

    dp = opendir(dir);
    if (dp == NULL)
    {
        perror(dir);
        return;
    }

    names = malloc(capacity * sizeof(char *));
    if (names == NULL)
    {
        perror("malloc");
        closedir(dp);
        return;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        size_t length;
        if (entry->d_name[0] == '.')
            continue;

        length = strlen(entry->d_name);

        if (count == capacity)
        {
            capacity *= 2;
            temp = realloc(names, capacity * sizeof(char *));
            if (temp == NULL)
            {
                perror("realloc");
                for (size_t i = 0; i < count; i++)
                    free(names[i]);
                free(names);
                closedir(dp);
                return;
            }
            names = temp;
        }

        names[count] = malloc(length + 1);
        if (names[count] == NULL)
        {
            perror("malloc");
            for (size_t i = 0; i < count; i++)
                free(names[i]);
            free(names);
            closedir(dp);
            return;
        }

        strcpy(names[count], entry->d_name);
        if (length > max_length)
            max_length = length;
        count++;
    }

    closedir(dp);

    if (count == 0)
    {
        free(names);
        return;
    }

    terminal_width = get_terminal_width();
    column_width = max_length + COLUMN_SPACING;
    columns = (size_t)terminal_width / column_width;

    if (columns < 1)
        columns = 1;
    if (columns > count)
        columns = count;

    if (horizontal)
    {
        size_t current_width = 0;

        for (size_t i = 0; i < count; i++)
        {
            size_t length = strlen(names[i]);

            if (current_width > 0 &&
                current_width + column_width > (size_t)terminal_width)
            {
                putchar('\n');
                current_width = 0;
            }

            printf("%s", names[i]);
            current_width += length;

            if (i + 1 < count)
            {
                printf("%*s", COLUMN_SPACING, "");
                current_width += COLUMN_SPACING;
            }
        }
        putchar('\n');
    }
    else
    {
        rows = (count + columns - 1) / columns;

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < columns; col++)
            {
                size_t index = col * rows + row;
                if (index < count)
                    printf("%-*s", (int)column_width, names[index]);
            }
            putchar('\n');
        }
    }

    for (size_t i = 0; i < count; i++)
        free(names[i]);
    free(names);
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

        permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
        permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
        permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
        permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
        permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
        permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
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
