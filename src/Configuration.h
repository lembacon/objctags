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

#ifndef __objctags_Configuration_h__
#define __objctags_Configuration_h__

#include <string>
#include <vector>

namespace objctags {

class Configuration {
public:
  void setSourceType(const std::string &sourceType);
  void setSysroot(const std::string &sysroot);
  void addSearchPath(const std::string &path);
  void addDefine(const std::string &key, const std::string &value = "");

  std::vector<std::string> getClangArgs() const;

private:
  std::string _sourceType;
  std::string _sysroot;
  std::vector<std::string> _searchPaths;
  std::vector<std::string> _defines;
};

std::string getSourceTypeForFileName(const std::string &fileName);
std::vector<std::string> recursivelySearchSourceFiles(const std::string &directory);
std::string expandPath(const std::string &path);
std::string readFile(const std::string &fileName);

std::string tagbarConfigurations();

} // end namespace objctags

#endif /* __objctags_Configuration_h__ */
