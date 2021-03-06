//===- TableGen.cpp - Top-Level TableGen implementation for Clang ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the main function for Clang's TableGen.
//
//===----------------------------------------------------------------------===//

#include "TableGenBackends.h" // Declares all backends.
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Main.h"
#include "llvm/TableGen/Record.h"

using namespace llvm;
using namespace clang;

enum ActionType {
  GenClangAttrClasses,
  GenClangAttrExprArgsList,
  GenClangAttrImpl,
  GenClangAttrList,
  GenClangAttrPCHRead,
  GenClangAttrPCHWrite,
  GenClangAttrSpellingList,
  GenClangAttrSpellingListIndex,
  GenClangAttrLateParsedList,
  GenClangAttrTemplateInstantiate,
  GenClangAttrParsedAttrList,
  GenClangAttrParsedAttrKinds,
  GenClangAttrDump,
  GenClangDiagsDefs,
  GenClangDiagGroups,
  GenClangDiagsIndexName,
  GenClangCommentNodes,
  GenClangDeclNodes,
  GenClangStmtNodes,
  GenClangSACheckers,
  GenClangCommentHTMLTags,
  GenClangCommentHTMLTagsProperties,
  GenClangCommentHTMLNamedCharacterReferences,
  GenClangCommentCommandInfo,
  GenClangCommentCommandList,
  GenArmNeon,
  GenArmNeonSema,
  GenArmNeonTest
};

namespace {
  cl::opt<ActionType>
  Action(cl::desc("Action to perform:"),
         cl::values(clEnumValN(GenClangAttrClasses, "gen-clang-attr-classes",
                               "Generate clang attribute clases"),
                    clEnumValN(GenClangAttrExprArgsList,
                               "gen-clang-attr-expr-args-list",
                               "Generate a clang attribute expression "
                               "arguments list"),
                    clEnumValN(GenClangAttrImpl, "gen-clang-attr-impl",
                               "Generate clang attribute implementations"),
                    clEnumValN(GenClangAttrList, "gen-clang-attr-list",
                               "Generate a clang attribute list"),
                    clEnumValN(GenClangAttrPCHRead, "gen-clang-attr-pch-read",
                               "Generate clang PCH attribute reader"),
                    clEnumValN(GenClangAttrPCHWrite, "gen-clang-attr-pch-write",
                               "Generate clang PCH attribute writer"),
                    clEnumValN(GenClangAttrSpellingList,
                               "gen-clang-attr-spelling-list",
                               "Generate a clang attribute spelling list"),
                    clEnumValN(GenClangAttrSpellingListIndex,
                               "gen-clang-attr-spelling-index",
                               "Generate a clang attribute spelling index"),
                    clEnumValN(GenClangAttrLateParsedList,
                               "gen-clang-attr-late-parsed-list",
                               "Generate a clang attribute LateParsed list"),
                    clEnumValN(GenClangAttrTemplateInstantiate,
                               "gen-clang-attr-template-instantiate",
                               "Generate a clang template instantiate code"),
                    clEnumValN(GenClangAttrParsedAttrList,
                               "gen-clang-attr-parsed-attr-list",
                               "Generate a clang parsed attribute list"),
                    clEnumValN(GenClangAttrParsedAttrKinds,
                               "gen-clang-attr-parsed-attr-kinds",
                               "Generate a clang parsed attribute kinds"),
                    clEnumValN(GenClangAttrDump, "gen-clang-attr-dump",
                               "Generate clang attribute dumper"),
                    clEnumValN(GenClangDiagsDefs, "gen-clang-diags-defs",
                               "Generate Clang diagnostics definitions"),
                    clEnumValN(GenClangDiagGroups, "gen-clang-diag-groups",
                               "Generate Clang diagnostic groups"),
                    clEnumValN(GenClangDiagsIndexName,
                               "gen-clang-diags-index-name",
                               "Generate Clang diagnostic name index"),
                    clEnumValN(GenClangCommentNodes, "gen-clang-comment-nodes",
                               "Generate Clang AST comment nodes"),
                    clEnumValN(GenClangDeclNodes, "gen-clang-decl-nodes",
                               "Generate Clang AST declaration nodes"),
                    clEnumValN(GenClangStmtNodes, "gen-clang-stmt-nodes",
                               "Generate Clang AST statement nodes"),
                    clEnumValN(GenClangSACheckers, "gen-clang-sa-checkers",
                               "Generate Clang Static Analyzer checkers"),
                    clEnumValN(GenClangCommentHTMLTags,
                               "gen-clang-comment-html-tags",
                               "Generate efficient matchers for HTML tag "
                               "names that are used in documentation comments"),
                    clEnumValN(GenClangCommentHTMLTagsProperties,
                               "gen-clang-comment-html-tags-properties",
                               "Generate efficient matchers for HTML tag "
                               "properties"),
                    clEnumValN(GenClangCommentHTMLNamedCharacterReferences,
                               "gen-clang-comment-html-named-character-references",
                               "Generate function to translate named character "
                               "references to UTF-8 sequences"),
                    clEnumValN(GenClangCommentCommandInfo,
                               "gen-clang-comment-command-info",
                               "Generate command properties for commands that "
                               "are used in documentation comments"),
                    clEnumValN(GenClangCommentCommandList,
                               "gen-clang-comment-command-list",
                               "Generate list of commands that are used in "
                               "documentation comments"),
                    clEnumValN(GenArmNeon, "gen-arm-neon",
                               "Generate arm_neon.h for clang"),
                    clEnumValN(GenArmNeonSema, "gen-arm-neon-sema",
                               "Generate ARM NEON sema support for clang"),
                    clEnumValN(GenArmNeonTest, "gen-arm-neon-test",
                               "Generate ARM NEON tests for clang"),
                    clEnumValEnd));

