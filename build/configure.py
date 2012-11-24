# Copyright 2012 Google Inc. All Rights Reserved.

from optparse import OptionParser
import os
import sys

ninja_dir = os.path.join(os.path.split(__file__)[0],
                         '..', 'third_party', 'ninja')
ninja_misc_dir = os.path.join(ninja_dir, 'misc')
if not os.path.exists(ninja_misc_dir):
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
                '.java', '_gcc', '.mm', 'android\\', '_libevent', 'chromeos\\',
                'data\\', '_freebsd', '_nacl', 'linux_', '_glib', '_gtk', 'mac\\',
                'unix_', 'file_descriptor', '_aurax11', 'sha1_win.cc', '_openbsd',
                'xdg_mime', '_kqueue', 'symbolize', 'string16.cc', '_chromeos',
                'nix\\', 'xdg_', 'file_path_watcher_stub.cc', 'dtoa.cc',
                'event_recorder_stubs.cc', '_mock.cc',
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
    return os.path.join('src', filename)
  def base_src(filename):
    return os.path.join('third_party', 'base', filename)
  def built(filename):
    return os.path.join('$builddir', 'obj', filename)
  def cc(name, src=src, **kwargs):
    return n.build(built(name + objext), 'cxx', src(name + '.c'), **kwargs)
  def cxx(name, src=src, **kwargs):
    return n.build(built(name + objext), 'cxx', src(name + '.cc'), **kwargs)
  def binary(name):
    exe = os.path.join('$builddir', name + '.exe')
    n.build(name, 'phony', exe)
    return exe

  if not os.path.exists('out'):
    os.makedirs('out')
  n.variable('builddir', 'out')
  n.variable('cxx', CXX)
  n.variable('ar', 'link')

  cflags = ['/nologo',  # Don't print startup banner.
            '/Zi',  # Create pdb with debug info.
            '/W4',  # Highest warning level.
            '/WX',  # Warnings as errors.
            '/wd4530', '/wd4100', '/wd4706', '/wd4245', '/wd4018',
            '/wd4512', '/wd4800', '/wd4702', '/wd4819', '/wd4355',
            '/wd4996', '/wd4481', '/wd4127', '/wd4310', '/wd4244',
            '/wd4701',
            '/GR-',  # Disable RTTI.
            '/DNOMINMAX', '/D_CRT_SECURE_NO_WARNINGS',
            '/DUNICODE', '/D_UNICODE',
            '/D_CRT_RAND_S', '/DWIN32', '/D_WIN32',
            '-I.', '-Ithird_party']
  if options.debug:
    cflags += ['/D_DEBUG', '/MTd']
  else:
    cflags += ['/DNDEBUG', '/MT']
  ldflags = ['/DEBUG', '/libpath:$builddir\\obj']
  if not options.debug:
    cflags += ['/Ox', '/DNDEBUG', '/GL']
    ldflags += ['/LTCG', '/OPT:REF', '/OPT:ICF']
  libs = []

  n.newline()

  n.variable('cflags', ' '.join(cflags))
  n.variable('ldflags', ' '.join(ldflags))
  n.newline()

  compiler = '%s -o $out -- $cxx /showIncludes' % 'ninja -t msvc'
  n.rule('cxx',
    command='%s $cflags -c $in /Fo$out' % compiler,
    depfile='$out.d',
    description='CXX $out')
  n.newline()

  n.rule('ar',
        command='lib /nologo /ltcg /out:$out $in',
        description='LIB $out')
  n.newline()

  n.rule('link',
        command='$cxx $in $libs /nologo /link $ldflags /out:$out',
        description='LINK $out')
  n.newline()

  objs = []

  n.comment('Core source files all build into sg library.')
  for name in ['backend',
              ]:
    objs += cxx(name)
  sg_lib = n.build(built('sg.lib'), 'ar', objs)
  n.newline() 

  n.comment('Gwen UI lib.')
  n.comment('TODO')
  n.newline()

  n.comment('Chromium base lib.')
  crfiles = GetChromiumBaseFileList('third_party/base')
  objs = []
  for name in crfiles:
    base, ext = os.path.splitext(name)
    if ext == '.c':
      objs += cc(base, src=base_src)
    else:
      objs += cxx(base, src=base_src)
  base_lib = n.build(built('base.lib'), 'ar', objs)
  n.newline()

  libs.extend(['sg.lib', 'base.lib',
               'user32.lib', 'advapi32.lib', 'dbghelp.lib', 'shell32.lib'])

  all_targets = []

  n.comment('Main executable is library plus main() function.')
  objs = cxx('sg')
  sg = n.build(binary('sg'), 'link', objs,
                       implicit=sg_lib + base_lib,
                       variables=[('libs', libs)])
  n.newline()
  all_targets += sg

  n.comment('Tests all build into sg_test executable.')

  variables = []
  test_cflags = None
  test_ldflags = None
  test_libs = libs
  objs = []
  path = 'third_party/testing/gtest'

  gtest_all_incs = ['-I%s' % path,  '-I%s' % os.path.join(path, 'include')]
  gtest_cflags = cflags + ['/nologo', '/EHsc', '/Zi'] + gtest_all_incs
  objs += n.build(built('gtest-all' + objext), 'cxx',
                  os.path.join(path, 'src', 'gtest-all.cc'),
                  variables=[('cflags', gtest_cflags)])
  objs += n.build(built('gtest_main' + objext), 'cxx',
                  os.path.join(path, 'src', 'gtest_main.cc'),
                  variables=[('cflags', gtest_cflags)])

  test_cflags = cflags + ['-DGTEST_HAS_RTTI=0',
                          '-I%s' % os.path.join(path, 'include')]

  for name in []:
    objs += cxx(name, variables=[('cflags', test_cflags)])

  sg_test = n.build(binary('sg_test'), 'link', objs, implicit=sg_lib,
                    variables=[('ldflags', test_ldflags),
                                ('libs', test_libs)])
  n.newline()
  all_targets += sg_test


  n.comment('Regenerate build files if build script changes.')
  n.rule('configure',
          command='python build/configure.py $configure_args',
          generator=True)
  n.build('build.ninja', 'configure',
          implicit=[os.path.normpath('build/configure.py'),
                    os.path.normpath('%s/misc/ninja_syntax.py' % ninja_dir)])
  n.newline()


  n.build('all', 'phony', all_targets)

  print 'wrote %s.' % BUILD_FILENAME

if __name__ == '__main__':
  main()
