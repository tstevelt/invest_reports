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

/*----------------------------------------------------------
	only one of the following should be defined
----------------------------------------------------------*/
#undef TOUGH_NICKLE 
#define SIMPLE_CROSSOVER 
#undef MA_CONFIRM 
#undef DONT_FIGHT_THE_MARKET


#define		STOCK
#define		HISTORY
#define		AVERAGE
#ifdef DONT_FIGHT_THE_MARKET
#define		MARKET
#endif
#define		MEMBER
#define		PORTFOLIO
#define		WATCHLIST
#define		TEMP
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	int		Debug;
TYPE	int		CountVisited;
TYPE	char	StockIndex;
TYPE	long	ReportMember;
TYPE	int		CrossOverSince;
TYPE	double	MinimumADX;
TYPE	double	BuyAmount;
TYPE	int		MinShares;
TYPE	double	MaxPrice;

typedef struct
{
	char	Date[12];
	double	Ratio;
} MARKET_RECORD;

TYPE	MARKET_RECORD	MarketArray[MAXHISTRECS];
TYPE	int				MarketCount;

#ifdef STUFF
TYPE	int		ThisCount, ThisError;
TYPE	XHISTORY		ThisHistory;
TYPE	HISTORY_RECORD	BenchmarkArray[MAXHISTRECS];
TYPE	int				BenchmarkCount;

TYPE	int		PriceInserted;
TYPE	int		PriceUpdated;
TYPE	int		PriceErrors;

TYPE	int		VolumeInserted;
TYPE	int		VolumeUpdated;
TYPE	int		VolumeErrors;

TYPE	int		RSI_Inserted;
TYPE	int		RSI_Updated;
TYPE	int		RSI_Errors;

#define		MODE_ALL		'A'
#define		MODE_ZERO		'Z'
TYPE	int		RunMode;
#endif

TYPE    REPORT_OPTIONS  ReportOptions;
TYPE    int         ReportCount;
TYPE    FILE        *fpOutput;
TYPE    char        OutFileName[256];

/*----------------------------------------------------------
	mysql and dbylib stuff
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

/* ClearFlags.c */
void ClearFlags ( void );

/* dorpt.c */
void dorpt ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* rpt_dicross.c */
int main ( int argc , char *argv []);

/* sortdata.c */
void sortdata ( void );
