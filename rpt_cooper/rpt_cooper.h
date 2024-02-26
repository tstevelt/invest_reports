//     Invest report
// 
//     Copyright (C)  2019 - 2024 Tom Stevelt
// 
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Affero General Public License as
//     published by the Free Software Foundation, either version 3 of the
//     License, or (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Affero General Public License for more details.
// 
//     You should have received a copy of the GNU Affero General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<ctype.h>

#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <sys/types.h>

#include	"shslib.h"

#include	"dbylib.h"
#include	"rptlib.h"
#include	"ta-lib/ta_func.h"

#define		STOCK
#define		HISTORY
#define		AVERAGE
#define		MEMBER
#define		WATCHLIST
#define		PORTFOLIO
#define		TEMP
#include	"fileinvest.h"
#include	"invlib.h"

/*----------------------------------------------------------
	10 days high and low = 10
	2 calendar months = 44
	123-pullback needs 14 day ADX,-DI,+DI = 14 + 13 + 100
	max volume on 7/2/2019 was 75 million
----------------------------------------------------------*/
#define		MINIMUM_HISTORY_RECORDS		127
#define		MINIMUM_PRICE				20.00
#define		MAXIMUM_PRICE				200.00
#define		MINIMUM_AVERAGE_VOLUME		250000
#define		MAXIMUM_AVERAGE_VOLUME		10000000

TYPE	char	StockIndex;
TYPE	int		CalcBothIndex;
TYPE	int		Weeks;
TYPE	int		Debug;
TYPE	char	EndDate[12];
TYPE	long	ReportMember;
TYPE	int		ReportCount;
TYPE	FILE	*fpOutput;
TYPE	char	OutFileName[256];
TYPE	REPORT_OPTIONS		ReportOptions;

#define		MAXRECS		20

typedef struct 
{
	char	Date[12];
	double	Open;
	double	Close;
	long	Volume;
} RECORD;

TYPE	RECORD	Array[MAXRECS];
TYPE	int		Count;

/*----------------------------------------------------------
	mysql stuff
----------------------------------------------------------*/
TYPE  MYSQL   MySql;
TYPE	DBY_OPTIONS	DbyOptions;
TYPE	char		Statement[1024];
TYPE	char		WhereClause[1024];
TYPE	char		OrderByClause[1024];
#ifdef MAIN
char	*LogFileName = "/var/local/invest.log";
#else
TYPE	char	*LogFileName;
#endif

/*------------------------------
:.,$d
:r ! mkproto -p *.c
------------------------------*/

/* breakout.c */
int main ( int argc , char *argv []);

/* dorpt.c */
void dorpt ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);
