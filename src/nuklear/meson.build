# Ensure Nuklear is downloaded
run_command('git', [
  'submodule',
  'update',
  '--init',
  '--recursive',
  'Nuklear'
], check: true)

nuklear_lib = library('nuklear', [
  'nuklear.c',
  'nuklear_sdl_renderer.c',
], dependencies: [
  sdl_dep,
])

nuklear_dep = declare_dependency(
  link_with: nuklear_lib,
  include_directories: '..'
  )
