# For Automatic variables see:
# http://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables


# Build Authy OpenVPN plugin module on *nix.

CC= gcc
LIBNAME= authy-openvpn

RELEASE_FLAGS= -fPIC -O2 -DNDEBUG
DEBUG_FLAGS= -fPIC -O2 -Wall -DWITH_DEBUG -ggdb -Werror

CFLAGS= $(RELEASE_FLAGS)



OBJFLAGS= -I./src/headers/ -c
LIBFLAGS= -shared -Wl,-soname
BUILD_DIR= build
SDIR= src

INSTDIR=/usr/lib/authy

# This functions will list all the files in a directory an add .o to them
# Wildcard: will list all .c files.
# basename: will remove the .c from the names
# notdir: will remove the directory
# addsuffix: will add the .o
# EG: $(addsuffix .o, $(notdir $(basename $(wildcard $(SDIR)/*.c))))
_OBJS = $(addsuffix .o, $(notdir $(basename $(wildcard $(SDIR)/*.c))))
OBJS = $(patsubst %,$(BUILD_DIR)/%,$(_OBJS))

_JSMN_OBJS = $(addsuffix .o, $(notdir $(basename $(wildcard $(SDIR)/vendor/jsmn/*.c))))
JSMN_OBJS = $(patsubst %,$(BUILD_DIR)/vendor/%,$(_JSMN_OBJS))


all: $(BUILD_DIR)/$(LIBNAME).so

# Build all our .o
$(BUILD_DIR)/%.o: $(SDIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $<

#build vendor jsmn module. We need to duplicate the rule above as it's not possible
# to use 2 patterns as pre-requisites in 1 rules.
$(BUILD_DIR)/vendor/%.o: $(SDIR)/vendor/jsmn/%.c
	mkdir -p $(BUILD_DIR)/vendor
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $<

# Make Authy shared Lib.
$(BUILD_DIR)/$(LIBNAME).so: $(OBJS) $(JSMN_OBJS)
	@# MAC SUPER dylib compile gcc -dynamiclib	-Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/lib$(OBJ).1.dylib	-o lib$(OBJ).1.dylib $(OBJ).o
	$(CC) $(CFLAGS) $(LIBFLAGS),$(LIBNAME).so -o $@ $^ -lc -lcurl


# $(DESTDIR) is used by debian makefiles
install: all
	mkdir -p $(DESTDIR)$(INSTDIR)
	cp $(BUILD_DIR)/$(LIBNAME).so $(DESTDIR)$(INSTDIR)
	chmod 755 $(DESTDIR)$(INSTDIR)/*.so
	mkdir -p $(DESTDIR)/usr/sbin
	cp scripts/authy-vpn-add-user $(DESTDIR)/usr/sbin/authy-vpn-add-user
	chmod 700 $(DESTDIR)/usr/sbin/authy-vpn-add-user


# Debug build with debug symbols
debug: CFLAGS = $(DEBUG_FLAGS)
debug: clean
debug: all


clean:
	rm -rf $(BUILD_DIR)

