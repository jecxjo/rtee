/*
 * =====================================================================================
 *
 *       Filename:  rtee.c
 *
 *    Description:  Rotating Tee
 *
 *        Version:  0.2.1
 *        Created:  06/16/2015 07:44:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jeff Parent (jeff@commentedcode.org),
 *
 * =====================================================================================
 *
 * Copyright (c) 2015, Jeff Parent
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 1988, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. All advertising materials mentioning features or use of this software
 *     must display the following acknowledgement:
 * This product includes software developed by the University of
 * California, Berkeley and its contributors.
 *  4. Neither the name of the University nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "config.h"

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
                        "    -e        end on newline if possible\n"
                        "    -b num    max number of bytes per file (default 1M)\n"
                        "    -f num    max number of previous files before roll over (default 4)\n"
                        "    -B num    buffer size of read/write operation (default 8192)\n"
                        "    -h        display help\n"
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

  sprintf(newName, "%s.%lu", name, count);

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
    /* Child deletes file as it may take a while */
    if ((newName = malloc((size_t)(strlen(name) + 11))) == NULL)
      print_error_and_die("Unable to malloc");

    sprintf(newName, "%s.%lu", name, count);

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
  int i, fd, ch, exitval, n, read_length, write_length;
  char append = 0;
  char eol = 0;
  char *end;
  char *write_pt;
  char *buf;
  char *buf_pt;
  size_t read_buffer_size = BSIZE;
  char *found;

  int files_count = 0;
  char **files_name;
  int *files_fd;

  /* Arguments */
  while ((ch = getopt(argc, argv, "haeb:f:B:")) != -1)
    switch ((char)ch)
    {
      case 'h':
        print_help_and_die();
        break;

      case 'a':
        append = 1;
        break;

      case 'e':
        eol = 1;
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

      case 'B':
        read_buffer_size = (size_t)strtoul(optarg, &end, 10);
        if (errno != 0)
          print_usage_and_die();
        break;

      default:
        print_usage_and_die();
        break;
    }

  /* No files supplied for output */
  if (optind >= argc)
    print_usage_and_die();

  /* Number of [FILE]... arguments */
  files_count = argc - optind;
  if ((files_name = malloc((size_t)(files_count * sizeof(char*)))) == NULL)
      print_error_and_die("Unable to malloc");

  if ((files_fd = malloc((size_t)(files_count * sizeof(int)))) == NULL)
      print_error_and_die("Unable to malloc");

  /* Open all [FILE]... */
  for (i = 0; i < files_count; i++)
  {
    files_name[i] = argv[optind + i];
    files_fd[i] = open_file(files_name[i], file_count, append);
  }

  /* If max bytes per file is less the the read buffer shrink buffer */
  if (max_bytes < read_buffer_size)
    read_buffer_size = (size_t)max_bytes;

  if ((buf = (char *)malloc((size_t)read_buffer_size))  == NULL)
    print_error_and_die("Unable to malloc");

  /* read from STDIN until EOF */
  while ((read_length = read(STDIN_FILENO, buf, read_buffer_size)) > 0)
  {
    /* Write to stdout */
    n = read_length;
    write_pt = buf;
    do
    {
      if ((write_length = write(STDOUT_FILENO, write_pt, n)) == -1)
        print_error_and_die("write error: stdout");

      write_pt += write_length;
    } while (n -= write_length);

    found = NULL;
    buf_pt = buf;

    /* check if new file is needed */
    if (byte_count + read_length > max_bytes)
    {
      /* If searching for eol */
      if (eol == 1)
      {
        /* search for newline */
        found = strchr(buf, '\n');

        if (found)
        {
          /* Write to files */
          for (i = 0; i < files_count; i++)
          {
            n = (found - buf + 1);
            write_pt = buf;
            do
            {
              if ((write_length = write(files_fd[i], write_pt, n)) == -1)
                print_error_and_die("write error: file");

              write_pt += write_length;
            } while (n -= write_length);
          }

          buf_pt = found + 1;
        }
      }

      file_count += 1;

      for (i = 0; i < files_count; i++)
      {
        close(files_fd[i]);
        files_fd[i] = open_file(files_name[i], file_count, append);
      }
      byte_count = 0;

      /* Check if file needs to be deleted */
      if (file_count > max_files)
      {
        for (i = 0; i < files_count; i++)
          del_file(files_name[i], del_count);

        del_count += 1;
      }
    }


    /* Write to files */
    for (i = 0; i < files_count; i++)
    {
      n = read_length - (buf_pt - buf);
      write_pt = buf_pt;
      do
      {
        if ((write_length = write(files_fd[i], write_pt, n)) == -1)
          print_error_and_die("write error: file");

        write_pt += write_length;
      } while (n -= write_length);
    }

    byte_count += read_length;
  }

  /* close all open files */
  for (i = 0; i < files_count; i++)
    close(files_fd[i]);

  free(files_fd);
  free(files_name);
  free(buf);

  exit(0);
}

