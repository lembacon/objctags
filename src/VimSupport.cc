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

#include <sstream>
#include <vector>
#include <utility>
#include "Configuration.h"
#include "TagInfo.h"
#include "Defines.h"

namespace objctags {

std::string tagbarConfigurations()
{
  std::vector< std::pair<char, std::string> > kindScopePairs;
  for (size_t i = 0; i < tagkinds_count; i++) {
    std::string scopedName = getTagKindScopedName(tagkinds[i]);
    if (!scopedName.empty()) {
      kindScopePairs.push_back(std::make_pair(tagkinds[i], scopedName));
    }
  }

  std::ostringstream os;
  os << "if executable('" << OBJCTAGS_PROGRAM_NAME << "')\n";
  os << "  let objctags_definitions = {\n";
  os << "    \\ 'ctagsbin': '" << OBJCTAGS_PROGRAM_NAME << "',\n";
  os << "    \\ 'ctagsargs': '-f -',\n";
  os << "    \\ 'kinds': [\n";
  for (size_t i = 0; i < tagkinds_count; i++) {
    os << "      \\ '" << tagkinds[i] << ":" << getTagKindLongName(tagkinds[i]) << ((i != tagkinds_count - 1) ? "',\n" : "'\n");
  }
  os << "    \\ ],\n";
  os << "    \\ 'sro': '" << tagscope_splitter << "',\n";
  os << "    \\ 'kind2scope': {\n";
  for (size_t i = 0; i < kindScopePairs.size(); i++) {
    os << "      \\ '" << kindScopePairs[i].first << "': '" << kindScopePairs[i].second << ((i != kindScopePairs.size() - 1) ? "',\n" : "'\n");
  }
  os << "    \\ },\n";
  os << "    \\ 'scope2kind': {\n";
  for (size_t i = 0; i < kindScopePairs.size(); i++) {
    os << "      \\ '" << kindScopePairs[i].second << "': '" << kindScopePairs[i].first << ((i != kindScopePairs.size() - 1) ? "',\n" : "'\n");
  }
  os << "    \\ },\n";
  os << "  \\ }\n";
  os << "  let g:tagbar_type_c = deepcopy(objctags_definitions)\n";
  os << "  let g:tagbar_type_cpp = deepcopy(objctags_definitions)\n";
  os << "  let g:tagbar_type_objc = deepcopy(objctags_definitions)\n";
  os << "  let g:tagbar_type_objcpp = deepcopy(objctags_definitions)\n";
  os << "  unlet objctags_definitions\n";
  os << "endif\n";
  return os.str();
}

} // end namespace objctags
