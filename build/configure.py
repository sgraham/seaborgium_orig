# Copyright 2012 Google Inc. All Rights Reserved.

from optparse import OptionParser
import os
import sys

ninja_dir = os.path.join(os.path.split(__file__)[0],
                         '..', 'third_party', 'ninja')
ninja_misc_dir = os.path.join(ninja_dir, 'misc')
print 'Updating git submodules...'
if os.system('git submodule update --init') != 0:
  raise SystemExit("Couldn't update git submodules")

sys.path.append(ninja_misc_dir)
import ninja_syntax

def GetChromiumBaseFileList(base_dir):
  def get_file_list():
    all_files = []
    orig = os.getcwd()
    os.chdir(base_dir)
    for path, dirs, files in os.walk('.'):
      if os.path.normpath(path).startswith('.git'):
        continue
      for file in files:
        all_files.append(os.path.normpath(os.path.join(path, file)))
    os.chdir(orig)
    return all_files


  def filter_file_list(all_files, for_types):
    result = all_files[:]
    if 'win' in for_types:
      for x in ('_posix', '_mac', '_android', '_linux', '_ios', '_solaris',
                '.java', '_gcc', '.mm', 'android\\', '_libevent',
                'chromeos\\', 'data\\', '_freebsd', '_nacl', 'linux_',
                '_glib', '_gtk', 'mac\\', 'unix_', 'file_descriptor',
                '_aurax11', 'sha1_win.cc', '_openbsd', 'xdg_mime', '_kqueue',
                'symbolize', 'string16.cc', '_chromeos', 'nix\\', 'xdg_',
                'file_path_watcher_stub.cc', 'dtoa.cc',
                'event_recorder_stubs.cc', '_mock.cc', 'check_example.cc',
                'dynamic_annotations.c', # avoid futzing with .c
                'debug_message.cc',
                'allocator\\', # Kind of overly involved for user-configuration.
                'field_trial.cc', # Has screwy winsock inclusion, don't need it.
                'i18n\\', # Requires icu (I think)
                ):
        result = [y for y in result if x not in y]
    if 'lib' in for_types:
      for x in ('README', 'LICENSE', 'OWNERS', '.h', '.patch', 'unittest',
                'PRESUBMIT', 'DEPS', '.gyp', '.py', '.isolate', '.nc', 'test\\',
                '.git', '_browsertest.cc', 'base64.cc' # TEMP
                ):
        result = [y for y in result if x not in y]
    if 'test' in for_types:
      for x in ('README', 'LICENSE', 'OWNERS', '.h', '.patch',
                'PRESUBMIT', 'DEPS', '.gyp', '.py', '.isolate', 'test\\',
                'base64.cc' # TEMP
                ):
        result = [y for y in result if x not in y]
    return result

  files = get_file_list()
  return filter_file_list(files, ('win', 'lib'))

