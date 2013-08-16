#
# Build Authy OpenVPN plugin module on *nix.

CC=gcc
LIBNAME=authy_openvpn
PAM=pam

CFLAGS=-fPIC -O2 -Wall
OBJFLAGS=-I./src/include/ -c
LIBFLAGS=-shared -Wl,-soname
INSTDIR=/usr/lib/authy
BUILD_DIR= build
SDIR= src

_OBJS = authy_openvpn.o authy_api.o jsmn.o authy_conf.o
OBJS = $(patsubst %,$(BUILD_DIR)/%,$(_OBJS))

_PAM_OBJS = auth-pam.o pamdl.o 
PAM_OBJS = $(patsubst %,$(BUILD_DIR)/vendor/%,$(_PAM_OBJS))

all: $(BUILD_DIR)/$(LIBNAME).so $(BUILD_DIR)/vendor/$(PAM).so

# Build all .o
$(BUILD_DIR)/%.o: $(SDIR)/%.c 
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -lcurl $(OBJFLAGS) -o $@ $<

# Make Authy lib
$(BUILD_DIR)/$(LIBNAME).so: $(OBJS)
	# MAC SUPER dylib compile gcc -dynamiclib	-Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/lib$(OBJ).1.dylib	-o lib$(OBJ).1.dylib $(OBJ).o
	$(CC) $(CFLAGS) -lc -lcurl $(LIBFLAGS),$(LIBNAME).so -o $@ $^

$(BUILD_DIR)/vendor/%.o: $(SDIR)/vendor/custom-pam-module/%.c
	mkdir -p $(BUILD_DIR)/vendor
	$(CC) $(CFLAGS) -lpam $(OBJFLAGS) -o $@ $< 

$(BUILD_DIR)/vendor/$(PAM).so: $(PAM_OBJS)
	$(CC) $(CFLAGS) -lc -lpam $(LIBFLAGS),$(PAM).so -o $@ $^


install: $(LIBNAME).so $(PAM).so
	mkdir -p $(DESTDIR)$(INSTDIR)
	cp $(LIBNAME).so $(PAM).so $(DESTDIR)$(INSTDIR)
	chmod 755 $(DESTDIR)$(INSTDIR)/*.so
	mkdir -p $(DESTDIR)/usr/sbin
	cp add_users $(DESTDIR)/usr/sbin/authy_vpn_add_users
	chmod 700 $(DESTDIR)/usr/sbin/authy_vpn_add_users

clean:
	rm -rf $(BUILD_DIR)

