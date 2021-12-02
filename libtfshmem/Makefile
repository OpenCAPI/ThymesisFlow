CC=gcc
CFLAGS=-fPIC -Wall
ODIR=objs
SRC_DIR=src
LIB_DIR=lib
BIN_DIR=bin
OUT_DIRS= $(ODIR) $(LIB_DIR) $(BIN_DIR)
INSTALL_LIBDIR=/usr/lib64
INSTALL_INCDIR=/usr/include


TF_FOLDER=../libthymesisflow
DEBUG=1

ifeq ($(DEBUG),1)
	CFLAGS+=-D DEBUG
endif

CFLAGS+=-D MOCK -I./include -I$(TF_FOLDER)/include

CORE_SRCS := $(wildcard src/*.c)
CORE_OBJ:= $(addprefix $(ODIR)/,$(notdir $(CORE_SRCS:.c=.o)))
TEST_SRCS := $(wildcard test/*.c)
TEST_BIN:= $(addprefix $(BIN_DIR)/,$(notdir $(TEST_SRCS:.c=)))

VPATH= src test

all: make_dirs libtfshmem tests

make_dirs:
	mkdir -p $(OUT_DIRS)

$(ODIR)/%.o: src/%.c
	$(info making $<)
	$(CC) $(CFLAGS) -c -o $@ $<

libtfshmem: $(CORE_OBJ)
	$(CC) -shared -o lib/$@.so $(CORE_OBJ) $(TF_FOLDER)/lib/libthymesisflow.a

$(BIN_DIR)/%: %.c
	$(CC) $(CFLAGS) -o $@ $< -L$(LIB_DIR) -ltfshmem -locxl -lpthread

tests: $(TEST_BIN)

clean:
	rm -fr cscope.*
	rm -rf $(OUT_DIRS)

install:
	sudo install include/tf_shmem_api.h $(INSTALL_INCDIR)
	sudo install lib/libtfshmem.so $(INSTALL_LIBDIR)

uninstall:
	sudo rm $(INSTALL_INCDIR)/tf_shmem_api.h $(INSTALL_LIBDIR)/libtfshmem.so


cscope:
	-find . -name "*.[ch]" > cscope.files
	-cscope -b -R
