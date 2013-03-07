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

#include <map>
#include <utility>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <wordexp.h>
#include <dirent.h>
#include <sys/types.h>
#include "Configuration.h"

namespace objctags {

void Configuration::setSourceType(const std::string &sourceType)
{
  _sourceType = sourceType;
}

void Configuration::setSysroot(const std::string &sysroot)
{
  _sysroot = sysroot;
}

void Configuration::addSearchPath(const std::string &path)
{
  if (!path.empty()) {
    _searchPaths.push_back("-I" + path);
  }
}

void Configuration::addDefine(const std::string &key, const std::string &value)
{
  if (!key.empty()) {
    if (!value.empty()) {
      _defines.push_back("-D" + key + "=" + value);
    }
    else {
      _defines.push_back("-D" + key);
    }
  }
}

std::vector<std::string> Configuration::getClangArgs() const
{
  std::vector<std::string> args;
  if (!_sourceType.empty()) {
    args.push_back("-x");
    args.push_back(_sourceType);
  }
  if (!_sysroot.empty()) {
    args.push_back("-isysroot");
    args.push_back(_sysroot);
  }
  args.insert(args.end(), _searchPaths.begin(), _searchPaths.end());
  args.insert(args.end(), _defines.begin(), _defines.end());
  return args;
}

namespace {

std::map<std::string, std::string> sourceTypeMap;
typedef std::map<std::string, std::string>::iterator SourceTypeMapIterator;

void initSourceTypeMap()
{
  if (!sourceTypeMap.empty()) {
    return;
  }

  std::string c = "c";
  std::string cxx = "c++";
  std::string objc = "objective-c";
  std::string objcxx = "objective-c++";
  //std::string c_header = "c-header";
  std::string cxx_header = "c++-header";
  //std::string objc_header = "objective-c-header";
  std::string objcxx_header = "objective-c++-header";

  sourceTypeMap.insert(std::make_pair("c", c));
  sourceTypeMap.insert(std::make_pair("cpp", cxx));
  sourceTypeMap.insert(std::make_pair("cc", cxx));
  sourceTypeMap.insert(std::make_pair("m", objc));
  sourceTypeMap.insert(std::make_pair("mm", objcxx));
  sourceTypeMap.insert(std::make_pair("h", objcxx_header));
  sourceTypeMap.insert(std::make_pair("hpp", cxx_header));
  sourceTypeMap.insert(std::make_pair("inl", cxx_header));
}

} // end namespace

std::string getSourceTypeForFileName(const std::string &fileName)
{
  if (fileName.empty()) {
    return "";
  }

  size_t index = fileName.rfind('.');
  if (index == std::string::npos) {
    return "";
  }

  std::string extension = fileName.substr(index + 1);
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  initSourceTypeMap();
  SourceTypeMapIterator it = sourceTypeMap.find(extension);
  if (it != sourceTypeMap.end()) {
    return it->second;
  }

  return "";
}

namespace {

void recursivelySearchSourceFiles(std::vector<std::string> &sourceFiles, const std::string &directory)
{
  if (directory.empty()) {
    return;
  }

  DIR *dir = opendir(directory.c_str());
  if (dir == NULL) {
    return;
  }

  while (true) {
    struct dirent *ent = readdir(dir);
    if (ent == NULL) {
      break;
    }

    if (ent->d_type & DT_REG) {
      std::string fullname = directory + "/" + ent->d_name;
      if (!getSourceTypeForFileName(fullname).empty()) {
        sourceFiles.push_back(fullname);
      }
    }
    else if (ent->d_type & DT_DIR) {
      if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
        std::string childDir = directory + "/" + ent->d_name;
        recursivelySearchSourceFiles(sourceFiles, childDir);
      }
    }
  }

  closedir(dir);
}

} // end namespace

std::vector<std::string> recursivelySearchSourceFiles(const std::string &directory)
{
  std::vector<std::string> sourceFiles;
  recursivelySearchSourceFiles(sourceFiles, directory);
  return sourceFiles;
}

std::string expandPath(const std::string &path)
{
  wordexp_t we;
  std::string result;
  if (wordexp(path.c_str(), &we, 0) == 0) {
    if (we.we_wordc > 0) {
      char *rp = realpath(we.we_wordv[0], NULL);
      if (rp != NULL) {
        result = std::string(rp);
        free(rp);
      }
      else {
        result = std::string(we.we_wordv[0]);
      }
    }
    wordfree(&we);
  }
  return result;
}

std::string readFile(const std::string &fileName)
{
  std::fstream fs(fileName.c_str());
  return std::string((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
}

} // end namespace objctags