  cl::opt<std::string>
  ClangComponent("clang-component",
                 cl::desc("Only use warnings from specified component"),
                 cl::value_desc("component"), cl::Hidden);

bool ClangTableGenMain(raw_ostream &OS, RecordKeeper &Records) {
  switch (Action) {
  case GenClangAttrClasses:
    EmitClangAttrClass(Records, OS);
    break;
  case GenClangAttrExprArgsList:
    EmitClangAttrExprArgsList(Records, OS);
    break;
  case GenClangAttrImpl:
    EmitClangAttrImpl(Records, OS);
    break;
  case GenClangAttrList:
    EmitClangAttrList(Records, OS);
    break;
  case GenClangAttrPCHRead:
    EmitClangAttrPCHRead(Records, OS);
    break;
  case GenClangAttrPCHWrite:
    EmitClangAttrPCHWrite(Records, OS);
    break;
  case GenClangAttrSpellingList:
    EmitClangAttrSpellingList(Records, OS);
    break;
  case GenClangAttrSpellingListIndex:
    EmitClangAttrSpellingListIndex(Records, OS);
    break;
  case GenClangAttrLateParsedList:
    EmitClangAttrLateParsedList(Records, OS);
    break;
  case GenClangAttrTemplateInstantiate:
    EmitClangAttrTemplateInstantiate(Records, OS);
    break;
  case GenClangAttrParsedAttrList:
    EmitClangAttrParsedAttrList(Records, OS);
    break;
  case GenClangAttrParsedAttrKinds:
    EmitClangAttrParsedAttrKinds(Records, OS);
    break;
  case GenClangAttrDump:
    EmitClangAttrDump(Records, OS);
    break;
  case GenClangDiagsDefs:
    EmitClangDiagsDefs(Records, OS, ClangComponent);
    break;
  case GenClangDiagGroups:
    EmitClangDiagGroups(Records, OS);
    break;
  case GenClangDiagsIndexName:
    EmitClangDiagsIndexName(Records, OS);
    break;
  case GenClangCommentNodes:
    EmitClangASTNodes(Records, OS, "Comment", "");
    break;
  case GenClangDeclNodes:
    EmitClangASTNodes(Records, OS, "Decl", "Decl");
    EmitClangDeclContext(Records, OS);
    break;
  case GenClangStmtNodes:
    EmitClangASTNodes(Records, OS, "Stmt", "");
    break;
  case GenClangSACheckers:
    EmitClangSACheckers(Records, OS);
    break;
  case GenClangCommentHTMLTags:
    EmitClangCommentHTMLTags(Records, OS);
    break;
  case GenClangCommentHTMLTagsProperties:
    EmitClangCommentHTMLTagsProperties(Records, OS);
    break;
  case GenClangCommentHTMLNamedCharacterReferences:
    EmitClangCommentHTMLNamedCharacterReferences(Records, OS);
    break;
  case GenClangCommentCommandInfo:
    EmitClangCommentCommandInfo(Records, OS);
    break;
  case GenClangCommentCommandList:
    EmitClangCommentCommandList(Records, OS);
    break;
  case GenArmNeon:
    EmitNeon(Records, OS);
    break;
  case GenArmNeonSema:
    EmitNeonSema(Records, OS);
    break;
  case GenArmNeonTest:
    EmitNeonTest(Records, OS);
    break;
  }

  return false;
}
}

int main(int argc, char **argv) {
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  cl::ParseCommandLineOptions(argc, argv);

  return TableGenMain(argv[0], &ClangTableGenMain);
}
