# Copyright 2013 The Chromium Authors. All Rights Reserved.

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

def GetChromiumBaseFileList(base_dir, platform):
  def get_file_list():
    files = [
      'android/activity_status.cc',
      'android/base_jni_registrar.cc',
      'android/build_info.cc',
      'android/context_types.cc',
      'android/cpu_features.cc',
      'android/important_file_writer_android.cc',
      'android/jni_android.cc',
      'android/jni_array.cc',
      'android/jni_helper.cc',
      'android/jni_registrar.cc',
      'android/jni_string.cc',
      'android/path_service_android.cc',
      'android/path_utils.cc',
      'android/scoped_java_ref.cc',
      'at_exit.cc',
      'atomicops_internals_x86_gcc.cc',
      'base64.cc',
      'base_paths.cc',
      'base_paths_android.cc',
      'base_paths_mac.mm',
      'base_paths_posix.cc',
      'base_paths_win.cc',
      'base_switches.cc',
      'bind_helpers.cc',
      'build_time.cc',
      'callback_internal.cc',
      'chromeos/chromeos_version.cc',
      'command_line.cc',
      'cpu.cc',
      'critical_closure_ios.mm',
      'debug/alias.cc',
      'debug/crash_logging.cc',
      'debug/debug_on_start_win.cc',
      'debug/debugger.cc',
      'debug/debugger_posix.cc',
      'debug/debugger_win.cc',
      'debug/profiler.cc',
      'debug/stack_trace.cc',
      'debug/stack_trace_android.cc',
      'debug/stack_trace_ios.mm',
      'debug/stack_trace_posix.cc',
      'debug/stack_trace_win.cc',
      'debug/trace_event_android.cc',
      'debug/trace_event_impl.cc',
      'debug/trace_event_win.cc',
      'deferred_sequenced_task_runner.cc',
      'environment.cc',
      'event_recorder_stubs.cc',
      'event_recorder_win.cc',
      'file_util.cc',
      'file_util_android.cc',
      'file_util_linux.cc',
      'file_util_mac.mm',
      'file_util_posix.cc',
      'file_util_win.cc',
      'file_version_info_mac.mm',
      'file_version_info_win.cc',
      'files/file_path.cc',
      'files/file_path_constants.cc',
      'files/file_path_watcher.cc',
      'files/file_path_watcher_kqueue.cc',
      'files/file_path_watcher_linux.cc',
      'files/file_path_watcher_win.cc',
      'files/file_util_proxy.cc',
      'files/important_file_writer.cc',
      'files/memory_mapped_file.cc',
      'files/memory_mapped_file_posix.cc',
      'files/memory_mapped_file_win.cc',
      'files/scoped_temp_dir.cc',
      'guid.cc',
      'guid_posix.cc',
      'guid_win.cc',
      'hash.cc',
      'hi_res_timer_manager_posix.cc',
      'hi_res_timer_manager_win.cc',
      'ios/device_util.mm',
      'ios/device_util_unittest.mm',
      'ios/ios_util.mm',
      'ios/scoped_critical_action.mm',
      'json/json_file_value_serializer.cc',
      'json/json_parser.cc',
      'json/json_reader.cc',
      'json/json_string_value_serializer.cc',
      'json/json_writer.cc',
      'json/string_escape.cc',
      'lazy_instance.cc',
      'linux_util.cc',
      'location.cc',
      'logging.cc',
      'logging_win.cc',
      'mac/authorization_util.mm',
      'mac/bind_objc_block_unittest.mm',
      'mac/bundle_locations.mm',
      'mac/foundation_util.mm',
      'mac/foundation_util_unittest.mm',
      'mac/launch_services_util.cc',
      'mac/launchd.cc',
      'mac/libdispatch_task_runner.cc',
      'mac/mac_logging.cc',
      'mac/mac_util.mm',
      'mac/mac_util_unittest.mm',
      'mac/objc_property_releaser.mm',
      'mac/objc_property_releaser_unittest.mm',
      'mac/os_crash_dumps.cc',
      'mac/scoped_mach_port.cc',
      'mac/scoped_nsautorelease_pool.mm',
      'mac/scoped_nsexception_enabler.mm',
      'mac/scoped_sending_event.mm',
      'mac/scoped_sending_event_unittest.mm',
      'md5.cc',
      'memory/aligned_memory.cc',
      'memory/discardable_memory.cc',
      'memory/discardable_memory_android.cc',
      'memory/discardable_memory_mac.cc',
      'memory/ref_counted.cc',
      'memory/ref_counted_memory.cc',
      'memory/scoped_nsobject_unittest.mm',
      'memory/shared_memory_android.cc',
      'memory/shared_memory_nacl.cc',
      'memory/shared_memory_posix.cc',
      'memory/shared_memory_win.cc',
      'memory/singleton.cc',
      'memory/weak_ptr.cc',
      'message_loop.cc',
      'message_loop/message_loop_proxy.cc',
      'message_loop/message_loop_proxy_impl.cc',
      'message_pump.cc',
      'message_pump_android.cc',
      'message_pump_aurax11.cc',
      'message_pump_default.cc',
      'message_pump_glib.cc',
      'message_pump_gtk.cc',
      'message_pump_io_ios.cc',
      'message_pump_libevent.cc',
      'message_pump_mac.mm',
      'message_pump_ozone.cc',
      'message_pump_win.cc',
      'metrics/bucket_ranges.cc',
      'metrics/histogram.cc',
      'metrics/histogram_base.cc',
      'metrics/histogram_samples.cc',
      'metrics/histogram_snapshot_manager.cc',
      'metrics/sample_map.cc',
      'metrics/sample_vector.cc',
      'metrics/sparse_histogram.cc',
      'metrics/statistics_recorder.cc',
      'metrics/stats_counters.cc',
      'metrics/stats_table.cc',
      'native_library_mac.mm',
      'native_library_posix.cc',
      'native_library_win.cc',
      'nix/mime_util_xdg.cc',
      'nix/xdg_util.cc',
      'os_compat_android.cc',
      'os_compat_nacl.cc',
      'path_service.cc',
      'pending_task.cc',
      'perftimer.cc',
      'pickle.cc',
      'platform_file.cc',
      'platform_file_posix.cc',
      'platform_file_win.cc',
      'posix/file_descriptor_shuffle.cc',
      'posix/global_descriptors.cc',
      'posix/unix_domain_socket_linux.cc',
      'power_monitor/power_monitor.cc',
      'power_monitor/power_monitor_android.cc',
      'power_monitor/power_monitor_ios.mm',
      'power_monitor/power_monitor_mac.mm',
      'power_monitor/power_monitor_posix.cc',
      'power_monitor/power_monitor_win.cc',
      'prefs/default_pref_store.cc',
      'prefs/json_pref_store.cc',
      'prefs/overlay_user_pref_store.cc',
      'prefs/pref_change_registrar.cc',
      'prefs/pref_member.cc',
      'prefs/pref_notifier_impl.cc',
      'prefs/pref_registry.cc',
      'prefs/pref_registry_simple.cc',
      'prefs/pref_service.cc',
      'prefs/pref_service_builder.cc',
      'prefs/pref_store.cc',
      'prefs/pref_value_map.cc',
      'prefs/pref_value_store.cc',
      'prefs/value_map_pref_store.cc',
      'process_info_mac.cc',
      'process_info_win.cc',
      'process_linux.cc',
      'process_posix.cc',
      'process_util.cc',
      'process_util_freebsd.cc',
      'process_util_ios.mm',
      'process_util_linux.cc',
      'process_util_mac.mm',
      'process_util_openbsd.cc',
      'process_util_posix.cc',
      'process_util_unittest_mac.mm',
      'process_util_win.cc',
      'process_win.cc',
      'profiler/alternate_timer.cc',
      'profiler/scoped_profile.cc',
      'profiler/tracked_time.cc',
      'rand_util.cc',
      'rand_util_nacl.cc',
      'rand_util_posix.cc',
      'rand_util_win.cc',
      'run_loop.cc',
      'safe_strerror_posix.cc',
      'scoped_native_library.cc',
      'sequence_checker_impl.cc',
      'sequenced_task_runner.cc',
      'sha1_portable.cc',
      'string16.cc',
      'string_util.cc',
      'string_util_constants.cc',
      'stringprintf.cc',
      'strings/string_number_conversions.cc',
      'strings/string_piece.cc',
      'strings/string_split.cc',
      'strings/sys_string_conversions_mac.mm',
      'strings/sys_string_conversions_mac_unittest.mm',
      'strings/sys_string_conversions_posix.cc',
      'strings/sys_string_conversions_win.cc',
      'strings/utf_offset_string_conversions.cc',
      'strings/utf_string_conversion_utils.cc',
      'strings/utf_string_conversions.cc',
      'supports_user_data.cc',
      'sync_socket_nacl.cc',
      'sync_socket_posix.cc',
      'sync_socket_win.cc',
      'synchronization/cancellation_flag.cc',
      'synchronization/condition_variable_posix.cc',
      'synchronization/condition_variable_win.cc',
      'synchronization/lock.cc',
      'synchronization/lock_impl_posix.cc',
      'synchronization/lock_impl_win.cc',
      'synchronization/waitable_event_posix.cc',
      'synchronization/waitable_event_watcher_posix.cc',
      'synchronization/waitable_event_watcher_win.cc',
      'synchronization/waitable_event_win.cc',
      'sys_info.cc',
      'sys_info_android.cc',
      'sys_info_chromeos.cc',
      'sys_info_freebsd.cc',
      'sys_info_ios.mm',
      'sys_info_linux.cc',
      'sys_info_mac.cc',
      'sys_info_openbsd.cc',
      'sys_info_posix.cc',
      'sys_info_win.cc',
      'system_monitor/system_monitor.cc',
      'task_runner.cc',
      'test/mock_chrome_application_mac.mm',
      'test/test_listener_ios.mm',
      'test/test_support_ios.mm',
      'third_party/dmg_fp/dtoa_wrapper.cc',
      'third_party/dmg_fp/g_fmt.cc',
      'third_party/icu/icu_utf.cc',
      'third_party/nspr/prtime.cc',
      'third_party/xdg_user_dirs/xdg_user_dir_lookup.cc',
      'thread_task_runner_handle.cc',
      'threading/non_thread_safe_impl.cc',
      'threading/platform_thread_android.cc',
      'threading/platform_thread_linux.cc',
      'threading/platform_thread_mac.mm',
      'threading/platform_thread_posix.cc',
      'threading/platform_thread_win.cc',
      'threading/post_task_and_reply_impl.cc',
      'threading/sequenced_worker_pool.cc',
      'threading/simple_thread.cc',
      'threading/thread.cc',
      'threading/thread_checker_impl.cc',
      'threading/thread_collision_warner.cc',
      'threading/thread_id_name_manager.cc',
      'threading/thread_local_posix.cc',
      'threading/thread_local_storage_posix.cc',
      'threading/thread_local_storage_win.cc',
      'threading/thread_local_win.cc',
      'threading/thread_restrictions.cc',
      'threading/watchdog.cc',
      'threading/worker_pool.cc',
      'threading/worker_pool_posix.cc',
      'threading/worker_pool_win.cc',
      'time.cc',
      'time/clock.cc',
      'time/default_clock.cc',
      'time/default_tick_clock.cc',
      'time/tick_clock.cc',
      'time_mac.cc',
      'time_posix.cc',
      'time_win.cc',
      'timer.cc',
      'tracked_objects.cc',
      'tracking_info.cc',
      'value_conversions.cc',
      'values.cc',
      'version.cc',
      'vlog.cc',
      'win/enum_variant.cc',
      'win/event_trace_controller.cc',
      'win/event_trace_provider.cc',
      'win/i18n.cc',
      'win/iat_patch_function.cc',
      'win/iunknown_impl.cc',
      'win/metro.cc',
      'win/object_watcher.cc',
      'win/pe_image.cc',
      'win/registry.cc',
      'win/resource_util.cc',
      'win/sampling_profiler.cc',
      'win/scoped_bstr.cc',
      'win/scoped_handle.cc',
      'win/scoped_process_information.cc',
      'win/scoped_variant.cc',
      'win/shortcut.cc',
      'win/startup_information.cc',
      'win/text_services_message_filter.cc',
      'win/win_util.cc',
      'win/windows_version.cc',
      'win/wrapped_window_proc.cc',
      ]
    return [os.path.normpath(p) for p in files]


  def filter_file_list(all_files, for_types):
    result = all_files[:]
    if 'windows' in for_types:
      for x in ('_posix', '_mac', '_android', '_linux', '_ios', '_solaris',
                '.java', '_gcc', '.mm', 'android\\', '_libevent',
                'chromeos\\', '_freebsd', '_nacl', 'linux_', '_glib', '_gtk',
                'mac\\', 'unix_', 'posix\\', '_aurax11', '_openbsd', '_ozone',
                'android\\', '_kqueue', '_chromeos', 'nix\\', 'xdg_',
                'string16.cc', 'event_recorder_stubs'):
        result = [y for y in result if x not in y]
    if 'linux' in for_types:
      for x in ('_win', '_mac', '_android', '_ios', '_solaris',
                '.java', '.mm', 'android/', '_libevent',
                'chromeos/', 'data/', '_freebsd', '_nacl', 'linux_',
                '_glib', '_gtk', 'mac/', 'file_descriptor',
                '_aurax11', '_openbsd', 'xdg_mime', '_kqueue',
                'symbolize', 'string16.cc', '_chromeos', 'xdg_',
                ):
        result = [y for y in result if x not in y]
    if 'lib' in for_types:
      for x in ('README', 'LICENSE', 'OWNERS', '.h', '.patch', 'unittest',
                'PRESUBMIT', 'DEPS', '.gyp', '.py', '.isolate', '.nc', 'test\\',
                'test/', '.git', '_browsertest.cc', 'base64.cc', # TEMP
                'testing_', 'testing_',
                ):
        result = [y for y in result if x not in y]
    return result

  files = get_file_list()
  return filter_file_list(files, (platform, 'lib'))


