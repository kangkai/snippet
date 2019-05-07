all:
	/tmp/kaibuild/host/bin/arm-linux-gcc -Wall -Wextra -Wwrite-strings -fPIC -c -o hook.o hook.c
	/tmp/kaibuild/host/bin/arm-linux-gcc -Wall -Wextra -Wwrite-strings -shared -o hook.so hook.o -ldl

android:
	NDK_ARCH=arm ndk-gcc -Wall -Wextra -Wwrite-strings -fPIC -c -o hook.o hook.c
	NDK_ARCH=arm ndk-gcc -Wall -Wextra -Wwrite-strings -shared -o hook.so hook.o -ldl

clean:
	rm -f *.so *.o
