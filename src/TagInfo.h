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

#ifndef __objctags_TagInfo_h__
#define __objctags_TagInfo_h__

#include <string>
#include <vector>

namespace objctags {

struct TagInfo {
  std::string name;
  std::string file;
  std::string line;
  char kind;
  std::string scope;

  bool operator==(const TagInfo &tagInfo)
  {
    return name == tagInfo.name && file == tagInfo.file &&
           line == tagInfo.line && kind == tagInfo.kind &&
           scope == tagInfo.scope;
  }

  bool operator!=(const TagInfo &tagInfo)
  {
    return !operator==(tagInfo);
  }
};
typedef std::vector<TagInfo> TagInfoVector;
typedef TagInfoVector::iterator TagInfoIterator;
typedef TagInfoVector::const_iterator TagInfoConstIterator;

std::string getTagKindScopedName(const char tagkind);
std::string getTagKindLongName(const char tagkind);

/*
 * The following C/C++ tag kinds are compatible with ctags.
 */
static const char tagkind_define = 'd';
static const char tagkind_typedef = 't';
static const char tagkind_prototype = 'p';
static const char tagkind_variable = 'v';
static const char tagkind_enum = 'g';
static const char tagkind_enum_member = 'e';
static const char tagkind_namespace = 'n';
static const char tagkind_class = 'c';
static const char tagkind_struct = 's';
static const char tagkind_union = 'u';
static const char tagkind_function = 'f';
static const char tagkind_member = 'm';

/*
 * These Objective-C/Objective-C++ specific tag kinds
 * are not compatible with ctags.
 */
static const char tagkind_interface = 'a';
static const char tagkind_implementation = 'A';
static const char tagkind_category = 'h';
static const char tagkind_category_impl = 'H';
static const char tagkind_protocol = 'j';
static const char tagkind_property = 'J';
static const char tagkind_method = 'k';

static const char tagkinds[] = {
  tagkind_define,
  tagkind_typedef,
  tagkind_prototype,
  tagkind_variable,
  tagkind_enum,
  tagkind_enum_member,
  tagkind_namespace,
  tagkind_class,
  tagkind_struct,
  tagkind_union,
  tagkind_function,
  tagkind_member,

  tagkind_interface,
  tagkind_implementation,
  tagkind_category,
  tagkind_category_impl,
  tagkind_protocol,
  tagkind_property,
  tagkind_method
};
static const size_t tagkinds_count = sizeof(tagkinds) / sizeof(tagkinds[0]);

static const char *const tagscope_splitter = "::";
static const char *const tagextra_filescope = "file:";

} // end namespace objctags

#endif /* __objctags_TagInfo_h__ */
