@echo off
cls

REM *NOTE* Change these based on 
SET ASSIMP_DIR=..\
SET OUTPUT_DIR=assimp-build-armeabi-v7a
REM SET OUTPUT_DIR=assimp-build-arm64-v8a
SET ANDROID_PATH=%ANDROID_HOME%
SET NDK_PATH=%ANDROID_PATH%\ndk-bundle
SET NDK_TOOLCHAIN=..\android_tool_chain
SET CMAKE_TOOLCHAIN=%NDK_PATH%\build\cmake\android.toolchain.cmake
SET CMAKE_PATH=%ANDROID_PATH%\cmake\3.6.4111459

REM *NOTE* Careful if you don't want rm -rf, I use it for testing purposes.
rm -rf %OUTPUT_DIR%
mkdir %OUTPUT_DIR%

REM pushd doesn't seem to work ):<
cd %OUTPUT_DIR%

if not defined ORIGPATH set ORIGPATH=%PATH%
SET PATH=%CMAKE_PATH%\bin;%ANDROID_PATH%\tools;%ANDROID_PATH%\platform-tools;%ORIGPATH%

cmake ^
      -GNinja ^
      -DCMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN% ^
      -DASSIMP_ANDROID_JNIIOSYSTEM=ON ^
      -DANDROID_NDK=%NDK_PATH% ^
      -DCMAKE_MAKE_PROGRAM=%CMAKE_PATH%\bin\ninja.exe ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DANDROID_ABI="armeabi-v7a" ^
      -DANDROID_NATIVE_API_LEVEL=23 ^
      -DANDROID_FORCE_ARM_BUILD=TRUE ^
      -DCMAKE_INSTALL_PREFIX=install ^
      -DANDROID_STL=c++_shared ^
      -DCMAKE_CXX_FLAGS="-frtti -fexceptions -std=c++11 -Wno-c++11-narrowing"^
      -DANDROID_TOOLCHAIN=clang ^

      -DASSIMP_BUILD_TESTS=OFF ^
	  -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF ^
	  -DASSIMP_BUILD_OBJ_IMPORTER=ON ^
	  -DASSIMP_BUILD_FBX_IMPORTER=ON ^
	  -DASSIMP_BUILD_GLTF_IMPORTER=ON ^
	  -DASSIMP_NO_EXPORT=ON ^
      ..\..\

cmake --build .

cd ..

pause