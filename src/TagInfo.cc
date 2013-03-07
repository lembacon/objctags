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

#include "TagInfo.h"

namespace objctags {

std::string getTagKindScopedName(const char tagkind)
{
  switch (tagkind) {
  case tagkind_enum:
    return "enum";
  case tagkind_namespace:
    return "namespace";
  case tagkind_class:
    return "class";
  case tagkind_struct:
    return "struct";
  case tagkind_union:
    return "union";

  case tagkind_interface:
    return "interface";
  case tagkind_implementation:
    return "implementation";
  case tagkind_category:
    return "category";
  case tagkind_category_impl:
    return "categoryimpl";
  case tagkind_protocol:
    return "protocol";

  default:
    return "";
  }
}

std::string getTagKindLongName(const char tagkind)
{
  switch (tagkind) {
  case tagkind_define:
    return "macros";
  case tagkind_typedef:
    return "typedefs";
  case tagkind_prototype:
    return "prototypes";
  case tagkind_variable:
    return "variables";
  case tagkind_enum:
    return "enums";
  case tagkind_enum_member:
    return "enumerators";
  case tagkind_namespace:
    return "namespaces";
  case tagkind_class:
    return "classes";
  case tagkind_struct:
    return "structs";
  case tagkind_union:
    return "unions";
  case tagkind_function:
    return "functions";
  case tagkind_member:
    return "members";

  case tagkind_interface:
    return "interfaces";
  case tagkind_implementation:
    return "implementations";
  case tagkind_category:
    return "categories";
  case tagkind_category_impl:
    return "categoryimpls";
  case tagkind_protocol:
    return "protocols";
  case tagkind_property:
    return "properties";
  case tagkind_method:
    return "methods";

  default:
    return "";
  }
}

} // end namespace objctags
