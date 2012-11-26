#include "source_view.h"

#include "base/file_util.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "Gwen/Gwen.h"

using namespace Gwen;
using namespace Gwen::Controls;

GWEN_CONTROL_CONSTRUCTOR(SourceView) {
  Dock(Pos::Fill);
  color_ = Colors::Black;
  string16 text = L"File\nload\nfailed!";
  std::string utf8_text;
  if (file_util::ReadFileToString(
      FilePath(FILE_PATH_LITERAL("sample_source_code_file.cc")), &utf8_text)) {
    text = UTF8ToUTF16(utf8_text);
  }
  // TODO: Each line is lexed at load time: List of (Color, Text) pairs.
  //       Based on http://pygments.org/docs/lexerdevelopment/
  //            and https://bitbucket.org/birkenfeld/pygments-main/src/24c16194941a7464a0cbe27896cc103b9bbab6a9/pygments/lexers/compiled.py?at=default#cl-172
  //       maybe?
  // TODO: tab to spaces
  base::SplitString(text, L'\n', &lines_);
}

void SourceView::Render(Skin::Base* skin) {
  GetCanvas()->SetBackgroundColor(Colors::White);
  skin->GetRender()->SetDrawColor(color_);
  const size_t line_height = 13;
  for (size_t i = 0; i < lines_.size(); ++i) {
    if (i * line_height > Height())
      break;
    skin->GetRender()->RenderText(
        skin->GetDefaultFont(),
        Gwen::Point(0, i * line_height),
        lines_[i].c_str());
  }
}
