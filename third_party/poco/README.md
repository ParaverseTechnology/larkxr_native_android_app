# Poco C++ lib

Version 1.9.4

link: [Poco](https://github.com/pocoproject/poco)

```cmd
mkdir release & cd release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
mkdir debug & cd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
```

## build Android

ubuntu 18.04lts

```cmd
./configure --config=Android --no-samples --no-tests --cflags="-std=c++11 -std=c99"  --static  --shared
make -s -j4
make -s -j4 ANDROID_ABI=armeabi-v7a

zip -r lib.zip ./lib
```

## 1.11 android build

```powershell
<path_to_android_sdk>/cmake/<cmake_version>/bin/cmake -H<path_to_poco_sourcen> -B/tmp/poco-build -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=<path_to_android_sdk>/cmake/<cmake_version>/bin/ninja -DCMAKE_TOOLCHAIN_FILE=<path_to_android_sdk>/ndk-bundle/build/cmake/android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=armeabi-v7a
```

## 1

armeabi-v7a release

```powershell
G:\Android\sdk\cmake\3.10.2.4988404\bin\cmake -HD:\libs\poco-poco-1.10.1-release -BD:\libs\poco-poco-1.10.1-release\poco-build -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=G:\Android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=G:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=armeabi-v7a
```

armeabi-v7a debug

```powershell
G:\Android\sdk\cmake\3.10.2.4988404\bin\cmake -HD:\libs\poco-poco-1.10.1-release -BD:\libs\poco-poco-1.10.1-release\poco-build -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=G:\Android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=G:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=armeabi-v7a
```

arm64-v8a release

```powershell
G:\Android\sdk\cmake\3.10.2.4988404\bin\cmake -HD:\libs\poco-poco-1.10.1-release -BD:\libs\poco-poco-1.10.1-release\poco-build -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=G:\Android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=G:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=arm64-v8a
```

arm64-v8a debug

```cmd
G:\Android\sdk\cmake\3.10.2.4988404\bin\cmake -HD:\libs\poco-poco-1.10.1-release -BD:\libs\poco-poco-1.10.1-release\poco-build -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=G:\Android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=G:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=arm64-v8a
```

```cmd
cd D:\libs\poco-poco-1.10.1-release\poco-build
G:\Android\sdk\cmake\3.10.2.4988404\bin\ninja
```

## 2

-DCMAKE_EXE_LINKER_FLAGS="-s -Wl,-O3 -Wl,--hash-style=both -Wl,-Map,mapfile"

-DCMAKE_CXX_VISIBILITY_PRESET=hidden

armeabi-v7a release

add openssl

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DOPENSSL_SSL_LIBRARY=E:\work\libs\openssl_pre_build\lib\armeabi-v7a\lib -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\openssl_pre_build\lib\armeabi-v7a\lib -DOPENSSL_INCLUDE_DIR=E:\work\libs\openssl_pre_build\include -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=armeabi-v7a
```

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=armeabi-v7a
```

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=armeabi-v7a
```

armeabi-v7a debug

add openssl

-DOPENSSL_SSL_LIBRARY
-DOPENSSL_CRYPTO_LIBRARY
-DANDROID_NATIVE_API_LEVEL=16
-DANDROID_ABI=armeabi-v7a

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=armeabi-v7a
```

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=armeabi-v7a
```

arm64-v8a release

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-arm64 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=arm64-v8a
```

arm64-v8a debug

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-arm64 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=arm64-v8a
```

x86_64 release

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86_64 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86_64
```

x86_64 debug

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86_64 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86_64
```

x86 release

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86
```

x86 debug

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86
```

```cmd
cd E:\work\libs\poco-1.10.1-all\poco-build
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

-DANDROID_NATIVE_API_LEVEL=16 min sdk 设置为16，修复“dlopen failed: empty/missing DT_HASH”错误

[Android NDK UnsatisfiedLinkError: “dlopen failed: empty/missing DT_HASH”](https://stackoverflow.com/questions/28638809/android-ndk-unsatisfiedlinkerror-dlopen-failed-empty-missing-dt-hash)


### build static libs


```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=armeabi-v7a -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build-armv7
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=armeabi-v7a -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build-armv7
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

arm64-v8a release

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-arm64 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=arm64-v8a -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build-arm64
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

arm64-v8a debug

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-arm64 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=arm64-v8a -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build-arm64
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

x86_64 release

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86_64 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86_64 -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build_x86_64
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

x86_64 debug

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86_64 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86_64 -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build_x86_64
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

x86 release

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86 -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build_x86
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

x86 debug

```cmd
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=16 -DANDROID_ABI=x86 -DBUILD_SHARED_LIBS=off -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include
cd E:\work\libs\poco-1.10.1-all\poco-build_x86
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

### build test shared libs


```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=armeabi-v7a -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DANDROID_STL="c++_shared" -DANDROID_CPP_FEATURES="exceptions rtti" -DENABLE_TESTS=OFF ; cd E:\work\libs\poco-1.10.1-all\poco-build-armv7 ; E:\android\sdk\cmake\3.10.2.4988404\bin\ninja ; E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-armv7 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=armeabi-v7a -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\armeabi-v7a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DANDROID_STL="c++_shared" -DANDROID_CPP_FEATURES="exceptions rtti" -DENABLE_TESTS=OFF ; cd E:\work\libs\poco-1.10.1-all\poco-build-armv7 ; E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

arm64-v8a release

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-arm64 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\Android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=arm64-v8a -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\arm64-v8a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\arm64-v8a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DANDROID_STL="c++_shared" -DANDROID_CPP_FEATURES="exceptions rtti" -DENABLE_TESTS=OFF ; cd E:\work\libs\poco-1.10.1-all\poco-build-arm64 ; E:\android\sdk\cmake\3.10.2.4988404\bin\ninja ; E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build-arm64 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=arm64-v8a -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\arm64-v8a\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\arm64-v8a\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DANDROID_STL="c++_shared" -DANDROID_CPP_FEATURES="exceptions rtti" -DENABLE_TESTS=OFF ; cd E:\work\libs\poco-1.10.1-all\poco-build-arm64 ; E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

x86_64 release

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86_64 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=x86_64 -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86_64\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86_64\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DANDROID_STL="c++_shared" -DANDROID_CPP_FEATURES="exceptions rtti" -DENABLE_TESTS=OFF ; cd E:\work\libs\poco-1.10.1-all\poco-build_x86_64 ; E:\android\sdk\cmake\3.10.2.4988404\bin\ninja ; E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86_64 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=x86_64 -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86_64\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86_64\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DANDROID_STL="c++_shared" -DANDROID_CPP_FEATURES="exceptions rtti" -DENABLE_TESTS=OFF ; cd E:\work\libs\poco-1.10.1-all\poco-build_x86_64 ; E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```

x86 release

```powershell
E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86 -G'Ninja' -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=x86 -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include -DANDROID_STL="c++_static" -DANDROID_CPP_FEATURES="exceptions rtti" -DENABLE_TESTS=OFF ; cd E:\work\libs\poco-1.10.1-all\poco-build_x86 ; E:\android\sdk\cmake\3.10.2.4988404\bin\ninja ; E:\android\sdk\cmake\3.10.2.4988404\bin\cmake -HE:\work\libs\poco-1.10.1-all -BE:\work\libs\poco-1.10.1-all\poco-build_x86 -G'Ninja' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=E:\android\sdk\cmake\3.10.2.4988404\bin\ninja -DCMAKE_TOOLCHAIN_FILE=E:\android\sdk\ndk-bundle\build\cmake\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_ABI=x86 -DBUILD_SHARED_LIBS=on -DOPENSSL_SSL_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86\lib\libssl.a -DOPENSSL_CRYPTO_LIBRARY=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\x86\lib\libcrypto.a -DOPENSSL_INCLUDE_DIR=E:\work\libs\OpenSSL-for-Android-Prebuilt\openssl-1.1.1d-clang\include ; cd E:\work\libs\poco-1.10.1-all\poco-build_x86 ;
E:\android\sdk\cmake\3.10.2.4988404\bin\ninja
```
