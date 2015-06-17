/*
 * =====================================================================================
 *
 *       Filename:  rtee.c
 *
 *    Description:  Rotating Tee
 *
 *        Version:  0.1
 *        Created:  06/16/2015 07:44:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jeff Parent (jeff@commentedcode.org),
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define BSIZE (8 * 1024)

/* Print the usage line and exit */
void print_usage_and_die (void)
{
  (void)fprintf(stderr, "Usage: rtee [OPTIONS]... <FILE>\n");
  exit(1);
}

/*  Print the usage with documentation and exit */
void print_help_and_die (void)
{
  (void)fprintf(stderr, "Usage: rtee [OPTIONS]... <FILE>\n"
                        "Copy standard input to FILE until a fixed size and roll over\n"
                        "\n"
                        "    -a        append to the given FILE\n"
                        "    -b num    max number of bytes per file (default 1M)\n"
                        "    -f num    max number of previous files before roll over (default 4)\n"
                        "\n"
                        "The FILE name is appended with the count starting at FILE.0 and incrementing.\n"
                        "\n");
  exit(0);
}

/*  Print error message and exit */
void print_error_and_die (const char* err)
{
  (void)fprintf(stderr, "Error: %s\n", err);
  exit(1);
}

/*  Open  file "name.count" */
int open_file (char *name, unsigned long count, char append)
{
  char *newName;
  int fd;

  if ((newName = malloc((size_t)(strlen(name) + 11))) == NULL)
    print_error_and_die("Unable to malloc");

  sprintf(newName, "%s.%lu\0", name, count);

  fd = open(newName,
      O_CREAT | O_WRONLY | (append == 1 ? O_APPEND : O_TRUNC),
      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

  if (fd == -1)
    print_error_and_die("Unable to open file");

  free(newName);

  return fd;
}

/*  Fork process, child delete file */
void del_file (char *name, unsigned long count)
{
  int status;
  char *newName;
  pid_t child = fork();

  if (child == 0)
  {
    // Child deletes file as it may take a while
    if ((newName = malloc((size_t)(strlen(name) + 11))) == NULL)
      print_error_and_die("Unable to malloc");

    sprintf(newName, "%s.%lu\0", name, count);

    status = remove(newName);

    if (status != 0)
      fprintf(stderr, "Error deleting file");

    free(newName);

    exit(0);
  }
}

int main (int argc, char *argv[])
{
  unsigned long max_bytes = 1000000;
  unsigned long max_files = 4;
  unsigned int byte_count = 0;
  unsigned int file_count = 0;
  unsigned int del_count = 0;
  int fd, ch, exitval, n, rval, wval;
  char append = 0;
  char *end;
  char *wrt_pt;
  char *buf;
  size_t read_buffer_size = BSIZE;

  while ((ch = getopt(argc, argv, "hab:f:")) != -1)
    switch ((char)ch)
    {
      case 'h':
        print_help_and_die();
        break;

      case 'a':
        append = 1;
        break;

      case 'b':
        max_bytes = strtoul(optarg, &end, 10);
        if (errno != 0)
          print_usage_and_die();
        break;

      case 'f':
        max_files = strtoul(optarg, &end, 10);
        if (errno != 0)
          print_usage_and_die();
        break;

      default:
        print_usage_and_die();
        break;
    }

  if (optind >= argc)
    print_usage_and_die();

  fd = open_file(argv[optind], file_count, append);

  if (max_bytes < BSIZE)
    read_buffer_size = (size_t)max_bytes;

  if ((buf = (char *)malloc((size_t)read_buffer_size))  == NULL)
    print_error_and_die("Unable to malloc");

  while ((rval = read(STDIN_FILENO, buf, read_buffer_size)) > 0)
  {
    // Write to stdout
    n = rval;
    wrt_pt = buf;
    do
    {
      if ((wval = write(STDOUT_FILENO, wrt_pt, n)) == -1)
        print_error_and_die("write error: stdout");

      wrt_pt += wval;
    } while (n -= wval);

    // check if new file is needed
    if (byte_count + rval > max_bytes)
    {
      file_count += 1;
      close(fd);
      fd = open_file(argv[optind], file_count, append);
      byte_count = 0;

      // Check if file needs to be deleted
      if (file_count > max_files)
      {
        del_file(argv[optind], del_count);
        del_count += 1;
      }
    }

    // Write to file
    n = rval;
    wrt_pt = buf;
    do
    {
      if ((wval = write(fd, wrt_pt, n)) == -1)
        print_error_and_die("write error: file");

      wrt_pt += wval;
    } while (n -= wval);

    byte_count += rval;
  }

  close(fd);

  exit(0);
}

