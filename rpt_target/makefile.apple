
PROG=rpt_target

ifeq ($(LDFLAGS),"-m32")
LIB = rpt_target.a
XLIB = /usr/local/lib/invlib.a /usr/local/lib/dbylib.a /usr/local/lib/rptlib.a /usr/local/lib/shslib.a  -luuid -lcrypto -lm
else
LIB = rpt_target_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/rptlib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so  /usr/local/lib64/libxlsxwriter.a -lz -lssl -luuid -lcrypto -lm
endif

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(rpt_target.o)	\
	$(LIB)(getargs.o)		\
	$(LIB)(EachPortfolio.o)	\
	$(LIB)(EachBond.o)		\
	$(LIB)(EachCash.o)		\
	$(LIB)(EachStock.o)		\
	$(LIB)(MapStype.o)		\
	$(LIB)(getdata.o)		\
	$(LIB)(cmprec.o)		\
	$(LIB)(dorpt.o)		

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): rpt_target.h $(DEF)/shslib.h $(DEF)/rptlib.h  $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
