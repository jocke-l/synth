synth_lib = library('synth', [
  'synth.c',
  'ui.c',
], dependencies: [
  audio_dep,
  nuklear_dep,
  ui_dep,
])

synth_dep = declare_dependency(
  link_with: synth_lib,
  include_directories: '..'
)
