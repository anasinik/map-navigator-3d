# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-src"
  "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-build"
  "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-subbuild/glew-populate-prefix"
  "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-subbuild/glew-populate-prefix/tmp"
  "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp"
  "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-subbuild/glew-populate-prefix/src"
  "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Desktop/MapNavigator3D/out/build/x64-Debug/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
