CC := gcc
CFLAGS := -nostdlib
CFLAGS += -I ./src/include

DIR_SRC = ./src
DIR_BIN = ./bin
DIR_OBJ = $(DIR_BIN)/obj

# $@ 表示规则的目标文件名。
# $< 规则的第一个依赖的文件名。
# $? 所有比目标文件更新的依赖文件列表，空格分隔。
# $^ 代表的是所有依赖文件列表，使用空格分隔。
# $+ 类似 $^ ，但是它保留了依赖文件中重复出现的文件。

$(DIR_BIN)/Kernel.elf: \
	$(DIR_OBJ)/Kernel.o \
	$(DIR_OBJ)/Graphic.o \
	$(DIR_OBJ)/Memory.o \
	$(DIR_OBJ)/Printk.o \
	$(DIR_OBJ)/X64/GDT.o

	$(shell mkdir -p $(dir $@))
	ld $^ -o $@
	cp $@ /mnt/d/qemu/ovmf/esp/

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(DIR_BIN)
