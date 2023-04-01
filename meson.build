project('synth', ['c'],
  default_options: [
    'c_std=gnu2x',
    'warning_level=3',
    'default_library=static',
    'buildtype=release',
  ])

if not meson.is_subproject()
  add_global_arguments([
    '-Werror=return-type'
  ], language: 'c')
endif

sdl_dep = dependency('SDL2')
jack2_dep = dependency('jack')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: true)

subdir('src')

run_target('run', command: standalone_exe)
