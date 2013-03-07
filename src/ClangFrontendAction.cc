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

#include <algorithm>
#include <sstream>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Lex/Preprocessor.h>
#include "ClangFrontendAction.h"

namespace objctags {

namespace {

class RecursiveASTVisitor : public clang::RecursiveASTVisitor<RecursiveASTVisitor> {
public:
  RecursiveASTVisitor(TagInfoVector *tagInfoVector, clang::ASTContext *context);

  bool VisitTypedefDecl(clang::TypedefDecl *decl);
  //bool VisitTypeAliasDecl(clang::TypeAliasDecl *decl);
  bool VisitEnumDecl(clang::EnumDecl *decl);
  bool VisitRecordDecl(clang::RecordDecl *decl);
  bool VisitEnumConstantDecl(clang::EnumConstantDecl *decl);
  bool VisitFunctionDecl(clang::FunctionDecl *decl);
  bool VisitFieldDecl(clang::FieldDecl *decl);
  bool VisitVarDecl(clang::VarDecl *decl);
  //bool VisitLabelDecl(clang::LabelDecl *decl);
  //bool VisitParmVarDecl(clang::ParmVarDecl *decl);
  //bool VisitFileScopeAsmDecl(clang::FileScopeAsmDecl *decl);
  //bool VisitImportDecl(clang::ImportDecl *decl);
  //bool VisitStaticAssertDecl(clang::StaticAssertDecl *decl);
  bool VisitNamespaceDecl(clang::NamespaceDecl *decl);
  //bool VisitUsingDirectiveDecl(clang::UsingDirectiveDecl *decl);
  bool VisitNamespaceAliasDecl(clang::NamespaceAliasDecl *decl);
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl);
  //bool VisitLinkageSpecDecl(clang::LinkageSpecDecl *decl);
  //bool VisitTemplateDecl(const clang::TemplateDecl *decl);
  //bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl *decl);
  //bool VisitClassTemplateDecl(clang::ClassTemplateDecl *decl);
  bool VisitObjCMethodDecl(clang::ObjCMethodDecl *decl);
  bool VisitObjCImplementationDecl(clang::ObjCImplementationDecl *decl);
  bool VisitObjCInterfaceDecl(clang::ObjCInterfaceDecl *decl);
  bool VisitObjCProtocolDecl(clang::ObjCProtocolDecl *decl);
  bool VisitObjCCategoryImplDecl(clang::ObjCCategoryImplDecl *decl);
  bool VisitObjCCategoryDecl(clang::ObjCCategoryDecl *decl);
  //bool VisitObjCCompatibleAliasDecl(clang::ObjCCompatibleAliasDecl *decl);
  bool VisitObjCPropertyDecl(clang::ObjCPropertyDecl *decl);
  //bool VisitObjCPropertyImplDecl(clang::ObjCPropertyImplDecl *decl);
  //bool VisitUnresolvedUsingTypenameDecl(clang::UnresolvedUsingTypenameDecl *decl);
  //bool VisitUnresolvedUsingValueDecl(clang::UnresolvedUsingValueDecl *decl);
  //bool VisitUsingDecl(clang::UsingDecl *decl);
  //bool VisitUsingShadowDecl(clang::UsingShadowDecl *decl);

private:
  clang::ASTContext *_context;
  TagInfoVector *_tagInfoVector;

  bool _isMain(clang::Decl *decl);
  std::string _getScope(clang::DeclContext *declContext);
  std::string _getPrettyFunctionName(clang::FunctionDecl *decl);
  std::string _getPrettyCategoryName(clang::ObjCContainerDecl *decl);
  std::string _getRealName(clang::NamedDecl *decl);
  void _addTag(clang::NamedDecl *decl, char kind, const std::string &scope);
};

class ASTConsumer : public clang::ASTConsumer {
public:
  ASTConsumer(TagInfoVector *tagInfoVector, clang::ASTContext *context);
  virtual void HandleTranslationUnit(clang::ASTContext &context);

private:
  RecursiveASTVisitor _visitor;
};

} // end namespace

bool RecursiveASTVisitor::_isMain(clang::Decl *decl)
{
  clang::FullSourceLoc fullLoc = _context->getFullLoc(decl->getLocStart());
  if (fullLoc.isInvalid() || fullLoc.getFileID() != _context->getSourceManager().getMainFileID()) {
    return false;
  }
  return true;
}

