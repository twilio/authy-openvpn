make clean && gcc -fPIC -O2 -Wall src/*.c src/*.h -I./include/ -lcurl -c && gcc -fPIC -O2 -Wall -lcurl -shared -Wl,-soname,authy-openvpn.so -o authy-openvpn.so *.o -lc -lcurl
