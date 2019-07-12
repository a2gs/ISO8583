CFLAGS_OPTIMIZATION = -g
#CFLAGS_OPTIMIZATION = -O3
CFLAGS_VERSION = -std=c11
CFLAGS_WARNINGS = -Wall -Wextra -Wno-unused-parameter # -Wno-unused-but-set-parameter -Wno-unused-result
CFLAGS_DEFINES = -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE=1 -D_DEFAULT_SOURCE=1 -D_GNU_SOURCE=1
CFLAGS = $(CFLAGS_OPTIMIZATION) $(CFLAGS_VERSION) $(CFLAGS_WARNINGS) $(CFLAGS_DEFINES)

CC = gcc
RM = rm -rf
CPPCHECK = cppcheck

LIBS = -lm -ldl

BIN_PATH = bin
SRC_PATH = src
INC_PATH = include
BKP_PATH = bkps
TOOLS_PATH = Tools

INCS = -I$(INC_PATH)

CPPCHECK_OPTS = --enable=all --std=c11 --platform=unix64 --language=c --check-config --suppress=missingIncludeSystem

all: clean cppcheck server client tcp_server
	@echo
	@echo ">>>>>>>>>>>>>>>>>>>> CTAGS/CSCOPE <<<<<<<<<<<<<<<<<<<<"
	-ctags -R *
	-cscope -b -R

server:
	@echo
	@echo ">>>>>>>>>>>>>>>>>>>> SERVER <<<<<<<<<<<<<<<<<<<<"
# Under construction
#	cc -o $(BIN_PATH)/ISO8583_server -DSERVER    \
		$(SRC_PATH)/main.c                        \
		$(SRC_PATH)/iso8583Utils.c                \
		$(SRC_PATH)/defaultBitFmt.c               \
		$(SRC_PATH)/metaData.c                    \
		$(SRC_PATH)/network.c                     \
		$(SRC_PATH)/plugins.c                     \
		$(TOOLS_PATH)/dump.c                      \
		$(TOOLS_PATH)/time_measurement.c          \
		$(TOOLS_PATH)/ascii2ebcdic_ebcdic2ascii.c \
		$(INCS) $(LIBS) $(CFLAGS)

client:
	@echo
	@echo ">>>>>>>>>>>>>>>>>>>> CLIENT <<<<<<<<<<<<<<<<<<<<"
	cc -o $(BIN_PATH)/ISO8583_client -DCLIENT    \
		$(SRC_PATH)/main.c                        \
		$(SRC_PATH)/iso8583Utils.c                \
		$(SRC_PATH)/defaultBitFmt.c               \
		$(SRC_PATH)/metaData.c                    \
		$(SRC_PATH)/network.c                     \
		$(SRC_PATH)/plugins.c                     \
		$(TOOLS_PATH)/dump.c                      \
		$(TOOLS_PATH)/time_measurement.c          \
		$(TOOLS_PATH)/ascii2ebcdic_ebcdic2ascii.c \
		$(INCS) $(LIBS) $(CFLAGS)

tcp_server:
	@echo
	@echo ">>>>>>>>>>>>>>>>>>>> DUMMY TCP SERVER <<<<<<<<<<<<<<<<<<<<"
	cc -o $(BIN_PATH)/tcp_server $(TOOLS_PATH)/tcp_server.c

clean:
	-$(RM) $(BIN_PATH)/* cscope.out tags
	echo "./ISO8583_client -i ../Messages/msg.txt -b ../BitsPlugins/ -s localhost -p 666" > $(BIN_PATH)/go
	chmod u+x bin/go

cppcheck:
	@echo "=== cppcheck ================"
	-$(CPPCHECK) $(CPPCHECK_OPTS) $(INCS) -i $(BIN_PATH) -i ./BitsPlugins/ -i ./Messages/ -i ./bkps/ -i ./docs/ -i ./format_sample/ -i ./trxs_dumps/ --suppress=missingIncludeSystem $(SRC_PATH) $(TOOLS_PATH)

#run:
#	$(BIN_PATH)/ISO8583_client -i Messages/msg.txt -s localhost -p 9998 -b./BitsPlugins

run_server:
	$(BIN_PATH)/ISO8583_server -p 9998

bkp:
	-tar -czf ./$(BKP_PATH)/bkp_`date +%Y%m%d_%H%M%S`.tar.gz ./BitsPlugins/ ./makefile ./Messages/ ./README.txt ./$(SRC_PATH)/ ./$(INC_PATH)/ ./$(TOOLS_PATH)/ ./$(BIN_PATH)/go
