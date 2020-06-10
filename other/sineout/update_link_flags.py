# Custom settings, as referred to as "extra_script" in platformio.ini
#
# See http://docs.platformio.org/en/latest/projectconf.html#extra-script

from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

env.Append(
    LINKFLAGS=[
        '-std=gnu++2a',
        '-O0'
    ]
)