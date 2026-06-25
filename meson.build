project(
    'Eve', 
    'cpp',
    default_options: ['cpp_std=c++20', 'b_vscrt=md']
)

cpp = meson.get_compiler('cpp')

# Flag CRT corretto per clang++ in GCC-driver mode su Windows
if cpp.get_id() == 'clang' and host_machine.system() == 'windows'
    add_project_arguments('-fms-runtime-lib=dll', language: 'cpp')
endif


cmd = find_program('cmd', required: true)
vulkan_sdk_path = run_command(cmd, '/c', 'echo %VULKAN_SDK%', check: true).stdout().strip()

vulkan_inc = include_directories(vulkan_sdk_path / 'Include')
vulkan_lib_dir = vulkan_sdk_path / 'Lib'

# volk
volk_lib = cpp.find_library('volk', dirs: [vulkan_lib_dir], required: true)
shaderc_lib = cpp.find_library('shaderc_combined', dirs: [vulkan_lib_dir], required: true)


# glm, sdl3 and vulkan
sdl3 = dependency('sdl3')
vulkan = dependency('vulkan', include_type: 'system')
glm = dependency('glm')
volk = declare_dependency(dependencies: volk_lib, include_directories: vulkan_inc)
shaderc = declare_dependency(dependencies: shaderc_lib, include_directories: vulkan_inc)

engine_source = []

subdir('source')

include_dir_source = include_directories('source')
include_dir = include_directories('include')
third_parties = include_directories('third-parties')

executable(
    'app',
    [
        'main.cpp',
        engine_source
    ],
    include_directories: [include_dir_source, include_dir , third_parties],
    dependencies: 
    [
        sdl3,
        vulkan,
        volk,
        shaderc,
        glm
    ],
    win_subsystem: 'console',
    cpp_args: [
        '-D_DISABLE_STRING_ANNOTATION',
        '-D_DISABLE_VECTOR_ANNOTATION',
        '-D_DISABLE_DEQUE_ANNOTATION'
    ],
    link_args: [
        '-fms-runtime-lib=dll',
        '-shared-libsan', # Suggerisce a Clang di preferire il runtime ASan condiviso
        '-Wl,/NODEFAULTLIB:clang_rt.asan_static_runtime_thunk-x86_64.lib', # Esclude il thunk statico buggato
        '-Wl,clang_rt.asan_dynamic_runtime_thunk-x86_64.lib',             # Forza il thunk dinamico corretto
        '-Wl,clang_rt.asan_dynamic-x86_64.lib'
    ],
    override_options: ['b_sanitize=address']
)