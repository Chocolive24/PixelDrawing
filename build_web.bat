call C:\Users\olivi\source\repos\Tools\emsdk\emsdk_env.bat

mkdir bin

call C:\Users\olivi\source\repos\Tools\emsdk\upstream\emscripten\em++ code\RunnerGame.cpp ^
    -o bin\game.js ^
    -std=c++20 ^
    -Wall -Wextra ^
    -I libs\include ^
    libs\wasm\libminifb.a ^
    -fcolor-diagnostics ^
    -fansi-escape-codes ^
    -sEXPORT_NAME=my_game ^
    -sASYNCIFY ^
    --preload-file assets ^
    -sSTRICT=1 ^
    -sENVIRONMENT=web ^
    -sMODULARIZE=1 ^
    -sALLOW_MEMORY_GROWTH=1 ^
    -sALLOW_TABLE_GROWTH ^
    -sMALLOC=emmalloc ^
    -sEXPORT_ALL=1 ^
    -sEXPORTED_FUNCTIONS=["_malloc","_free","_main"] ^
    -sEXPORTED_RUNTIME_METHODS=ccall,cwrap ^
    -sASYNCIFY ^
    --no-entry 