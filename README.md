# C++ GUI Interface for Mounting Volumes in MIRACL using FULLTICK

## Compiling

**Development**

```bash
g++ -std=c++23 -ggdb -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror \ main.cpp -o main \ -I/usr/local/include -I/usr/local/include/FL/images -I/usr/local/include/jpeg -I/usr/local/include/yaml-cpp \ -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_THREAD_SAFE -D_REENTRANT \ -L/usr/local/lib -L/usr/local/lib/yaml-cpp \ -lfltk_images -lfltk_jpeg /usr/lib/libpng.so /usr/lib/libz.so -lfltk_gl -lGLU -lGL -lfltk -lm -lX11 \ -lXext -lpthread -lXinerama -lXfixes -lXcursor -lXft -lXrender -lfontconfig -ldl -lyaml-cpp && ./main
```

**Production**

makefile
