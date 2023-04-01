audio_lib = library('audio', [
  'audio.c',
], dependencies: [
  jack2_dep,
  m_dep
])

audio_dep = declare_dependency(
  link_with: audio_lib,
  include_directories: '..'
)
