module;

#include <string>
#include <variant>

export module JetPawn.Pawn.Preprocess;

import Jet.Comp.Foundation;
using namespace jet::comp::foundation;


export namespace jet_pawn
{

struct PawnPreprocessError
{
  std::string details;
  int         exit_code = 1;
};

struct ParseArgsError
{
  std::string details;
};

struct PreprocessSettings
{
  struct Source
  {
    Path file_path;
  } source;

  struct Output
  {
    Path pawn_file_path;
    Path jet_file_path;
  } output;

  bool verbose = false;

  static auto from_args(ProgramArgs const& args) -> Result<PreprocessSettings, ParseArgsError>;
};

auto run_pawn_preprocess(PreprocessSettings const& settings) -> Result<std::monostate, PawnPreprocessError>;

} // namespace jet_pawn