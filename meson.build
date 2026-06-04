project(
    'project01', 
    'cpp'
)


cpp = meson.get_compiler('cpp')

cmd = find_program('cmd', required: true)
vulkan_sdk_path = run_command(cmd, '/c', 'echo %VULKAN_SDK%', check: true).stdout().strip()

vulkan_inc = include_directories(vulkan_sdk_path / 'Include')
vulkan_lib_dir = vulkan_sdk_path / 'Lib'

# volk
volk_lib = cpp.find_library('volk', dirs: [vulkan_lib_dir], required: true)
volk = declare_dependency(dependencies: volk_lib, include_directories: vulkan_inc)

# shaderc
shaderc_lib = cpp.find_library('shaderc_combined', dirs: [vulkan_lib_dir], required: true)
shaderc = declare_dependency(dependencies: shaderc_lib, include_directories: vulkan_inc)

# glm
glm = declare_dependency(include_directories: vulkan_inc)

# sdl3 and vulkan
sdl3 = dependency('sdl3')
vulkan = dependency('vulkan')

executable(
    'app',
    'main.cpp',
    dependencies: 
    [
        sdl3,
        vulkan,
        volk,
        shaderc,
        glm
    ],
    win_subsystem: 'console'
)