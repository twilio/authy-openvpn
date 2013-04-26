#
# Build Authy OpenVPN plugin module on *nix.

CC=gcc
LIBNAME=pam
CFLAGS=-fPIC -O2 -Wall
OBJFLAGS=-c
LIBFLAGS=-shared -Wl,-soname,$(LIBNAME).so

all: clean $(OBJ).so

objects:
	$(CC) $(CFLAGS) $(OBJFLAGS) *.c *.h -lpam

$(OBJ).so: objects
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $(LIBNAME).so *.o -lc -lpam

clean:
	rm -f *.so *.o
