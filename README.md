
<h1 align="center">A simple OpenGL rendering library in C</h1> 

I made this library mostly for learning and to use in other projects i wanna make\
Of course if you for whatever reason want to use it, go ahead lol


## Dependencies

It's advised to install these with your systems package manager

- [openGL V4.5 or newer](https://www.opengl.org/)
- [glad](https://github.com/Dav1dde/glad)
- [glfw](https://www.glfw.org/)
- [cglm](https://github.com/recp/cglm)

- [cmake (for system wide installation)](https://cmake.org/)


## Instalation

```
git clone https://github.com/ashleythatcatgirl/C-rendering-library
cd C-rendering-library
```

Manual
>It's necessary to copy cglm and stb_image from the include folder into your systems library directory\
`/usr/local/include/` for linux systems

```
cp -rp ./include/cglm/ /usr/local/include/
cp -rp ./include/stb_image.h /usr/local/include/stb_image/
cmake -B build
cmake --build build
sudo cmake --install build
```

Build script
> (make sure to run in the root project directory)
```
bash build.sh
```


## Usage

After installation include header with:
`
#include <GLib/glib.h>
`

And compile your program with:
`
gcc -lGLib
`


## Documentation

TBA
