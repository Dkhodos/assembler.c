CC	:=	gcc

SRC_DIR	:=	src
OBJ_DIR	:=	obj
BIN_DIR	:=	bin

EXE	:=	$(BIN_DIR)/assembler
SRC	:=	$(wildcard $(SRC_DIR)/*.c)
OBJ	:=	$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS	:=	-g	-Wall	-pedantic	-ansi
LDLIBS	:=	-lm

.PHONY:	all	clean

all:	$(EXE)

$(EXE):	$(OBJ)	|	$(BIN_DIR)
	$(CC)	$^	$(LDLIBS)	-o	$@

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c	|	$(OBJ_DIR)
	$(CC)	$(CFLAGS)	-c	$<	-o	$@

$(BIN_DIR)	$(OBJ_DIR):
	mkdir	-p $@

clean:
	@$(RM)	-rv	$(BIN_DIR)	$(OBJ_DIR)

-include	$(OBJ:.o=.d)