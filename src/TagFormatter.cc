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

#include "TagFormatter.h"
#include "Defines.h"

namespace objctags {
  std::string TagFormatter::header()
  {
    std::ostringstream os;
    os << "!_TAG_FILE_FORMAT\t2\t/extended format/\n";
    os << "!_TAG_FILE_SORTED\t0\t/0=unsorted, 1=sorted, 2=foldcase/\n";
    os << "!_TAG_PROGRAM_AUTHOR\t" << OBJCTAGS_PROGRAM_AUTHOR << "\n";
    os << "!_TAG_PROGRAM_NAME\t" << OBJCTAGS_PROGRAM_NAME << "\n";
    os << "!_TAG_PROGRAM_URL\t" << OBJCTAGS_PROGRAM_URL << "\n";
    os << "!_TAG_PROGRAM_VERSION\t" << OBJCTAGS_PROGRAM_VERSION << "\n";
    return os.str();
  }

  void TagFormatter::merge(const TagInfoVector &tagInfoVector)
  {
    for (TagInfoConstIterator it = tagInfoVector.begin(); it != tagInfoVector.end(); it++) {
      _stream << it->name << "\t"
              << it->file << "\t"
              << "/^" << it->line << "$/;\"\t"
              << it->kind << "\t"
              << it->scope << "\n";
    }
  }
}
