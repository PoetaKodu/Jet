export module Jet.Compiler.Settings;

export import Jet.Comp.Foundation;
export import Jet.Parser.ModuleParse;
using namespace jet::comp::foundation;

using jet::parser::ModuleParse;

export namespace jet::compiler
{

struct Settings;

auto make_settings_from_args(ProgramArgs const& args) -> Settings;

struct CompileError
{
  String details;
};

using CompileResult = Result<int, CompileError>;

using CompilerBackendFn = CompileResult(ModuleParse, Settings&);

struct Settings
{
  // TODO: this should be optional.
  String root_module_name;
  Func<CompilerBackendFn> compiler_backend;

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