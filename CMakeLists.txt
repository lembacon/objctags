cmake_minimum_required(VERSION 2.8)
project(objctags)

set(LLVM_PREFIX /usr/local)
set(LLVM_LIBS
  LLVMAnalysis LLVMArchive LLVMAsmParser LLVMAsmPrinter LLVMBitReader
  LLVMBitWriter LLVMCodeGen LLVMDebugInfo LLVMExecutionEngine
  LLVMInstCombine LLVMInstrumentation LLVMInterpreter LLVMJIT
  LLVMLinker LLVMMC LLVMMCDisassembler LLVMMCJIT LLVMMCParser
  LLVMRuntimeDyld LLVMScalarOpts LLVMSelectionDAG LLVMTableGen
  LLVMTransformUtils LLVMVectorize LLVMX86AsmParser LLVMX86AsmPrinter
  LLVMX86CodeGen LLVMX86Desc LLVMX86Disassembler LLVMX86Info LLVMX86Utils
  LLVMipa LLVMipo LLVMCore LLVMSupport LLVMObject LLVMTarget
  clangBasic clangAST clangTooling clangDriver clangFrontend
  clangLex clangARCMigrate clangASTMatchers clangAnalysis
  clangCodeGen clangEdit clangFrontendTool clangParse
  clangRewriteCore clangRewriteFrontend clangSema clangSerialization
  clangStaticAnalyzerCheckers clangStaticAnalyzerCore
  clangStaticAnalyzerFrontend
  )

foreach(lib ${LLVM_LIBS})
  add_library(${lib} STATIC IMPORTED)
  set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION ${LLVM_PREFIX}/lib/lib${lib}.a)
endforeach(lib)
set(LIBS ${LIBS} ${LLVM_LIBS})
include_directories(${LLVM_PREFIX}/include)

set(OBJCTAGS_PROGRAM_VERSION 0.0.2)
configure_file(src/Defines.h.in ${CMAKE_CURRENT_BINARY_DIR}/Defines.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_BUILD_TYPE Release)
add_definitions(-D__STDC_LIMIT_MACROS)
add_definitions(-D__STDC_CONSTANT_MACROS)
set(CMAKE_CXX_FLAGS "-O3 -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-S -Wl,-x -Wl,-dead_strip")

aux_source_directory(src OBJCTAGS_SRC)
add_executable(objctags ${OBJCTAGS_SRC})
target_link_libraries(objctags ${LIBS})

install(TARGETS objctags DESTINATION bin)
