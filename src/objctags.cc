/* vim: set ft=cpp fenc=utf-8 sw=2 ts=2 et: */
/*
 * Copyright (c) 2013 Chongyu Zhu <lembacon@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Defines.h"
#include "TagFormatter.h"
#include "Configuration.h"
#include "ClangTool.h"
#include "ClangFrontendAction.h"

static int flag_recursive = 0;

static struct option options[] = {
  { "file", required_argument, NULL, 'f' },
  { "recursive", no_argument, &flag_recursive, 1 },
  { "vim-conf", no_argument, NULL, 0 },
  { "version", no_argument, NULL, 'v' },
  { "help", no_argument, NULL, 'h' },
  { NULL, 0, NULL, 0 }
};

static void usage(void) {
  std::ostringstream os;
  os << "Usage: " << OBJCTAGS_PROGRAM_NAME << " [options] [file(s)]\n";
  os << "\n";
  os << "  -f, --file [FILE]  Output file. '-' for stdout.\n";
  os << "  -r, --recursive    Recursively search for source files\n";
  os << "      --vim-conf     Show vim conf for TagBar\n";
  os << "  -v, --version      Show version\n";
  os << "  -h, --help         Show help\n";
  fprintf(stderr, "%s\n", os.str().c_str());
}

static void version(void) {
  fprintf(stderr, "%s %s\n", OBJCTAGS_PROGRAM_NAME, OBJCTAGS_PROGRAM_VERSION);
}

static void vim_conf(void) {
  printf("%s\n", objctags::tagbarConfigurations().c_str());
}

struct ThreadInfo {
  pthread_t thread;
  pthread_mutex_t *tagFormatterMutex;
  objctags::TagFormatter *tagFormatter;
  std::vector<std::string> sourceFiles;
};

static void *threadMain(void *data) {
  ThreadInfo *threadInfo = (ThreadInfo *)data;
  for (size_t i = 0; i < threadInfo->sourceFiles.size(); i++) {
    objctags::Configuration config;
    //config.setSourceType(objctags::getSourceTypeForFileName(threadInfo->sourceFiles[i]));
    config.setSourceType("objective-c++");

    objctags::TagInfoVector tagInfoVector;
    objctags::runClangToolOnCodeWithArgs(new objctags::ClangFrontendAction(tagInfoVector),
                                         objctags::readFile(threadInfo->sourceFiles[i]),
                                         config.getClangArgs(),
                                         threadInfo->sourceFiles[i]);

    pthread_mutex_lock(threadInfo->tagFormatterMutex);
    threadInfo->tagFormatter->merge(tagInfoVector);
    pthread_mutex_unlock(threadInfo->tagFormatterMutex);
  }
  return NULL;
}

int main(int argc, char **argv) {
  int ch;
  int opt_index;
  std::string file = "tags";

  if (argc == 1) {
    usage();
    exit(EXIT_SUCCESS);
  }

  while ((ch = getopt_long(argc, argv, "f:rvh", options, &opt_index)) != -1) {
    switch (ch) {
      case 0:
        if (opt_index == 2) {
          vim_conf();
          exit(EXIT_SUCCESS);
        }
        break;

      case 'f':
        file = optarg;
        break;

      case 'r':
        flag_recursive = 1;
        break;

      case 'v':
        version();
        exit(EXIT_SUCCESS);

      case 'h':
        usage();
        exit(EXIT_SUCCESS);

      case '?':
        usage();
        exit(EXIT_FAILURE);

      default:
        abort();
    }
  }

  argc -= optind;
  argv += optind;

  std::vector<std::string> sourceFiles;

  if (flag_recursive) {
    std::string directory;
    if (argc > 0) {
      directory = argv[0];
    }
    else {
      directory = ".";
    }

    std::string expandedDir = objctags::expandPath(directory);
    DIR *dir = opendir(expandedDir.c_str());
    if (dir == NULL) {
      fprintf(stderr, "'%s' is not a valid directory\n", directory.c_str());
      exit(EXIT_FAILURE);
    }
    else {
      closedir(dir);
    }

    sourceFiles = objctags::recursivelySearchSourceFiles(expandedDir);
  }
  else {
    if (argc == 0) {
      fprintf(stderr, "missing input directory or files\n");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < argc; i++) {
      sourceFiles.push_back(objctags::expandPath(argv[i]));
    }
  }

  objctags::TagFormatter tagFormatter;
  pthread_mutex_t tagFormatterMutex;
  pthread_mutex_init(&tagFormatterMutex, NULL);

  size_t threadCount = sysconf(_SC_NPROCESSORS_ONLN);
  ThreadInfo *threads = new ThreadInfo[threadCount];

  size_t perThreadSFCount = sourceFiles.size() / threadCount;
  size_t remainingSFCount = sourceFiles.size() % threadCount;

  for (size_t i = 0; i < threadCount; i++) {
    threads[i].tagFormatterMutex = &tagFormatterMutex;
    threads[i].tagFormatter = &tagFormatter;
    std::vector<std::string>::iterator startIterator = sourceFiles.begin() + i * perThreadSFCount;
    threads[i].sourceFiles.insert(threads[i].sourceFiles.begin(), startIterator, startIterator + perThreadSFCount);
  }
  for (size_t i = 0; i < remainingSFCount; i++) {
    threads[i].sourceFiles.push_back(sourceFiles[sourceFiles.size() - remainingSFCount + i]);
  }

  for (size_t i = 0; i < threadCount; i++) {
    pthread_create(&threads[i].thread, NULL, threadMain, &threads[i]);
  }
  for (size_t i = 0; i < threadCount; i++) {
    pthread_join(threads[i].thread, NULL);
  }

  pthread_mutex_destroy(&tagFormatterMutex);
  delete[] threads;

  if (file == "-") {
    printf("%s\n", tagFormatter.str().c_str());
  }
  else {
    std::ofstream fs(objctags::expandPath(file).c_str());
    fs << tagFormatter.str() << "\n";
  }

  return 0;
}
