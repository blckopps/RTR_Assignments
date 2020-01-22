mkdir -p window.app/Contents/MacOS
#rm -rf *.txt
Clang++ -Wdeprecated-declarations -o window.app/Contents/MacOS/window window.mm -framework Cocoa -framework QuartzCore -framework OpenGL
