module;

#include <variant>
#include <filesystem>
#include <fstream>
#include <cassert>

module JetPawn.Pawn.Preprocess;

import Jet.Comp.Format;
import Jet.Core.File;
using namespace jet::comp::foundation;


namespace jet_pawn
{

static auto constexpr JET_START_DIRECTIVE = "#jet";
static auto constexpr JET_END_DIRECTIVE   = "#end_jet";

struct CodeExtraction
{
  String pawn_code;
  String jet_code;
};

struct CodeExtractionError
{
  String details;
};

struct CodeExtractorContext
{
  StringView file_content;
  usize      line_start  = 0;
  usize      line_number = 0;
  String     pawn_code;
  String     jet_code;
  bool       in_jet_block;
};

static auto use_default_output(PreprocessSettings& settings) -> void;
static auto display_startup_message(PreprocessSettings const& settings) -> void;
static auto extract_code(Path const& input_file) -> Result<CodeExtraction, CodeExtractionError>;
static auto split_pawn_and_jet_code(StringView file_content, String& pawn_code, String& jet_code) -> void;
static auto try_extract_next_source_line(CodeExtractorContext& ctx) -> bool;

auto run_pawn_preprocess(PreprocessSettings const& settings) -> Result<std::monostate, PawnPreprocessError>
{
  if (settings.verbose) {
    display_startup_message(settings);
  }

  auto maybe_code_extraction = extract_code(settings.source.file_path);

  if (auto err = maybe_code_extraction.err()) {
    return error(PawnPreprocessError{
      .details = std::move(err->details),
    });
  }

  auto const& code_extraction = maybe_code_extraction.get_unchecked();

  jet::core::overwrite_file(settings.output.pawn_file_path, code_extraction.pawn_code);
  jet::core::overwrite_file(settings.output.jet_file_path, code_extraction.jet_code);

  return success(std::monostate{});
}

auto PreprocessSettings::from_args(ProgramArgs const& args) -> Result<PreprocessSettings, ParseArgsError>
{
  auto input_file_name = args[1];

  assert(input_file_name.has_value() && "Preprocessing started with no input file.");

  auto result             = PreprocessSettings{};
  result.source.file_path = Path(*input_file_name);

  // TODO: handle --output arguments
  {
    use_default_output(result);
  }

  result.verbose = args.contains("--verbose");

  return success(std::move(result));
}

auto display_startup_message(PreprocessSettings const& settings) -> void
{
  namespace fmt = jet::comp::fmt;

  fmt::println(
    "    Source file: {}\n"
    "    Output files:\n"
    "      - output pawn: {}\n"
    "      - output jet: {}",
    settings.source.file_path.string(),
    settings.output.pawn_file_path.string(),
    settings.output.jet_file_path.string()
  );
}

auto use_default_output(PreprocessSettings& settings) -> void
{
  settings.output.pawn_file_path = settings.source.file_path;
  settings.output.jet_file_path  = settings.source.file_path;

  settings.output.pawn_file_path.replace_extension(".p.inc");
  settings.output.jet_file_path.replace_extension(".jet");
}

auto extract_code(Path const& input_file) -> Result<CodeExtraction, CodeExtractionError>
{
  auto maybe_file_content = jet::core::read_file(input_file);

  if (!maybe_file_content.has_value()) {
    return error(CodeExtractionError{
      .details = "Cannot open input file.",
    });
  }

  auto const& file_content = maybe_file_content.value();

  auto result = CodeExtraction{
    .pawn_code = String(),
    .jet_code  = String(),
  };

  result.pawn_code.reserve(file_content.size());
  result.jet_code.reserve(file_content.size());

  split_pawn_and_jet_code(file_content, result.pawn_code, result.jet_code);

  return success(std::move(result));
}

auto split_pawn_and_jet_code(StringView file_content, String& pawn_code, String& jet_code) -> void
{
  auto context = CodeExtractorContext{
    .file_content = file_content,
    .line_start   = 0,
    .line_number  = 1,
    .pawn_code    = String(),
    .jet_code     = String(),
    .in_jet_block = false,
  };

  while (try_extract_next_source_line(context)) {
    // Nothing to do here.
  }

  pawn_code = std::move(context.pawn_code);
  jet_code  = std::move(context.jet_code);
}

auto try_extract_next_source_line(CodeExtractorContext& ctx) -> bool
{
  namespace fmt = jet::comp::fmt;

  if (ctx.line_start >= ctx.file_content.size()) {
    return false;
  }

  auto line_end = ctx.file_content.find('\n', ctx.line_start);
  if (line_end == StringView::npos) {
    line_end = ctx.file_content.size();
  } else {
    line_end += 1; // with the newline character
  }

  auto const line = ctx.file_content.substr(ctx.line_start, line_end - ctx.line_start);

  if (line.starts_with(JET_START_DIRECTIVE)) {
    ctx.in_jet_block = true;
  }
  else if (line.starts_with(JET_END_DIRECTIVE)) {
    if (!ctx.in_jet_block) {
      fmt::println("Warning: {} (L:{}) found without matching #jet start directive.", JET_END_DIRECTIVE, ctx.line_number);
    }
    ctx.in_jet_block = false;
  }
  else if (ctx.in_jet_block) {
    ctx.jet_code += line;
  }
  else {
    ctx.pawn_code += line;
  }

  ctx.line_start = line_end;
  ctx.line_number += 1;
  return true;
}

} // namespace jet_pawn