std::string RecursiveASTVisitor::_getScope(clang::DeclContext *declContext)
{
  if (declContext != NULL && declContext->getDeclKind() == clang::Decl::LinkageSpec) {
    declContext = declContext->getParent();
  }
  if (declContext == NULL || declContext->getDeclKind() == clang::Decl::TranslationUnit) {
    return tagextra_filescope;
  }

  std::string scopedTypeName;
  switch (declContext->getDeclKind()) {
  case clang::Decl::Namespace:
    scopedTypeName = getTagKindScopedName(tagkind_namespace);
    break;
  case clang::Decl::Enum:
    scopedTypeName = getTagKindScopedName(tagkind_enum);
    break;
  case clang::Decl::ObjCInterface:
    scopedTypeName = getTagKindScopedName(tagkind_interface);
    break;
  case clang::Decl::ObjCImplementation:
    scopedTypeName = getTagKindScopedName(tagkind_implementation);
    break;
  case clang::Decl::ObjCCategory:
    scopedTypeName = getTagKindScopedName(tagkind_category);
    break;
  case clang::Decl::ObjCCategoryImpl:
    scopedTypeName = getTagKindScopedName(tagkind_category_impl);
    break;
  case clang::Decl::ObjCProtocol:
    scopedTypeName = getTagKindScopedName(tagkind_protocol);
    break;
  case clang::Decl::Record:
  case clang::Decl::CXXRecord:
    switch (llvm::dyn_cast<clang::TagDecl>(declContext)->getTagKind()) {
    case clang::TTK_Class:
      scopedTypeName = getTagKindScopedName(tagkind_class);
      break;
    case clang::TTK_Struct:
      scopedTypeName = getTagKindScopedName(tagkind_struct);
      break;
    case clang::TTK_Union:
      scopedTypeName = getTagKindScopedName(tagkind_union);
      break;
    default:
      return "";
    }
    break;
  default:
    return "";
  }

  std::ostringstream os;
  os << scopedTypeName << ":";

  std::vector<std::string> names;
  do {
    clang::NamedDecl *namedDecl = llvm::dyn_cast<clang::NamedDecl>(declContext);

    std::string name;
    if (namedDecl->getKind() == clang::Decl::ObjCCategory ||
        namedDecl->getKind() == clang::Decl::ObjCCategoryImpl) {
      name = _getPrettyCategoryName(llvm::dyn_cast<clang::ObjCContainerDecl>(namedDecl));
    }
    else {
      name = namedDecl->getNameAsString();
    }

    if (name.length() == 0) {
      name = _getRealName(namedDecl);
      if (name.length() == 0) {
        name = "<Anonymous>";
      }
    }

    names.push_back(name);
  } while ((declContext = declContext->getParent()) != NULL &&
           llvm::isa<clang::NamedDecl>(declContext));

  for (ssize_t i = names.size() - 1; i >= 0; i--) {
    os << names[i];
    if (i > 0) {
      os << tagscope_splitter;
    }
  }

  return os.str();
}

std::string RecursiveASTVisitor::_getPrettyFunctionName(clang::FunctionDecl *decl)
{
  return decl->getNameAsString();
}

std::string RecursiveASTVisitor::_getPrettyCategoryName(clang::ObjCContainerDecl *decl)
{
  clang::ObjCInterfaceDecl *interfaceDecl = NULL;
  if (decl->getKind() == clang::Decl::ObjCCategory) {
    interfaceDecl = llvm::dyn_cast<clang::ObjCCategoryDecl>(decl)->getClassInterface();
  }
  else if (decl->getKind() == clang::Decl::ObjCCategoryImpl) {
    interfaceDecl = llvm::dyn_cast<clang::ObjCCategoryImplDecl>(decl)->getClassInterface();
  }
  else {
    return decl->getNameAsString();
  }

  if (interfaceDecl != NULL) {
    return interfaceDecl->getNameAsString() + "(" + decl->getNameAsString() + ")";
  }

  return "";
}

std::string RecursiveASTVisitor::_getRealName(clang::NamedDecl *decl)
{
  std::string realName = decl->getNameAsString();
  if (realName.length() > 0) {
    return realName;
  }

  if (decl->getKind() == clang::Decl::Namespace) {
    return "<Anonymous Namespace>";
  }
  else if (llvm::isa<clang::TypeDecl>(decl)) {
    llvm::raw_string_ostream os(realName);
    clang::LangOptions langOptions;
    clang::PrintingPolicy policy(langOptions);
    _context->getTypeDeclType(llvm::dyn_cast<clang::TypeDecl>(decl)).print(os, policy);
    return os.str();
  }

  return "";
}

