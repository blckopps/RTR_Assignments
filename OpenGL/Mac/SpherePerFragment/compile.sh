
mkdir -p window.app/Contents/MacOS

Clang++ -Wdeprecated-declarations -o window.app/Contents/MacOS/window Sphere.mm window.mm -framework Cocoa -framework QuartzCore -framework OpenGL
