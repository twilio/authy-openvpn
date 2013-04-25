#
# Build Authy OpenVPN plugin module on *nix.

CC=gcc
LIBNAME=authy-openvpn
CFLAGS=-fPIC -O2 -Wall
OBJFLAGS=-I./include/ -lcurl -c
LIBFLAGS=-shared -Wl,-soname,$(OBJ).so

all: clean $(OBJ).so

objects:
	$(CC) $(CFLAGS) $(OBJFLAGS) src/*.c src/*.h

$(OBJ).so: objects
	# MAC SUPER dylib compile gcc -dynamiclib	-Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/lib$(OBJ).1.dylib	-o lib$(OBJ).1.dylib $(OBJ).o
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $(LIBNAME).so *.o -lc -lcurl

#install: $(OBJ).so for now nothing

clean:
	rm -f *.so *.o
