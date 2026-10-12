/*
 * Operating Systems Programming Assignment 02
 * Features:
 * 1. Basic Listing
 * 2. Long Listing (-l)
 * 3. Column Display (default)
 * 4. Horizontal Display (-x)
 * 5. Alphabetical Sorting
 * 6. Colorized Output
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>
#include <sys/ioctl.h>

#define DEFAULT_WIDTH 80
#define COLUMN_SPACING 2

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_CYAN    "\033[1;36m"

void do_ls(const char *dir, int horizontal);
void long_listing(const char *dir);
int get_terminal_width(void);
int compare_names(const void *a, const void *b);
const char *get_file_color(const char *path, const char *name);

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

            putchar('\n');
        }
    }

    return 0;
}

/* Compare filenames alphabetically */
int compare_names(const void *a, const void *b)
{
    const char *name_a = *(const char **)a;
    const char *name_b = *(const char **)b;

    return strcmp(name_a, name_b);
}

/* Return color according to file type */
const char *get_file_color(const char *path, const char *name)
{
    struct stat file_stat;
    char full_path[PATH_MAX];

    if (snprintf(full_path, sizeof(full_path), "%s/%s", path, name)
        >= (int)sizeof(full_path))
        return COLOR_RESET;

    if (lstat(full_path, &file_stat) == -1)
        return COLOR_RESET;

    if (S_ISLNK(file_stat.st_mode))
        return COLOR_CYAN;

    if (S_ISDIR(file_stat.st_mode))
        return COLOR_BLUE;

    if (file_stat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        return COLOR_GREEN;

    return COLOR_RESET;
}

/* Get terminal width */
int get_terminal_width(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
        ws.ws_col > 0)
        return ws.ws_col;

    return DEFAULT_WIDTH;
}

/* Display directory contents */
void do_ls(const char *dir, int horizontal)
{
    DIR *dp;
    struct dirent *entry;
    char **names;
    size_t count = 0;
    size_t capacity = 16;
    size_t max_length = 0;
    size_t rows;
    size_t columns;
    size_t column_width;
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

    /* Read filenames into a dynamic array */
    while ((entry = readdir(dp)) != NULL)
    {
        size_t length;

        /* Skip hidden files */
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

    /* Sort filenames alphabetically */
    qsort(names, count, sizeof(char *), compare_names);

    terminal_width = get_terminal_width();
    column_width = max_length + COLUMN_SPACING;

    columns = (size_t)terminal_width / column_width;

    if (columns < 1)
        columns = 1;

    if (columns > count)
        columns = count;

    /* Horizontal display: left to right */
    if (horizontal)
    {
        size_t current_width = 0;

        for (size_t i = 0; i < count; i++)
        {
            size_t length = strlen(names[i]);
            const char *color = get_file_color(dir, names[i]);

            if (current_width > 0 &&
                current_width + COLUMN_SPACING + length >
                (size_t)terminal_width)
            {
                putchar('\n');
                current_width = 0;
            }

            printf("%s%s%s", color, names[i], COLOR_RESET);
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
        /* Default display: down then across */
        rows = (count + columns - 1) / columns;

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < columns; col++)
            {
                size_t index = col * rows + row;

                if (index < count)
                {
                    const char *color =
                        get_file_color(dir, names[index]);

                    printf("%s%-*s%s",
                           color,
                           (int)column_width,
                           names[index],
                           COLOR_RESET);
                }
            }

            putchar('\n');
        }
    }

    for (size_t i = 0; i < count; i++)
        free(names[i]);

    free(names);
}

/* Display detailed file information */
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

        if (lstat(path, &file_stat) == -1)
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

        const char *color = COLOR_RESET;

        if (S_ISDIR(file_stat.st_mode))
            color = COLOR_BLUE;
        else if (S_ISLNK(file_stat.st_mode))
            color = COLOR_CYAN;
        else if (file_stat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
            color = COLOR_GREEN;

        printf("%s %3lu %-8s %-8s %8ld %s %s%s%s\n",
               permissions,
               (unsigned long)file_stat.st_nlink,
               owner ? owner->pw_name : "unknown",
               group ? group->gr_name : "unknown",
               (long)file_stat.st_size,
               time_string,
               color,
               name,
               COLOR_RESET);
    }

    closedir(dp);
}
