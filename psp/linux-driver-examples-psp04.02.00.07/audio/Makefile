ROOTDIR = .

include $(ROOTDIR)/Rules.make

CFLAGS = -I$(INCLUDE_DIR) -I$(LIB_INC) -Wall
LFLAGS = -L$(LIB_DIR)

objects := $(patsubst %.c,%.o,$(wildcard *.c))
executables := $(patsubst %.c,%,$(wildcard *.c))

LIBS = -lasound -lpthread

all: $(.DELETE_ON_ERROR) $(objects)
$(objects): %.o: %.c
	mkdir -p ./bin
	$(CROSS_COMPILE)gcc -c $(CFLAGS) $< -o $@
	$(CROSS_COMPILE)gcc $(CFLAGS) $< -o $(subst .o, ,$(EXE_DIR)/$@) $(LFLAGS) $(LIBS)

clean:
	rm -f *.o rm -f ./bin/* rm -rf ./bin		      

distclean: clean
