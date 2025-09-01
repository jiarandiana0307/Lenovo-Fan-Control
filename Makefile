BIN_DIR = bin
SRC_DIR = src
RES_DIR = res

BINARY_PREFIX = LenovoFanControl
OUT_x64 = ${BIN_DIR}/${BINARY_PREFIX}-x64.exe
OUT_x86 = ${BIN_DIR}/${BINARY_PREFIX}-x86.exe

source_files = ${SRC_DIR}/lenovo_fan_control.c ${SRC_DIR}/fanctrl.c ${SRC_DIR}/fanctrl.h
res_files = ${RES_DIR}/icon.ico ${RES_DIR}/icon.rc ${RES_DIR}/resource.h

all: ${OUT_x64} ${OUT_x86}
x64: ${OUT_x64}
x86: ${OUT_x86}

# x64
#########################################################

${OUT_x64}: ${source_files} ${BIN_DIR}/icon-x64.o
	gcc -static -m64 -o ${OUT_x64} ${source_files} ${BIN_DIR}/icon-x64.o -lpthread -mwindows

${BIN_DIR}/icon-x64.o: ${res_files} ${BIN_DIR}
	windres -F pe-x86-64 -i ${RES_DIR}/icon.rc -o ${BIN_DIR}/icon-x64.o

# x86
#########################################################

${OUT_x86}: ${source_files} ${BIN_DIR}/icon-x86.o
	gcc -static -m32 -o ${OUT_x86} ${source_files} ${BIN_DIR}/icon-x86.o -lpthread -mwindows

${BIN_DIR}/icon-x86.o: ${res_files} ${BIN_DIR}
	windres -F pe-i386 -i ${RES_DIR}/icon.rc -o ${BIN_DIR}/icon-x86.o

#########################################################

${BIN_DIR}:
	MKDIR "${BIN_DIR}"

clean_all: clean
	del ${BIN_DIR}\*.exe

clean:
	del ${BIN_DIR}\*.o