def GetRe2FileList():
  files = [
    're2/bitstate.cc',
    're2/compile.cc',
    're2/dfa.cc',
    're2/filtered_re2.cc',
    're2/mimics_pcre.cc',
    're2/nfa.cc',
    're2/onepass.cc',
    're2/parse.cc',
    're2/perl_groups.cc',
    're2/prefilter.cc',
    're2/prefilter_tree.cc',
    're2/prog.cc',
    're2/re2.cc',
    're2/regexp.cc',
    're2/set.cc',
    're2/simplify.cc',
    're2/tostring.cc',
    're2/unicode_casefold.cc',
    're2/unicode_groups.cc',
    'util/arena.cc',
    'util/hash.cc',
    'util/rune.cc',
    'util/stringpiece.cc',
    'util/stringprintf.cc',
    'util/strutil.cc',
    'util/valgrind.cc',
    ]
  return [os.path.normpath(p) for p in files]


def GetFreetypeFileList():
  files = [
    'src/autofit/autofit.c',
    'src/base/ftbase.c',
    'src/base/ftbbox.c',
    'src/base/ftbitmap.c',
    'src/base/ftdebug.c',
    'src/base/ftfstype.c',
    'src/base/ftgasp.c',
    'src/base/ftglyph.c',
    'src/base/ftgxval.c',
    'src/base/ftinit.c',
    'src/base/ftlcdfil.c',
    'src/base/ftmm.c',
    'src/base/ftpatent.c',
    'src/base/ftpfr.c',
    'src/base/ftstroke.c',
    'src/base/ftsynth.c',
    'src/base/ftsystem.c',
    'src/base/fttype1.c',
    'src/base/ftwinfnt.c',
    'src/base/ftxf86.c',
    'src/bdf/bdf.c',
    'src/cache/ftcache.c',
    'src/cff/cff.c',
    'src/cid/type1cid.c',
    'src/gzip/ftgzip.c',
    'src/lzw/ftlzw.c',
    'src/pcf/pcf.c',
    'src/pfr/pfr.c',
    'src/psaux/psaux.c',
    'src/pshinter/pshinter.c',
    'src/psnames/psmodule.c',
    'src/raster/raster.c',
    'src/sfnt/sfnt.c',
    'src/smooth/smooth.c',
    'src/truetype/truetype.c',
    'src/type1/type1.c',
    'src/type42/type42.c',
    'src/winfonts/winfnt.c',
  ]
  return [os.path.normpath(p) for p in files]


