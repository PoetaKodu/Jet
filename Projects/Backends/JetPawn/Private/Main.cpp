#include <iostream>
#include <optional>
#include <filesystem>

import Jet.Compiler.BuildProcess;
import Jet.Compiler.Settings;

import Jet.Comp.Foundation;
import Jet.Comp.Format;

import JetPawn.Pawn.Preprocess;

using namespace jet::comp::foundation;

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

  auto preprocess_result = jp::run_pawn_preprocess(preprocess_settings.get_unchecked());

  if (auto err = preprocess_result.err()) {
    fmt::println(std::cerr, "Preprocessing failed, details:\n{}\n", err->details);
    return err->exit_code;
  }

  auto build_result = run_build(args);
  if (auto err = build_result.err()) {
    fmt::println(std::cerr, "Compilation failed, details:\n{}\n", err->details);
    return err->exit_code;
  }

  fmt::println("Compilation successful.");
}