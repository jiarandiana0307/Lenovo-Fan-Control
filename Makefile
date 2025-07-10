BINARY_PREFIX = LenovoFanControl
OUT_x64 = ${BINARY_PREFIX}-x64.exe
OUT_x86 = ${BINARY_PREFIX}-x86.exe
source_files = lenovo_fan_control.c fanctrl.c fanctrl.h

all: ${OUT_x64} ${OUT_x86}

${OUT_x64}: ${source_files} icon-x64.o
	gcc -m64 -static -o ${OUT_x64} ${source_files} icon-x64.o -lpthread -mwindows

${OUT_x86}: ${source_files} icon-x86.o
	gcc -m32 -static -o ${OUT_x86} ${source_files} icon-x86.o -lpthread -mwindows

icon-x64.o: icon.ico icon.rc resource.h
	windres -F pe-x86-64 -i icon.rc -o icon-x64.o

icon-x86.o: icon.ico icon.rc resource.h
	windres -F pe-i386 -i icon.rc -o icon-x86.o

clean:
	del *.o
