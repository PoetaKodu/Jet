module;

#include <iostream>
#include <variant>
#include <cassert>

module Jet.Compiler.BuildProcess;

import Jet.Compiler.Compile;
import Jet.Parser;
import Jet.Core.Module;
import Jet.Core.File;

import Jet.Comp.Format;

namespace jet::compiler
{

auto run_build(BuildSettings settings, ProgramArgs const& args) -> BuildResult
{
  static auto constexpr NOT_READY_ERROR = StringView(
    "build cannot be run with given setup - BuildState was not ready "
    "after being configured using specified program arguments"
  );

  auto build_state     = BuildState();
  build_state.settings = make_compiler_settings_from_args(args);

  if (!build_state.can_start()) {
    return error(BuildError{1, String(NOT_READY_ERROR)});
  }

  return begin_build(settings, build_state);
}

auto begin_build(BuildSettings& settings, BuildState& state) -> BuildResult
{
  namespace fmt = jet::comp::fmt;
  using core::read_file, core::find_module;
  using parser::parse;
  using compiler::compile;

  assert(state.can_start() && "begin_build() called on a state that is not ready.");

  auto& file_path = settings.root_module_path;

  auto module_path = find_module(file_path);
  if (!module_path) {
    return error(BuildError{1, "cannot find module file"});
  }

  auto file_content = read_file(*module_path);
  if (!file_content) {
    return error(BuildError{1, "cannot open module file"});
  }

  if (file_content->empty()) {
    return error(BuildError{1, "module file is empty"});
  }

  auto maybe_parsed = parse(*file_content);
  if (auto failed_parse = maybe_parsed.err()) {
    auto msg = fmt::format("module parse failed, details: {}", failed_parse->details);
    return error(BuildError{1, msg});
  }
  auto compile_result = settings.compiler_backend(maybe_parsed.get_unchecked(), state.settings);

  if (auto err = compile_result.err()) {
    return error(BuildError{1, err->details});
  }

  return success(std::monostate{});
}

} // namespace jet::compiler