#
# Build Authy OpenVPN plugin module on *nix.

CC=gcc
LIBNAME=authy-openvpn
PAM=pam

CFLAGS=-fPIC -O2 -Wall
OBJFLAGS=-I./src/include/ -c
LIBFLAGS=-shared -Wl,-soname
BUILD_DIR= build
SDIR= src

INSTDIR=/usr/lib/authy

_OBJS = authy_openvpn.o authy_api.o jsmn.o authy_conf.o
OBJS = $(patsubst %,$(BUILD_DIR)/%,$(_OBJS))

_PAM_OBJS = auth-pam.o pamdl.o 
PAM_OBJS = $(patsubst %,$(BUILD_DIR)/vendor/%,$(_PAM_OBJS))

all: $(BUILD_DIR)/$(LIBNAME).so $(BUILD_DIR)/vendor/$(PAM).so

# Build all .o
$(BUILD_DIR)/%.o: $(SDIR)/%.c 
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $< -lcurl

# Make Authy lib
$(BUILD_DIR)/$(LIBNAME).so: $(OBJS)
	@# MAC SUPER dylib compile gcc -dynamiclib	-Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/lib$(OBJ).1.dylib	-o lib$(OBJ).1.dylib $(OBJ).o
	$(CC) $(CFLAGS) $(LIBFLAGS),$(LIBNAME).so -o $@ $^ -lc -lcurl

$(BUILD_DIR)/vendor/%.o: $(SDIR)/vendor/custom-pam-module/%.c
	mkdir -p $(BUILD_DIR)/vendor
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $< -lpam

$(BUILD_DIR)/vendor/$(PAM).so: $(PAM_OBJS)
	$(CC) $(CFLAGS) $(LIBFLAGS),$(PAM).so -o $@ $^ -lc -lpam


install: $(BUILD_DIR)/$(LIBNAME).so $(BUILD_DIR)/vendor/$(PAM).so
	mkdir -p $(INSTDIR)
	cp $(BUILD_DIR)/$(LIBNAME).so $(BUILD_DIR)/vendor/$(PAM).so $(INSTDIR)
	chmod 755 $(INSTDIR)/*.so
	mkdir -p /usr/sbin
	cp scripts/add-users /usr/sbin/authy-vpn-add-users
	chmod 700 /usr/sbin/authy-vpn-add-users

clean:
	rm -rf $(BUILD_DIR)

