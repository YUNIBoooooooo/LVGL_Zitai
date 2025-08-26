# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/ESP/v5.2.3/esp-idf/components/bootloader/subproject"
  "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader"
  "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader-prefix"
  "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader-prefix/tmp"
  "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader-prefix/src/bootloader-stamp"
  "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader-prefix/src"
  "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/360MoveData/Users/19736/Desktop/ESP32S3/jialichuang/my/LVGL_Zitai/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
