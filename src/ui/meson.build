ui_lib = library('ui', [
  'ui.c',
], dependencies: [
  nuklear_dep,
  sdl_dep,
])

ui_dep = declare_dependency(
  link_with: ui_lib,
  include_directories: '..'
  )
