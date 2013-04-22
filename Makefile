#
# Build Authy OpenVPN plugin module on *nix.

CC=gcc
OBJ=authy-openvpn
CFLAGS=-fPIC -O2 -Wall # for now warn everything
OBJFLAGS=-I./include/ -c
LIBFLAGS=-shared
# to add later -Wl

all: $(OBJ).so

$(OBJ).c: src/$(OBJ).c

$(OBJ).o: $(OBJ).c
	$(CC) $(CFLAGS) $(OBJFLAGS) src/$(OBJ).c

$(OBJ).so: $(OBJ).o
	$(CC) $(CFLAGS) $(LIBFLAGS),-soname,$(OBJ).so -o $(OBJ).so $(OBJ).o -lc

#install: $(OBJ).so for now nothing

clean:
	rm -f *.so *.o
