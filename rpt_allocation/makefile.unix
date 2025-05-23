
PROG=rpt_allocation

ifeq ($(LDFLAGS),"-m32")
LIB = rpt_allocation.a
XLIB = /usr/local/lib/invlib.a /usr/local/lib/dbylib.a /usr/local/lib/rptlib.a /usr/local/lib/shslib.a  -luuid -lcrypto -lm
else
LIB = rpt_allocation_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/rptlib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so  /usr/local/lib64/libxlsxwriter.a -lz -lssl -luuid -lcrypto -lm
endif

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(rpt_allocation.o)	\
	$(LIB)(getargs.o)			\
	$(LIB)(getdata_portfolio.o)	\
	$(LIB)(getdata_cashbal.o)	\
	$(LIB)(putdata.o)			\
	$(LIB)(cmprec.o)			\
	$(LIB)(dorpt_industry.o)	\
	$(LIB)(dorpt_both.o)		\
	$(LIB)(dorpt_sector.o)	

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): rpt_allocation.h $(DEF)/shslib.h $(DEF)/rptlib.h  $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
