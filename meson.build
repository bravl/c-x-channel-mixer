#**********************************************************************************************
# Project definition
#**********************************************************************************************
project('X Channel mixer', 'c', 
        version : '1.0.0',
        meson_version : '>= 0.44.0',
        default_options : ['c_std=gnu99'])

#**********************************************************************************************
# Project Global arguments
#**********************************************************************************************

c_mixer_divide_src = [
  files(
    'snd-wave.c',
    'mixer2.c',
  )
]

c_mixer_src = [
  files(
    'snd-wave.c',
    'mixer.c',
  )
]

c_mixer_interpol_src = [
  files(
    'snd-wave.c',
    'mixer-linear-interpol.c',
  )
]

executable('mixer-divide', c_mixer_divide_src)
executable('mixer-lut', c_mixer_src)
executable('mixer-lut-interpol', c_mixer_interpol_src)
