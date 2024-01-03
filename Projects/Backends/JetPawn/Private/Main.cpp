#include <iostream>
#include <optional>
#include <filesystem>

import Jet.Compiler.BuildProcess;
import Jet.Compiler.Settings;

import Jet.Comp.Foundation;
import Jet.Comp.Format;

import JetPawn.Pawn.Preprocess;

using namespace jet::comp::foundation;

using jet::compiler::CompileResult;
using jet::compiler::CompileError;
using jet::compiler::BuildSettings;
using jet::compiler::CompilerSettings;

using jet::parser::ModuleParse;

auto compile(ModuleParse parse, CompilerSettings& settings) -> CompileResult {
  return error(CompileError{
    .details = "Transpiler not implemented",
  });
}

auto main(int argc, char* argv[]) -> int
{
  using jet::compiler::run_build;
  namespace fmt = jet::comp::fmt;
  namespace jp  = jet_pawn;

  ensure_utf8_in_console();

  auto args = ProgramArgs(argc, argv);

  auto file_name = args[1];
  if (!file_name) {
    fmt::println("Usage:");
    fmt::println("    jet-pawn [module-name]");
    return 0;
  }

  auto preprocess_settings = jp::PreprocessSettings::from_args(args);

  if (auto err = preprocess_settings.err()) {
    fmt::println(std::cerr, "Parsing arguments failed, details:\n{}\n", err->details);
    return 1;
  }

  auto maybe_preprocessed = jp::run_pawn_preprocess(preprocess_settings.get_unchecked());

  if (auto err = maybe_preprocessed.err()) {
    fmt::println(std::cerr, "Preprocessing failed, details:\n{}\n", err->details);
    return err->exit_code;
  }

  auto const& preprocess_result = maybe_preprocessed.get_unchecked();
  auto build_settings = BuildSettings{
    .root_module_path = preprocess_settings.get_unchecked().output.jet_file_path,
    .compiler_backend = compile,
  };

  auto build_result = run_build(build_settings, args);
  if (auto err = build_result.err()) {
    fmt::println(std::cerr, "Compilation failed, details:\n{}\n", err->details);
    return err->exit_code;
  }

  fmt::println("Compilation successful.");
}