def GetGwenFileList():
  files = [
    'gwen/src/Anim',
    'gwen/src/BaseRender',
    'gwen/src/ControlList',
    'gwen/src/DragAndDrop',
    'gwen/src/events',
    'gwen/src/Gwen',
    'gwen/src/Hook',
    'gwen/src/inputhandler',
    'gwen/src/Skin',
    'gwen/src/ToolTip',
    'gwen/src/Utility',
    'gwen/src/Controls/Base',
    'gwen/src/Controls/Button',
    'gwen/src/Controls/Canvas',
    'gwen/src/Controls/CheckBox',
    'gwen/src/Controls/CollapsibleCategory',
    'gwen/src/Controls/ColorControls',
    'gwen/src/Controls/ColorPicker',
    'gwen/src/Controls/ComboBox',
    'gwen/src/Controls/CrossSplitter',
    'gwen/src/Controls/DockBase',
    'gwen/src/Controls/DockedTabControl',
    'gwen/src/Controls/Dragger',
    'gwen/src/Controls/GroupBox',
    'gwen/src/Controls/HorizontalScrollBar',
    'gwen/src/Controls/HorizontalSlider',
    'gwen/src/Controls/HSVColorPicker',
    'gwen/src/Controls/ImagePanel',
    'gwen/src/Controls/Label',
    'gwen/src/Controls/LabelClickable',
    'gwen/src/Controls/ListBox',
    'gwen/src/Controls/Menu',
    'gwen/src/Controls/MenuItem',
    'gwen/src/Controls/MenuStrip',
    'gwen/src/Controls/NumericUpDown',
    'gwen/src/Controls/PageControl',
    'gwen/src/Controls/ProgressBar',
    'gwen/src/Controls/Properties',
    'gwen/src/Controls/PropertyTree',
    'gwen/src/Controls/RadioButton',
    'gwen/src/Controls/RadioButtonController',
    'gwen/src/Controls/Rectangle',
    'gwen/src/Controls/ResizableControl',
    'gwen/src/Controls/Resizer',
    'gwen/src/Controls/RichLabel',
    'gwen/src/Controls/ScrollBar',
    'gwen/src/Controls/ScrollBarBar',
    'gwen/src/Controls/ScrollBarButton',
    'gwen/src/Controls/ScrollControl',
    'gwen/src/Controls/Slider',
    'gwen/src/Controls/SplitterBar',
    'gwen/src/Controls/TabButton',
    'gwen/src/Controls/TabControl',
    'gwen/src/Controls/TabStrip',
    'gwen/src/Controls/Text',
    'gwen/src/Controls/TextBox',
    'gwen/src/Controls/TextBoxNumeric',
    'gwen/src/Controls/TreeControl',
    'gwen/src/Controls/TreeNode',
    'gwen/src/Controls/VerticalScrollBar',
    'gwen/src/Controls/VerticalSlider',
    'gwen/src/Controls/WindowCanvas',
    'gwen/src/Controls/WindowControl',
    'gwen/src/Controls/Dialog/FileOpen',
    'gwen/src/Controls/Dialog/FileSave',
    'gwen/src/Controls/Dialog/FolderOpen',
    'gwen/src/Controls/Dialog/Query',
    'gwen/src/Platforms/Null',
    'gwen/src/Platforms/Windows',

    # These are fairly temporary; using Gwen sample renderer.
    'gwen/Renderers/Direct2D/Direct2D',
    ]
  return [os.path.normpath(p) for p in files]


def GetRe2FileList():
  files = [
    're2/bitstate',
    're2/compile',
    're2/dfa',
    're2/filtered_re2',
    're2/mimics_pcre',
    're2/nfa',
    're2/onepass',
    're2/parse',
    're2/perl_groups',
    're2/prefilter',
    're2/prefilter_tree',
    're2/prog',
    're2/re2',
    're2/regexp',
    're2/set',
    're2/simplify',
    're2/tostring',
    're2/unicode_casefold',
    're2/unicode_groups',
    'util/arena',
    'util/hash',
    'util/rune',
    'util/stringpiece',
    'util/stringprintf',
    'util/strutil',
    'util/valgrind',
    ]
  return [os.path.normpath(p) for p in files]



