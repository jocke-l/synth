subdir('nuklear')
subdir('ui')
subdir('audio')
subdir('synth')

standalone_exe = executable('standalone', [
  'standalone.c',
], dependencies: [
  audio_dep,
  nuklear_dep,
  synth_dep,
  ui_dep,
])