void RecursiveASTVisitor::_addTag(clang::NamedDecl *decl, char kind, const std::string &scope)
{
  std::string name;
  if (decl->getKind() == clang::Decl::ObjCMethod) {
    if (llvm::dyn_cast<clang::ObjCMethodDecl>(decl)->isClassMethod()) {
      name = "+" + decl->getNameAsString();
    }
    else {
      name = "-" + decl->getNameAsString();
    }
  }
  else if (decl->getKind() == clang::Decl::ObjCCategory ||
           decl->getKind() == clang::Decl::ObjCCategoryImpl) {
    name = _getPrettyCategoryName(llvm::dyn_cast<clang::ObjCContainerDecl>(decl));
    if (name.length() == 0) {
      return;
    }
  }
  else if (decl->getKind() == clang::Decl::Function) {
    clang::FunctionDecl *function = llvm::dyn_cast<clang::FunctionDecl>(decl);
    //if (!function->hasBody()) {
    //  kind = tagkind_prototype;
    //}
    name = _getPrettyFunctionName(function);
  }
  else if (decl->getKind() == clang::Decl::NamespaceAlias) {
    name = decl->getNameAsString() + llvm::dyn_cast<clang::NamespaceAliasDecl>(decl)->getAliasedNamespace()->getNameAsString();
  }
  else {
    name = decl->getNameAsString();
    if (name.length() == 0) {
      name = _getRealName(decl);
      if (name.length() == 0) {
        return;
      }
    }
  }

  clang::FullSourceLoc fullLoc = _context->getFullLoc(decl->getLocStart());
  clang::SourceLocation spellingLoc = _context->getSourceManager().getSpellingLoc(fullLoc);
  llvm::StringRef fileNameRef = _context->getSourceManager().getFilename(spellingLoc);

  unsigned spellingColumnNumber = fullLoc.getSpellingColumnNumber();
  const char *beginOfLine = fullLoc.getCharacterData() - spellingColumnNumber + 1;
  const char *endOfLine = beginOfLine;
  while (*endOfLine != '\r' && *endOfLine != '\n' && *endOfLine != '\0') {
    endOfLine++;
  }
  std::string line(beginOfLine, static_cast<size_t>(endOfLine - beginOfLine));

  TagInfo tagInfo;
  tagInfo.name = name;
  tagInfo.file = fileNameRef.str();
  tagInfo.line = line;
  tagInfo.kind = kind;
  tagInfo.scope = scope;

  _tagInfoVector->push_back(tagInfo);
}

RecursiveASTVisitor::RecursiveASTVisitor(TagInfoVector *tagInfoVector, clang::ASTContext *context) :
  _context(context),
  _tagInfoVector(tagInfoVector)
{
}

