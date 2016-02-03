CFLAGS := -fPIC -O3 -g -Wall -Werror

CC := gcc
MAJOR := 0
MINOR := 1
NAME := alocs
VERSION := $(MAJOR).$(MINOR)

lib: lib$(NAME).so.$(VERSION)

lib$(NAME).so.$(VERSION): common.o $(NAME).o iceph.o ceph_lib.o
	$(CC) -shared -Wl,-soname,lib$(NAME).so.$(MAJOR) common.o alocs.o iceph.o ceph_lib.o -o $@

common.o: common.c common.h
	$(COMPILE.c) -g common.c
	
alocs.o: alocs.c alocs.h
	$(COMPILE.c) -g alocs.c
	
iceph.o: iceph.c iceph.h
	$(COMPILE.c) -g iceph.c

ceph_lib.o: ceph_lib.c ceph_lib.h
	$(COMPILE.c) -g ceph_lib.c
	
clean:
	$(RM) *.o *.so*		
