
PROG=rpt_cooper

LIB = rpt_cooper_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/rptlib.a /usr/local/lib64/libta_lib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so  /usr/local/lib64/libxlsxwriter.a -lz -lssl -luuid -lcrypto -lm

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(rpt_cooper.o)	\
	$(LIB)(getargs.o)	\
	$(LIB)(EachStock.o)	\
	$(LIB)(dorpt.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): rpt_cooper.h $(DEF)/shslib.h $(DEF)/rptlib.h $(DEF)/invlib.h  $(DEF)/dbylib.h $(DEF)/fileinvest.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
