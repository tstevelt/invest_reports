
PROG=rpt_near52

ifeq ($(LDFLAGS),"-m32")
LIB = rpt_near52.a
XLIB = /usr/local/lib/invlib.a /usr/local/lib/rptlib.a /usr/local/lib/dbylib.a /usr/local/lib/shslib.a  -luuid -lcrypto -lm
else
LIB = rpt_near52_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/rptlib.a  /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so  /usr/local/lib64/libxlsxwriter.a -lz -lssl -luuid -lcrypto -lm
endif

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(rpt_near52.o)	\
	$(LIB)(getargs.o)	\
	$(LIB)(EachStock.o)	\
	$(LIB)(dorpt.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): rpt_near52.h $(DEF)/dataproc.h $(DEF)/shslib.h $(DEF)/invlib.h  $(DEF)/rptlib.h $(DEF)/dbylib.h $(DEF)/fileinvest.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
