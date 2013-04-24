#
# Build Authy OpenVPN plugin module on *nix.

CC=gcc
OBJ=authy-openvpn
CFLAGS=-fPIC -O2 -Wall # for now warn everything
OBJFLAGS=-I./include/ -c
LIBFLAGS=-shared -Wl,-soname,$(OBJ).so

all: $(OBJ).so

$(OBJ).c: src/$(OBJ).c

$(OBJ).o: $(OBJ).c
	$(CC) $(CFLAGS) $(OBJFLAGS) src/$(OBJ).c

$(OBJ).so: $(OBJ).o
	# MAC SUPER dylib compile gcc -dynamiclib	-Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/lib$(OBJ).1.dylib	-o lib$(OBJ).1.dylib $(OBJ).o
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $(OBJ).so $(OBJ).o -lc

#install: $(OBJ).so for now nothing

clean:
	rm -f *.so *.o
