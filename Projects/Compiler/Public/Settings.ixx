export module Jet.Compiler.Settings;

export import Jet.Comp.Foundation;
export import Jet.Parser.ModuleParse;
using namespace jet::comp::foundation;

using jet::parser::ModuleParse;

export namespace jet::compiler
{

struct CompilerSettings;

auto make_compiler_settings_from_args(ProgramArgs const& args) -> CompilerSettings;

struct CompileError
{
  String details;
};

using CompileResult = Result<int, CompileError>;

using CompilerBackendFn = CompileResult(ModuleParse, CompilerSettings&);

struct CompilerSettings
{
  struct Output
  {
    Opt<String> llvm_ir_file_name;
    Opt<String> binary_name;
  };

  Output output;
  bool   cleanup_intermediate = true;

  auto should_output_llvm_ir() const -> bool;
  auto should_output_binary() const -> bool;
  auto should_cleanup_intermediate() const -> bool;
};

} // namespace jet::compiler