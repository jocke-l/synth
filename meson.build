project('synth', ['c'],
  default_options: [
    'c_std=gnu2x',
    'warning_level=3',
    'default_library=static',
    'buildtype=release',
  ])

sdl_dep = dependency('SDL2')
jack2_dep = dependency('jack')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: true)

# Ensure Nuklear is downloaded
run_command('git', [
  'submodule',
  'update',
  '--init',
  '--recursive'
], check: true)

subdir('src')
