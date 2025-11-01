# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-src")
  file(MAKE_DIRECTORY "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-src")
endif()
file(MAKE_DIRECTORY
  "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-build"
  "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-subbuild/raylib-populate-prefix"
  "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-subbuild/raylib-populate-prefix/tmp"
  "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-subbuild/raylib-populate-prefix/src/raylib-populate-stamp"
  "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-subbuild/raylib-populate-prefix/src"
  "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-subbuild/raylib-populate-prefix/src/raylib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-subbuild/raylib-populate-prefix/src/raylib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/alpharone/tec3/oop/origine/G-CPP-500-PAR-5-1-rtype-4/_deps/raylib-subbuild/raylib-populate-prefix/src/raylib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
