export module Jet.Compiler.Compile;

export import Jet.Parser;
export import Jet.Comp.Foundation;
export import Jet.Compiler.Settings;

using namespace jet::comp::foundation;
using jet::parser::ModuleParse;

export namespace jet::compiler
{

auto compile(ModuleParse parse_result, CompilerSettings settings) -> CompileResult;

} // namespace jet::compiler