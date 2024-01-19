#!python
import os

# Reads variables from an optional file.
customs = ['../custom.py']
opts = Variables(customs, ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = Environment(ENV=os.environ)

# Compilation options
opts.AddVariables(
    EnumVariable('target', "Compilation target", 'release', ['d', 'debug', 'r', 'release']),
    EnumVariable('platform', "Compilation platform", 'windows', ['windows', 'x11', 'linux']),
    PathVariable('target_path', 'The path where the lib is installed', 'demo/addons/godot-openvr/bin/'),
    # TODO: Including the `lib` prefix here causes the ar archive to have it doubled, but need this for DLL.
    PathVariable('target_name', 'The library name', 'libgodot_openvr', PathVariable.PathAccept),
    BoolVariable('use_mingw', "Use the Mingw compiler, even if MSVC installed", 'no'),
    BoolVariable('use_llvm', "Use the LLVM compiler", 'no'),
    EnumVariable('bits', "CPU architecture", '64', ['32', '64']),
)

# Options for locating the OpenVR library
opts.AddVariables(
    BoolVariable('builtin_openvr', "Use OpenVR library from submodule instead of system-provided", 'yes'),
    PathVariable('openvr_path', 'The path where the OpenVR repo is located, if builtin_openvr is used', 'openvr/'),
)

# Other needed paths
godot_headers_path = "godot-cpp/gdextension/"
godot_cpp_path = "godot-cpp/"
godot_cpp_library = "libgodot-cpp"

# Updates the environment with the option variables.
opts.Update(env)

if env['bits'] == '64':
    arch = 'x86_64'
elif env['bits'] == '32':
    arch = 'i686'

# Check some environment settings
if env['use_llvm']:
    env['CXX'] = 'clang++'

# platform dir for openvr libraries
platform_dir = ''

# openvr support dll
openvr_dll_source = '';
openvr_dll_target = '';

# Setup everything for our platform
if env['platform'] == 'windows':
    env['target_path'] += 'win' + env['bits'] + '/'
    godot_cpp_library += '.windows'
    platform_dir = 'win' + str(env['bits'])

    if env['use_llvm']:
        # untested
        env.Append(CPPDEFINES=["WIN32", "_WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS"])
        env.Append(CCFLAGS=["-W3", "-GR"])
        env.Append(CXXFLAGS=["-std:c++17"])
        if env['target'] in ('debug', 'd'):
            env.Append(CCFLAGS = ['-fPIC', '-g3','-Og'])
        else:
            env.Append(CCFLAGS = ['-fPIC', '-g','-O3'])

    elif env['use_mingw']:

        env['CXX'] = f'{arch}-w64-mingw32-g++'
        env['AR'] = f'{arch}-w64-mingw32-ar'
        env['RANLIB'] = f'{arch}-w64-mingw32-ranlib'
        env['LINK'] = f'{arch}-w64-mingw32-g++'

        env.Append(CCFLAGS=['-g', '-O3', '-std=c++17', '-Wwrite-strings'])
        env.Append(LINKFLAGS=['-Wl,--no-undefined', '-static-libgcc', '-static-libstdc++'])

    else:
        # Preserve the environment so that scons can be executed from within Visual Studio and find the correct
        # toolchain.
        env.Append(ENV = os.environ)

        env.Append(CPPDEFINES=["WIN32", "_WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "TYPED_METHOD_BIND"])
        env.Append(CCFLAGS=["-W3", "-GR"])
        env.Append(CXXFLAGS=["-std:c++17"])
        if env['target'] in ('debug', 'd'):
            env.Append(CPPDEFINES=["_DEBUG"])
            env.Append(CCFLAGS = ['-EHsc', '-MDd', '-ZI', '-FS'])
            env.Append(LINKFLAGS = ['-DEBUG'])
        else:
            env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '-MD'])

    openvr_dll_target = env['target_path'] + "openvr_api.dll"
    openvr_dll_source = env['openvr_path'] + "bin/win" + str(env['bits']) + "/openvr_api.dll"

elif env['platform'] in ('x11', 'linux'):
    env['target_path'] += 'x11/'
    godot_cpp_library += '.linux'
    platform_dir = 'linux' + str(env['bits'])
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-fPIC', '-g3','-Og', '-std=c++17'])
    else:
        env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])
    env.Append(CXXFLAGS='-std=c++0x')
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])

    openvr_dll_target = env['target_path'] + "libopenvr_api.so"
    openvr_dll_source = env['openvr_path'] + "bin/linux" + str(env['bits']) + "/libopenvr_api.so"

# Complete godot-cpp library path
if env['target'] in ('debug', 'd'):
    godot_cpp_library += '.template_debug'
    env.Append(CPPDEFINES=["DEBUG_ENABLED", "DEBUG_METHODS_ENABLED"])
else:
    godot_cpp_library += '.template_release'

godot_cpp_library += f'.{arch}'

# Update our include search path
env.Append(CPPPATH=[
    '.',
    'src/',
    'src/open_vr/',
    godot_headers_path,
    godot_cpp_path + 'include/',
    godot_cpp_path + 'gen/include/',
    env['openvr_path'] + 'headers/'])

# Add our godot-cpp library
env.Append(LIBPATH=[godot_cpp_path + 'bin/'])
env.Append(LIBS=[godot_cpp_library])

# Add openvr_api, either from the system or the provided path
if not env['builtin_openvr']:
    # Discover the system libopenvr if we've been asked to use it.
    # `pkg-config` is a Linux-ism, but also may exist in a Windows build environment if e.g. msys2 is in use.
    # If the user gave the option, assume they set up the system for it to work.
    # Because we won't have the library as a DLL to bundle, build statically. Need to define OPENVR_BUILD_STATIC,
    # see https://github.com/ValveSoftware/openvr/issues/1457
    env.ParseConfig('pkg-config openvr --cflags --libs')
    env.Append(LINKFLAGS=['-static'])
    env.Append(CPPDEFINES=['OPENVR_BUILD_STATIC'])
    openvr_dll_target = ''

elif (os.name == "nt" and os.getenv("VCINSTALLDIR")):
    env.Append(LIBPATH=[env['openvr_path'] + 'lib/' + platform_dir])
    env.Append(LINKFLAGS=['openvr_api.lib'])
else:
    env.Append(LIBPATH=[env['openvr_path'] + 'lib/' + platform_dir])
    env.Append(LIBS=['openvr_api'])

# Add our sources
sources = Glob('src/*.c')
sources += Glob('src/*.cpp')
sources += Glob('src/*/*.c')
sources += Glob('src/*/*.cpp')

if env['target'] in ('debug', 'd'):
    env['target_name'] += "_debug"
else:
    env['target_name'] += "_release"

# Build our library
library = env.SharedLibrary(target=env['target_path'] + env['target_name'], source=sources)

if openvr_dll_target != '':
    env.AddPostAction(library, Copy(
        openvr_dll_target,
        openvr_dll_source
    ))

Default(library)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))