def FilterForPlatform(sources_list, platform):
  # TODO(scottmg): Less lame for the next platform.
  if platform == 'linux':
    return filter(lambda x: not x.endswith('_win'), sources_list)
  elif platform == 'windows':
    return filter(lambda x: not x.endswith('_linux'), sources_list)
  elif platform == 'mac':
    return filter(lambda x: not x.endswith('_mac'), sources_list)


def main():
  platform = 'windows'
  if sys.platform.startswith('linux'):
    platform = 'linux'
  elif sys.platform.startswith('darwin'):
    platform = 'mac'

  if platform == 'windows':
    CXX = 'cl'
    CC = 'cl'
    objext = '.obj'
    exeext = '.exe'
  else:
    CXX = os.environ.get('CXX', 'g++')
    CC = os.environ.get('CC', 'gcc')
    objext = '.o'
    exeext = ''

  if not os.path.exists(os.path.join(ninja_dir, 'ninja' + exeext)):
    print "ninja binary doesn't exist, trying to build it..."
    old_dir = os.getcwd()
    os.chdir(ninja_dir)
    if os.system('%s bootstrap.py' % sys.executable) != 0:
      raise SystemExit('Failed to bootstrap ninja')
    os.chdir(old_dir)

  parser = OptionParser()
  parser.add_option('-d', '--debug', action='store_true')
  (options, args) = parser.parse_args()
  if args:
    print 'ERROR: extra unparsed command-line arguments:', args
    sys.exit(1)

  pch_enabled = options.debug and platform == 'windows'


  BUILD_FILENAME = 'build.ninja'
  buildfile = open(BUILD_FILENAME, 'w')
  n = ninja_syntax.Writer(buildfile)

  n.comment('The arguments passed to configure.py, for rerunning it.')
  n.variable('configure_args', ' '.join(sys.argv[1:]))
  n.newline()

  def src(filename):
    return os.path.normpath(os.path.join('sg', filename))
  def base_src(filename):
    return os.path.normpath(os.path.join('third_party', 'base', filename))
  def gwen_src(filename):
    return os.path.normpath(os.path.join('third_party', 'gwen', filename))
  def re2_src(filename):
    return os.path.normpath(os.path.join('third_party', 're2', filename))
  def built(filename):
    return os.path.normpath(os.path.join('$builddir', 'obj', filename))
  pch_name = built('sg.pch')
  pch_implicit = None
  pch_compile = ''
  if pch_enabled:
    pch_implicit = pch_name
    pch_compile = '/Fp' + pch_name + ' /Yusg/global.h'
  def cxx(name, src=src, **kwargs):
    return n.build(built(name + objext), 'cxx', src(name),
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

  if platform == 'windows':
    cflags = ['/nologo',  # Don't print startup banner.
              '/Zi',  # Create pdb with debug info.
              '/W4',  # Highest warning level.
              '/WX',  # Warnings as errors.
              '/FS',  # 2013 PDBs.
              '/wd4530', '/wd4100', '/wd4706', '/wd4245', '/wd4018',
              '/wd4512', '/wd4800', '/wd4702', '/wd4819', '/wd4355',
              '/wd4996', '/wd4481', '/wd4127', '/wd4310', '/wd4244',
              '/wd4701', '/wd4201', '/wd4389', '/wd4722', '/wd4703',
              '/wd4510', '/wd4610', '/wd4189',
              '/GR-',  # Disable RTTI.
              '/DNOMINMAX', '/D_CRT_SECURE_NO_WARNINGS',
              '/DUNICODE', '/D_UNICODE',
              '/D_CRT_RAND_S', '/DWIN32', '/D_WIN32',
              '/D_WIN32_WINNT=0x0601', '/D_VARIADIC_MAX=10',
              '/DDYNAMIC_ANNOTATIONS_ENABLED=0',
              '-I.', '-Ithird_party',
              '-Ithird_party/re2',
              '-Ibuild', '-Ithird_party/freetype/include',
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
    cxxflags = [
        '/Fd$builddir\\sg_cxx_intermediate.pdb',
        ]
    ccflags = [
        '/DFT2_BUILD_LIBRARY', '/wd4146',
        '/Fd$builddir\\sg_cc_intermediate.pdb',
        ]
  else:
    cflags = ['-g', '-Wall', '-Wextra',
              '-Wno-deprecated',
              '-Wno-unused-parameter',
              '-Wno-sign-compare',
              '-fno-rtti',
              '-fno-exceptions',
              '-fvisibility=hidden', '-pipe',
              '-Wno-missing-field-initializers',
              '-I.', '-Ithird_party',
              '-Ithird_party/re2',
              '-Ibuild', '-Ithird_party/freetype/include',
              '-include', 'sg/global.h',
              ]
    if options.debug:
      cflags += ['-D_GLIBCXX_DEBUG', '-D_GLIBCXX_DEBUG_PEDANTIC']
      cflags.remove('-fno-rtti')  # Needed for above pedanticness.
    else:
      cflags += ['-O2', '-DNDEBUG']
    ccflags = []
    cxxflags = []
    ldflags = ['-L$builddir']

  n.newline()

  n.variable('cflags', ' '.join(cflags))
  n.variable('ccflags', ' '.join(ccflags))
  n.variable('cxxflags', ' '.join(cxxflags))
  n.variable('ldflags', ' '.join(ldflags))
  n.newline()

  if platform == 'windows':
    cxx_compiler = 'ninja -t msvc -o $out -- $cxx /showIncludes'
    n.rule('cxx',
      command=('%s $cflags $cxxflags %s '
              '-c $in /Fo$out' % (cxx_compiler, pch_compile)),
      depfile='$out.d',
      description='CXX $out')
    n.newline()
  else:
    n.rule('cxx',
      command='$cxx -MMD -MT $out -MF $out.d $cflags $cxxflags -c $in -o $out',
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
  if pch_enabled:
    compiler = 'ninja -t msvc -o $objname -- $cxx /showIncludes'
    n.rule('cxx_pch',
      command=('%s $cflags $cxxflags /Ycsg/global.h %s '
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
  core_sources = [
               'app_thread.cc',
               #'backend/backend_native_win.cc',
               'backend/debug_core_gdb.cc',
               #'backend/debug_core_native_win.cc',
               'backend/gdb_mi_parse.cc',
               'backend/gdb_to_generic_converter.cc',
               #'backend/process_native_win.cc',
               'backend/subprocess_win.cc',
               'cpp_lexer.cc',
               'debug_presenter.cc',
               'debug_presenter_display.cc',
               'display_util.cc',
               'lexer.cc',
               'lexer_state.cc',
               'locals_view.cc',
               'main_loop.cc',
               'render/renderer.cc',
               'render/scoped_render_offset.cc',
               'render/texture.cc',
               'source_files.cc',
               'source_view.cc',
               'status_bar.cc',
               'stack_view.cc',
               'ui/dockable.cc',
               'ui/docking_resizer.cc',
               'ui/docking_split_container.cc',
               'ui/docking_tool_window.cc',
               'ui/docking_workspace.cc',
               'ui/focus.cc',
               'ui/scroll_helper.cc',
               'ui/scrolling_output_view.cc',
               'ui/skin.cc',
               'ui/tool_window_dragger.cc',
               'ui/tree_view_helper.cc',
               'workspace.cc',
              ]
  for name in FilterForPlatform(core_sources, platform):
    sg_objs += cxx(name)
  n.newline() 

  re2_objs = []
  n.comment('RE2.')
  for base in GetRe2FileList():
    re2_objs += cxx(base, src=re2_src)
  n.newline()

  n.comment('Chromium base.')
  crfiles = GetChromiumBaseFileList('third_party/base', platform)
  base_objs = []
  for name in crfiles:
    base_objs += cxx(name, src=base_src)
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

  app_objs = sg_objs + base_objs + re2_objs + ft2_objs + pch_objs

  n.comment('Main executable is library plus main() and some startup goop.')
  main_objs = []
  base_sources = ['application.cc',
               'render/application_window_win.cc',
               'render/gpu_win.cc',
               'render/direct2d_win.cc',
               'main_win.cc',
               ]
  for base in FilterForPlatform(base_sources, platform):
    main_objs += cxx(base)
  if platform == 'windows':
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
                       implicit=pch_implicit,
                       variables=[('cflags', gtest_cflags)])
  test_objs += n.build(built('gtest_main' + objext), 'cxx',
                       inputs='sg/main_test.cc',
                       implicit=pch_implicit,
                       variables=[('cflags', gtest_cflags)])

  test_cflags = cflags + ['-DGTEST_HAS_RTTI=0',
                          '-I%s' % os.path.join(path, 'include')]

  for name in [
               'lexer_test.cc',
               #'backend/debug_core_native_win_test.cc',
               'backend/debug_core_gdb_test.cc',
               'backend/gdb_mi_parse_test.cc',
               'backend/subprocess_test.cc',
               'ui/docking_test.cc',
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
  reader_writer_objs += cxx('backend/reader_writer_test.cc') + pch_objs
  reader_writer_test = n.build(binary('reader_writer_test'), 'link',
                               inputs=reader_writer_objs,
                               implicit=pch_implicit,
                               order_only=sg_binary,
                               variables=[('ldflags', test_ldflags)])
  all_targets += reader_writer_test
  n.newline()

  n.comment('Regenerate build files if build script changes.')
  prefix = ''
  if platform == 'windows':
    prefix = 'cmd /c '
  n.rule('configure',
          command=prefix + 'python build/configure.py $configure_args',
          generator=True)
  n.build('build.ninja', 'configure',
          implicit=[os.path.normpath('build/configure.py'),
                    os.path.normpath('%s/misc/ninja_syntax.py' % ninja_dir)])
  n.newline()


  n.build('all', 'phony', all_targets)

  print 'wrote %s.' % BUILD_FILENAME

if __name__ == '__main__':
  main()
