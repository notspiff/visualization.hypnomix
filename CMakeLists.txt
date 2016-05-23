project(visualization.hypnomix)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})


macro(build_module module sources)
  add_library(${module} SHARED ${${sources}})
  target_link_libraries(${module} hypnocommon)
  set_target_properties(${module} PROPERTIES PREFIX "")
  if(KODI_BUILD_DIR)
    add_custom_command(TARGET ${module} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                               $<TARGET_LINKER_FILE:${module}>
                               ${KODI_BUILD_DIR}/addons/visualization.hypnomix/resources)
  endif()
  install(TARGETS ${module}
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/kodi/addons/visualization.hypnomix/resources)
endmacro()


find_package(Kodi REQUIRED)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
include_directories(${OpenGL_INCLUDE_DIR}
                    ${GLEW_INCLUDE_DIRS}
                    ${PNG_INCLUDE_DIRS}
                    ${JPEG_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}
                    ${FREETYPE_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src)

set(HYPNO_SOURCES src/addon.cpp
                  src/dir.c
                  src/fft.c
                  src/hypnomix.c
                  src/module.c)

add_definitions(-DGL_EXT_PROTOTYPES -DX11)

if(NOT WIN32)
  add_options(ALL_LANGUAGES ALL_BUILDS -fPIC)
endif()

add_library(hypnocommon STATIC src/mathematics.c
                               src/opengl.c
                               src/presets.c
                               src/shader.c
                               src/transform.c)


set(tuto_srcs src/modules/mod_tuto.c
              src/modules/skybox.c
              src/modules/pool.c)
build_module(mod_tuto tuto_srcs)

set(7rain_srcs src/modules/mod_7rain.c
               src/modules/pool.c)
build_module(mod_7rain 7rain_srcs)

set(flag_srcs src/modules/mod_flag.c
              src/modules/font.c
              src/modules/texture.c)
build_module(mod_flag flag_srcs)

set(geo_srcs src/modules/mod_geography.c
             src/modules/skybox.c
             src/modules/texture.c)
build_module(mod_geography geo_srcs)

set(kinetics_srcs src/modules/mod_kinetics.c
                  src/modules/obj.c)
build_module(mod_kinetics kinetics_srcs)

set(strato_srcs src/modules/mod_stratoscape.c)
build_module(mod_stratoscape strato_srcs)

set(unknown_srcs src/modules/mod_unknown_pleasure.c
                 src/modules/skybox.c)
build_module(mod_unknown_pleasure unknown_srcs)

set(parasite_srcs src/modules/mod_parasite.c)
build_module(mod_parasite parasite_srcs)

set(waste_srcs src/modules/mod_waste.c
               src/modules/pool.c
               src/modules/texture.c
               src/modules/particles.c)
build_module(mod_waste waste_srcs)

set(DEPLIBS ${OPENGL_LIBRARIES}
            ${GLEW_LIBRARIES}
            ${JPEG_LIBRARIES}
            ${FREETYPE_LIBRARIES}
            ${PNG_LIBRARIES}
            hypnocommon)


build_addon(visualization.hypnomix HYPNO DEPLIBS)

include(CPack)

