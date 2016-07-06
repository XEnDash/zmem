call vcvarsall.bat x64

cl *.cpp /Femain.exe /F 536870912 /Zi /link /subsystem:console