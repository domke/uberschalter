# Tiny Makefile to generate the CGI and start a lighttpd server for testing.

CC=gcc
CFLAGS=-g
CGIDIR=www/cgi-bin
LAMPCGI=$(CGIDIR)/lamp.cgi
LAMPSRC=src/lamp.c

all: lighttpdrun

plugin: $(LAMPSRC)
	mkdir -p $(CGIDIR)
	$(CC) $(CFLAGS) $(LAMPSRC) -o $(LAMPCGI)

lighttpdrun: plugin
	sed -e "s|PWD|${PWD}|g" lighttpd_relative.conf > lighttpd.conf
	@echo "====== Start LIGHTTPD ======="
	lighttpd -f lighttpd.conf -D
	rm -f lighttpd.conf

clean:
	rm -rf $(LAMPCGI)
	rm -rf lighttpd.conf
