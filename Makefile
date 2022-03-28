CC := gcc
CFLAGS := -nostdlib
CFLAGS += -I ./src/include
CFLAGS += -g

# CFLAGS+= -fno-builtin	# 不需要 gcc 内置函数
# CFLAGS+= -nostdinc		# 不需要标准头文件
# CFLAGS+= -fno-pic		# 不需要位置无关的代码  position independent code
# CFLAGS+= -fno-pie		# 不需要位置无关的可执行程序 position independent executable
# CFLAGS+= -nostdlib		# 不需要标准库
# CFLAGS+= -fno-stack-protector	# 不需要栈保护

DIR_SRC = ./src
DIR_BIN = ./bin
DIR_OBJ = $(DIR_BIN)/obj

FILE_SRC = $(wildcard $(DIR_SRC)/*/*.c)
FILE_SRC += $(wildcard $(DIR_SRC)/*/*.S)

FILE_OBJ := $(patsubst $(DIR_SRC)/%.c, $(DIR_OBJ)/%_c.o, $(FILE_SRC))
FILE_OBJ := $(patsubst $(DIR_SRC)/%.S, $(DIR_OBJ)/%_S.o, $(FILE_OBJ))

# $@ 表示规则的目标文件名。
# $< 规则的第一个依赖的文件名。
# $? 所有比目标文件更新的依赖文件列表，空格分隔。
# $^ 代表的是所有依赖文件列表，使用空格分隔。
# $+ 类似 $^ ，但是它保留了依赖文件中重复出现的文件。

$(DIR_BIN)/Kernel.elf: $(FILE_OBJ)
	$(shell mkdir -p $(dir $@))
	ld $^ -o $@ -Ttext 0x101000
	cp $@ /mnt/d/qemu/ovmf/esp/

$(DIR_OBJ)/%_c.o: $(DIR_SRC)/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) -c $< -o $@ $(CFLAGS)

$(DIR_OBJ)/%_S.o: $(DIR_SRC)/%.S
	$(shell mkdir -p $(dir $@))
	$(CC) -c $< -o $@ -g

# nasm -f elf64 -g $< -o $@

.PHONY: c
c:
	rm -r $(DIR_BIN)

.PHONY: t
t:
	@echo $(FILE_SRC)
	@echo $(FILE_OBJ)