bool RecursiveASTVisitor::VisitTypedefDecl(clang::TypedefDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_typedef, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitEnumDecl(clang::EnumDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_enum, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitRecordDecl(clang::RecordDecl *decl)
{
  if (_isMain(decl)) {
    if (decl->isCompleteDefinition()) {
      switch (decl->getTagKind()) {
      case clang::TTK_Class:
        _addTag(decl, tagkind_class, _getScope(decl->getDeclContext()));
        break;
      case clang::TTK_Struct:
        _addTag(decl, tagkind_struct, _getScope(decl->getDeclContext()));
        break;
      case clang::TTK_Union:
        _addTag(decl, tagkind_union, _getScope(decl->getDeclContext()));
        break;
      default:
        break;
      }
    }
  }
  return true;
}

bool RecursiveASTVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_enum_member, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_function, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitFieldDecl(clang::FieldDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_member, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitVarDecl(clang::VarDecl *decl)
{
  if (_isMain(decl)) {
    switch (decl->getDeclContext()->getDeclKind()) {
    case clang::Decl::Function:
    case clang::Decl::ObjCMethod:
    case clang::Decl::CXXMethod:
    case clang::Decl::CXXConstructor:
      break;

    default:
      _addTag(decl, tagkind_variable, _getScope(decl->getDeclContext()));
      break;
    }
  }
  return true;
}

bool RecursiveASTVisitor::VisitNamespaceDecl(clang::NamespaceDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_namespace, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitNamespaceAliasDecl(clang::NamespaceAliasDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_namespace, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *decl)
{
  return VisitRecordDecl(decl);
}

bool RecursiveASTVisitor::VisitObjCMethodDecl(clang::ObjCMethodDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_method, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitObjCImplementationDecl(clang::ObjCImplementationDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_implementation, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitObjCInterfaceDecl(clang::ObjCInterfaceDecl *decl)
{
  if (_isMain(decl)) {
    if (decl->isThisDeclarationADefinition()) {
      _addTag(decl, tagkind_interface, _getScope(decl->getDeclContext()));
    }
  }
  return true;
}

bool RecursiveASTVisitor::VisitObjCProtocolDecl(clang::ObjCProtocolDecl *decl)
{
  if (_isMain(decl)) {
    if (decl->isThisDeclarationADefinition()) {
      _addTag(decl, tagkind_protocol, _getScope(decl->getDeclContext()));
    }
  }
  return true;
}

bool RecursiveASTVisitor::VisitObjCCategoryImplDecl(clang::ObjCCategoryImplDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_category_impl, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitObjCCategoryDecl(clang::ObjCCategoryDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_category, _getScope(decl->getDeclContext()));
  }
  return true;
}

bool RecursiveASTVisitor::VisitObjCPropertyDecl(clang::ObjCPropertyDecl *decl)
{
  if (_isMain(decl)) {
    _addTag(decl, tagkind_property, _getScope(decl->getDeclContext()));
  }
  return true;
}

ASTConsumer::ASTConsumer(TagInfoVector *tagInfoVector, clang::ASTContext *context) :
  _visitor(tagInfoVector, context)
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext &context)
{
  _visitor.TraverseDecl(context.getTranslationUnitDecl());
}

void ClangFrontendAction::EndSourceFileAction()
{
  clang::ASTFrontendAction::EndSourceFileAction();

  clang::CompilerInstance &compiler = getCompilerInstance();
  clang::ASTContext &context = compiler.getASTContext();
  clang::SourceManager &sourceManager = compiler.getSourceManager();
  clang::Preprocessor &pp = compiler.getPreprocessor();
  clang::Preprocessor::macro_iterator it;
  std::vector<TagInfo> macroTags;

  for (it = pp.macro_begin(); it != pp.macro_end(); it++) {
    if (it->second->isBuiltinMacro() || it->second->isFromAST()) {
      continue;
    }

    clang::FullSourceLoc fullLoc = context.getFullLoc(it->second->getDefinitionLoc());
    if (fullLoc.isInvalid() || fullLoc.getFileID() != sourceManager.getMainFileID()) {
      continue;
    }

    clang::SourceLocation spellingLoc = sourceManager.getSpellingLoc(fullLoc);
    llvm::StringRef fileNameRef = sourceManager.getFilename(spellingLoc);

    unsigned spellingColumnNumber = fullLoc.getSpellingColumnNumber();
    const char *beginOfLine = fullLoc.getCharacterData() - spellingColumnNumber + 1;
    const char *endOfLine = beginOfLine;
    while (*endOfLine != '\r' && *endOfLine != '\n' && *endOfLine != '\0') {
      endOfLine++;
    }
    std::string line(beginOfLine, static_cast<size_t>(endOfLine - beginOfLine));

    TagInfo tagInfo;
    tagInfo.name = it->first->getName().str();
    tagInfo.file = fileNameRef.str();
    tagInfo.line = line;
    tagInfo.kind = tagkind_define;
    tagInfo.scope = tagextra_filescope;

    macroTags.push_back(tagInfo);
  }

  _tagInfoVector->insert(_tagInfoVector->begin(), macroTags.begin(), macroTags.end());
  _tagInfoVector->resize(std::distance(_tagInfoVector->begin(), std::unique(_tagInfoVector->begin(), _tagInfoVector->end())));
}

ClangFrontendAction::ClangFrontendAction(TagInfoVector &tagInfoVector) :
  _tagInfoVector(&tagInfoVector)
{
}

clang::ASTConsumer *ClangFrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                      llvm::StringRef file)
{
  return new ASTConsumer(_tagInfoVector, &compiler.getASTContext());
}

} // end namespace objctags
