import os
import sys

root = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
lint_path = os.path.normpath(os.path.join(root, 'third_party/cpplint'))
sys.path.append(lint_path)
import cpplint
import cpplint_chromium

src_root = os.path.normpath(os.path.join(root, 'sg'))
os.chdir(src_root)
extra_check_functions = [cpplint_chromium.CheckPointerDeclarationWhitespace]
for root, dirs, files in os.walk(src_root):
  for file in files:
    ext = os.path.splitext(file)[1]
    if ext == '.cc' or ext == '.h':
      cpplint.ProcessFile(os.path.join(root, file),
                          cpplint._cpplint_state.verbose_level,
                          extra_check_functions)

print "Total errors found: %d\n" % cpplint._cpplint_state.error_count
