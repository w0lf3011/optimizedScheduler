cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_task_manager)

# Initialiser le SDK Pico
pico_sdk_init()

# Ajouter les fichiers source (ajustez avec les noms des fichiers pour chaque itération)
add_executable(pico_task_manager
    main.c
    task_manager.c
    energy_manager.c
    hardware_abstraction.c
    error_handling.c
)

# Inclure les répertoires d'en-têtes
target_include_directories(pico_task_manager PRIVATE ${CMAKE_SOURCE_DIR})

# Lier les bibliothèques du SDK Pico
target_link_libraries(pico_task_manager pico_stdlib)

# Créer un fichier .uf2 pour flasher le Pico
pico_add_extra_outputs(pico_task_manager)
