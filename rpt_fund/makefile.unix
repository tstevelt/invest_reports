
PROG=rpt_fund

ifeq ($(LDFLAGS),"-m32")
LIB = rpt_fund.a
XLIB = /usr/local/lib/invlib.a /usr/local/lib/rptlib.a /usr/local/lib/dbylib.a /usr/local/lib/shslib.a  -lcurl -luuid -lcrypto -lm
else
LIB = rpt_fund_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/rptlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so  /usr/local/lib64/libxlsxwriter.a -lcurl -lz -lssl -luuid -lcrypto -lm
endif

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(rpt_fund.o)		\
	$(LIB)(getargs.o)		\
	$(LIB)(getdata.o)		\
	$(LIB)(dorpt.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): rpt_fund.h $(DEF)/shslib.h  $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

test:
	gcc -g test_tiingo.c -Wall -ljson-c -o test_tiingo

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