def main():
  if not os.path.exists(os.path.join(ninja_dir, 'ninja.exe')):
    print "ninja binary doesn't exist, trying to build it..."
    old_dir = os.getcwd()
    os.chdir(ninja_dir)
    if os.system('%s bootstrap.py' % sys.executable) != 0:
      raise SystemExit('Failed to bootstrap ninja')
    os.chdir(old_dir)

  parser = OptionParser()
  parser.add_option('--debug', action='store_true',
                    help='enable debugging extras',)
  (options, args) = parser.parse_args()
  if args:
    print 'ERROR: extra unparsed command-line arguments:', args
    sys.exit(1)

  BUILD_FILENAME = 'build.ninja'
  buildfile = open(BUILD_FILENAME, 'w')
  n = ninja_syntax.Writer(buildfile)

  n.comment('The arguments passed to configure.py, for rerunning it.')
  n.variable('configure_args', ' '.join(sys.argv[1:]))
  n.newline()

  CXX = 'cl'
  objext = '.obj'

  def src(filename):
    return os.path.join('sg', filename)
  def base_src(filename):
    return os.path.join('third_party', 'base', filename)
  def gwen_src(filename):
    return os.path.join('third_party', 'gwen', filename)
  def re2_src(filename):
    return os.path.join('third_party', 're2', filename)
  def built(filename):
    return os.path.join('$builddir', 'obj', filename)
  pch_name = built('sg.pch')
  pch_implicit = None
  pch_compile = ''
  if options.debug:
    pch_implicit = pch_name
    pch_compile = '/Fp' + pch_name + ' /Yusg/global.h'
  def cxx(name, src=src, **kwargs):
    return n.build(built(name + objext), 'cxx', src(name + '.cc'),
                   implicit=pch_implicit, **kwargs)
  def cpp(name, src=src, **kwargs):
    return n.build(built(name + objext), 'cxx', src(name + '.cpp'),
                   implicit=pch_implicit, **kwargs)
  def rc(name, src=src, **kwargs):
    return n.build(built(name + objext), 'rc', src(name + '.rc'), **kwargs)
  def binary(name):
    exe = os.path.join('$builddir', name + '.exe')
    n.build(name, 'phony', exe)
    return exe

  if not os.path.exists('out'):
    os.makedirs('out')
  n.variable('builddir', 'out')
  n.variable('cxx', CXX)

  cflags = ['/nologo',  # Don't print startup banner.
            '/Zi',  # Create pdb with debug info.
            '/W4',  # Highest warning level.
            '/WX',  # Warnings as errors.
            '/wd4530', '/wd4100', '/wd4706', '/wd4245', '/wd4018',
            '/wd4512', '/wd4800', '/wd4702', '/wd4819', '/wd4355',
            '/wd4996', '/wd4481', '/wd4127', '/wd4310', '/wd4244',
            '/wd4701', '/wd4201', '/wd4389', '/wd4722',
            '/GR-',  # Disable RTTI.
            '/DNOMINMAX', '/D_CRT_SECURE_NO_WARNINGS',
            '/DUNICODE', '/D_UNICODE',
            '/D_CRT_RAND_S', '/DWIN32', '/D_WIN32',
            '/D_WIN32_WINNT=0x0601', '/D_VARIADIC_MAX=10',
            '/DDYNAMIC_ANNOTATIONS_ENABLED=0',
            '/Fd$builddir\\sg_intermediate.pdb',
            '-I.', '-Ithird_party', '-Ithird_party/gwen/gwen/include',
            '-Ithird_party/re2',
            '-FIsg/global.h']
  if options.debug:
    cflags += ['/D_DEBUG', '/MTd']
  else:
    cflags += ['/DNDEBUG', '/MT']
  ldflags = ['/DEBUG', '/SUBSYSTEM:WINDOWS']
  if not options.debug:
    cflags += ['/Ox', '/DNDEBUG', '/GL']
    ldflags += ['/LTCG', '/OPT:REF', '/OPT:ICF']
  else:
    ldflags += ['/INCREMENTAL']
  libs = []

  n.newline()

  n.variable('cflags', ' '.join(cflags))
  n.variable('ldflags', ' '.join(ldflags))
  n.newline()

  compiler = 'ninja -t msvc -o $out -- $cxx /showIncludes'
  n.rule('cxx',
    command=('%s $cflags %s '
             '-c $in /Fo$out' % (compiler, pch_compile)),
    depfile='$out.d',
    description='CXX $out')
  n.newline()

  n.rule('link',
        command='$cxx $in $libs /nologo /link $ldflags /out:$out',
        description='LINK $out')
  n.newline()

  n.rule('rc',
      command='rc /r /nologo /fo $out $in',
        description='RC $out')
  n.newline()

  pch_objs = []
  if options.debug:
    compiler = 'ninja -t msvc -o $objname -- $cxx /showIncludes'
    n.rule('cxx_pch',
      command=('%s $cflags /Ycsg/global.h %s '
              '-c $in /Fo$objname' % (compiler, pch_compile)),
      depfile=built('sg_pch.obj.d'),
      description='CXX $out')
    n.newline()

    n.comment('Build the precompiled header.')

    n.build([built('sg_pch.obj'), built('sg.pch')], 'cxx_pch', src('sg_pch.cc'),
            variables=[('objname', built('sg_pch.obj'))])
    pch_objs = [built('sg_pch.obj')]
    n.newline()

  sg_objs = []
  n.comment('Core source files.')
  for name in [
               'app_thread',
               'backend\\backend_native_win',
               'backend\\debug_core_gdb',
               'backend\\debug_core_native_win',
               'backend\\gdb_mi_parse',
               'backend\\process_native_win',
               'backend\\subprocess_win',
               'cpp_lexer',
               'debug_presenter',
               'debug_presenter_display',
               'lexer',
               'lexer_state',
               'main_loop',
               'source_files',
               'source_view',
               'status_bar',
               'ui\\container',
               'ui\\contents',
               'ui\\focus',
               'ui\\skin',
               'workspace',
              ]:
    sg_objs += cxx(name)
  n.newline() 

  gwen_objs = []
  n.comment('Gwen.')
  for base in GetGwenFileList():
    gwen_objs += cpp(base, src=gwen_src)
  n.newline()

  re2_objs = []
  n.comment('RE2.')
  for base in GetRe2FileList():
    re2_objs += cxx(base, src=re2_src)
  n.newline()

  n.comment('Chromium base.')
  crfiles = GetChromiumBaseFileList('third_party/base')
  base_objs = []
  for name in crfiles:
    base, ext = os.path.splitext(name)
    base_objs += cxx(base, src=base_src)
  n.newline()

  libs = ['advapi32.lib',
          'comdlg32.lib',
          'dbghelp.lib',
          'd2d1.lib',
          'dwrite.lib',
          'gdi32.lib',
          'ole32.lib',
          'oleaut32.lib',
          'opengl32.lib',
          'shell32.lib',
          'user32.lib',
          'version.lib',
          'windowscodecs.lib',
         ]

  all_targets = []

  app_objs = sg_objs + base_objs + gwen_objs + re2_objs + pch_objs

  n.comment('Main executable is library plus main() and some startup goop.')
  main_objs = []
  main_objs += cxx('application')
  main_objs += cxx('application_window_win')
  main_objs += cxx('gpu_win')
  main_objs += cxx('main_win')
  main_objs += rc('sg', implicit=['art\\sg.ico'])
  # No .libs for /incremental to work.
  sg_binary = binary('sg')
  sg = n.build(sg_binary, 'link', inputs=main_objs + app_objs,
               variables=[('libs', libs)])
  n.newline()
  all_targets += sg

  n.comment('Tests all build into sg_test executable.')

  variables = []
  test_cflags = None
  test_ldflags = ldflags + ['/SUBSYSTEM:CONSOLE']
  test_libs = libs
  test_objs = []
  path = 'third_party/testing/gtest'

  gtest_all_incs = ['-I%s' % path,  '-I%s' % os.path.join(path, 'include')]
  gtest_cflags = cflags + gtest_all_incs
  test_objs += n.build(built('gtest-all' + objext), 'cxx',
                       inputs=os.path.join(path, 'src', 'gtest-all.cc'),
                       implicit=built('sg.pch'),
                       variables=[('cflags', gtest_cflags)])
  test_objs += n.build(built('gtest_main' + objext), 'cxx',
                       inputs='sg/main_test.cc',
                       implicit=built('sg.pch'),
                       variables=[('cflags', gtest_cflags)])

  test_cflags = cflags + ['-DGTEST_HAS_RTTI=0',
                          '-I%s' % os.path.join(path, 'include')]

  for name in [
               'lexer_test',
               'backend\\debug_core_native_win_test',
               'backend\\debug_core_gdb_test',
               'backend\\gdb_mi_parse_test',
               'backend\\subprocess_test',
              ]:
    test_objs += cxx(name, variables=[('cflags', test_cflags)])

  sg_test = n.build(binary('sg_test'), 'link', inputs=test_objs + app_objs,
                    # Unnecessary but orders pdb access so test link isn't
                    # accessing the intermediate pdb while the main-app-only
                    # objs are still compiling.
                    order_only=sg_binary,
                    variables=[('ldflags', test_ldflags),
                               ('libs', test_libs)])
  all_targets += sg_test
  n.newline()

  reader_writer_objs = []
  reader_writer_objs += cxx('backend\\reader_writer_test')
  reader_writer_test = n.build(binary('reader_writer_test'), 'link',
                               inputs=reader_writer_objs,
                               variables=[('ldflags', test_ldflags)])
  all_targets += reader_writer_test
  n.newline()

  n.comment('Regenerate build files if build script changes.')
  n.rule('configure',
          command='cmd /c python build/configure.py $configure_args',
          generator=True)
  n.build('build.ninja', 'configure',
          implicit=[os.path.normpath('build/configure.py'),
                    os.path.normpath('%s/misc/ninja_syntax.py' % ninja_dir)])
  n.newline()


  n.build('all', 'phony', all_targets)

  print 'wrote %s.' % BUILD_FILENAME

if __name__ == '__main__':
  main()
