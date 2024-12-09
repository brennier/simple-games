#!/bin/bash
target="$1"
project="$2"
build="./build"

if [ -z $project ]; then
    echo "Please specify a project" >&2
    exit 1
elif [ -z $target ]; then
    echo "Please specify a target" >&2
    exit 1
fi

rm -rv "$build"/*

if [ "$target" = "-linux" ]; then
    gcc -Wall -std=c99 -I ./libraries/linux -o "$build/$project" "$project/main.c" -L ./libraries/linux -lraylib -lGL -lm
    # gcc -std=c99 -I./raylib//include -o main main.c -L./raylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
elif [ "$target" = "-windows" ]; then
    echo "Target Windows is not implemented yet" >&2
    exit 1
elif [ "$target" = "-web" ]; then
    source "./libraries/web/emsdk/emsdk_env.sh"
    emcc -o "$build/main.html" "$project/main.c" -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -O3 -I./libraries/linux/ -L./libraries/linux/ -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 ./libraries/web/libraylib.a -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' -s EXPORTED_RUNTIME_METHODS=ccall
    rm "$build/main.html"
    cp libraries/web/index.html "$build"
    cp "./$project/main.c" "$build"
else
    echo "Target $target is invalid" >&2
    exit 1
fi
