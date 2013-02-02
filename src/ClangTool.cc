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

#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/FileSystemOptions.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/Tool.h>
#include "ClangTool.h"

namespace objctags {
  // Most of codes here are stolen from 'clang/lib/Tooling/Tooling.cpp',
  // as we shall get rid of those annoying diagnostic messages.
  bool runClangToolOnCodeWithArgs(clang::FrontendAction *action,
                                  const llvm::Twine &code,
                                  const std::vector<std::string> &args,
                                  const llvm::Twine &fileName)
  {
    llvm::SmallString<16> fileNameStorage;
    llvm::StringRef fileNameRef = fileName.toNullTerminatedStringRef(fileNameStorage);
    llvm::SmallString<1024> pathStorage;
    llvm::sys::path::native(fileNameRef, pathStorage);
    llvm::SmallString<1024> codeStorage;
    llvm::StringRef codeRef = code.toNullTerminatedStringRef(codeStorage);
    llvm::OwningPtr<clang::FrontendAction> scopedToolAction(action);
    clang::FileManager fileManager((clang::FileSystemOptions()));

    std::vector<const char *> argv;
    argv.push_back("clang-tool");
    argv.push_back("-fsyntax-only");
    for (size_t i = 0; i < args.size(); i++) {
      argv.push_back(args[i].c_str());
    }
    argv.push_back(fileNameRef.data());

    llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> diagnosticOpts(new clang::DiagnosticOptions());
    llvm::OwningPtr<clang::DiagnosticsEngine> diagnostics(new clang::DiagnosticsEngine(llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(new clang::DiagnosticIDs()), &*diagnosticOpts, new clang::IgnoringDiagConsumer(), true));

    const llvm::OwningPtr<clang::driver::Driver> driver(new clang::driver::Driver(argv[0], llvm::sys::getDefaultTargetTriple(), "a.out", false, *diagnostics.get()));
    driver->setTitle("clang_based_tool");
    driver->setCheckInputsExist(false);

    const llvm::OwningPtr<clang::driver::Compilation> compilation(driver->BuildCompilation(llvm::makeArrayRef(argv)));
    const clang::driver::JobList &jobs = compilation.get()->getJobs();
    if (jobs.size() != 1 || !llvm::isa<clang::driver::Command>(*jobs.begin())) {
      return false;
    }
    const clang::driver::Command *cmd = llvm::cast<clang::driver::Command>(*jobs.begin());
    if (llvm::StringRef(cmd->getCreator().getName()) != "clang") {
      return false;
    }
    const clang::driver::ArgStringList *const cc1Args = &cmd->getArguments();

    //compilation->PrintJob(llvm::errs(), compilation->getJobs(), "\n", true);

    llvm::OwningPtr<clang::CompilerInvocation> invocation(new clang::CompilerInvocation());
    clang::CompilerInvocation::CreateFromArgs(*invocation, cc1Args->data() + 1, cc1Args->data() + cc1Args->size(), *diagnostics.get());
    invocation->getFrontendOpts().DisableFree = false;
    invocation->getFrontendOpts().SkipFunctionBodies = true;
    invocation->getDiagnosticOpts().ShowCarets = false;

    clang::CompilerInstance compiler;
    compiler.setInvocation(invocation.take());
    compiler.setDiagnostics(diagnostics.take());
    compiler.setFileManager(&fileManager);

    compiler.createSourceManager(fileManager);
    const llvm::MemoryBuffer *input = llvm::MemoryBuffer::getMemBuffer(codeRef);
    const clang::FileEntry *file = fileManager.getVirtualFile(pathStorage, input->getBufferSize(), 0);
    compiler.getSourceManager().overrideFileContents(file, input);

    const bool success = compiler.ExecuteAction(*scopedToolAction);
    compiler.resetAndLeakFileManager();
    fileManager.clearStatCaches();

    return success;
  }
}
