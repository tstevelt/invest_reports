
All of thees reports are called from the website.  They can also be run from the command line or scheduled in crontab.

# Screeer Page Reports

### rpt_ad_hist Accumulate / Distribute History report
```
USAGE: rpt_ad_hist timeframe -fmt fmt [options]
timeframe: -1mo -6mo -1yr
Options:
 -fmt = csv (default), txt, html, xlsx or pdf (n/a)
 -member id
```
---------------------------------------------------------------------
### rpt_adx Report stocks with strong ADX and diff of MinusDI and PlusDI.
```
USAGE: rpt_adx -index  index  -member ID [options]
USAGE: rpt_adx -ticker TICKER -member ID [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
 -member id
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
Options:
 -minadx #     = minimum ADX, default 20.0
 -min_di #     = minimum DI difference, default 10.0
 -type B|S|A  = looking for Buy, Sell or All
 -d   = Debug
```
---------------------------------------------------------------------
### rpt_breakout Report stocks which have strong price and volumn increase.
```
USAGE: rpt_breakout index [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
Options:
 -member id
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
 -price #.#   Percent Increase (default 2)
 -volume #    Percent Increase (default 25)
 -minvol #    Minimum Avg Vol  (default 300000)
 -d
```
---------------------------------------------------------------------
### rpt_cooper Implement Jeff Cooper 'Hit and Run' screeners
```
USAGE: rpt_cooper index [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
Options:
 -member id
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
 -d
```
---------------------------------------------------------------------
### rpt_ctb Compare stocks perfomance to S&P 500
```
USAGE: rpt_ctb index [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
OPTIONS
 -member id   for watchlist or portfolio.  also for email report.
 {-top #|-bot #|-max #.#|-min #.#}] 
   -top #   = top number to report, default 5
   -bot #   = bottom number to report
   -min #.# = minimum RS to report, default 0.0
   -max #.# = maximum RS to report, default 0.0
 -fmt fmt] 
 -d]
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
```
---------------------------------------------------------------------
### rpt_dicross Report crossover of MinusDI and PlusDI
```
USAGE: rpt_dicross -index  index  -member ID [options]
USAGE: rpt_dicross -ticker TICKER -member ID [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
 -member id
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
Options:
 -days #       = cross over during past # of days, default 1
 -buyadx #     = minimum ADX for buy, default 30.0
 -buyamt #     = Dollar to buy, default 1000.0
 -minshares #  = Minimum shares to buy. Default 10
 -d   = Debug
```
---------------------------------------------------------------------
### rpt_fund Report of stocks which match criteria entered on the website
```
USAGE: rpt_fund -script filename -member ID [options]
Options:
 -fmt = csv (default), txt, html, xlsx or pdf (n/a)
 -d   = debug
```
---------------------------------------------------------------------
### rpt_macross Find tops or bottoms based on two ma crossing in past x days.
```
USAGE: rpt_macross index -avgs # # -days # -type {B|S|A} [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
 -avgs # #  = numbers can be 5, 10, 20, 50, 100 and 200
 -days #    = cross over during past # of days
 -type B|S|A  = looking for Buy, Sell or All
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
Options:
 -member id
 -d   = Debug
```
---------------------------------------------------------------------
### rpt_maratio Report ratio between count of MA1 > MA2
```
USAGE: rpt_maratio index -avgs # # -days # [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
 -avgs # #  = numbers can be 5, 10, 20, 50, 100 and 200
 -days #    = cross over during past # of days
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
Options:
 -minimum #.# = minimum percentage to report (default 90)
 -member id
 -d   = Debug
```
---------------------------------------------------------------------
### rpt_near52 Find stocks which have dropped x percent from 52 week high
and are now within y percent of recovery.  Similar to 
early IBD Cup and Handle (without the handle).
```
USAGE: rpt_near52 index [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
Options:
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
 -months #   = age of 52 week high (default 3)
 -within #.# = within percent of 52 week high (default 5)
 -dip #.#    = percent dip from 52 week high (default 25)
 -member id
 -one ticker (overrides any index)
 -d
```
---------------------------------------------------------------------
### rpt_regress Find best fit straight lines, output slope and std dev.
```
USAGE: rpt_regress startdate enddate index          [options]
USAGE: rpt_regress startdate enddate -file filename [options]
USAGE: rpt_regress startdate enddate -ticker ticker [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
Options:
 -member id
 -top = top number to report
 -sort = (R)eturn desc (C)orrCoeff desc (V)ariance
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
 -d   = debug
```
---------------------------------------------------------------------
### rpt_rsi Report stocks by RSI, by sector or by industry.  Can find 
```
USAGE: rpt_rsi index [-group {none|sector*|industry}] [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
OPTIONS
 -member id   for watchlist or portfolio.  also for email report.
 {-top #|-bot #|-max #.#|-min #.#}] 
   -top #   = top number to report, default 5
   -bot #   = bottom number to report
   -min #.# = minimum RS to report, default 0.0
   -max #.# = maximum RS to report, default 0.0
 -fmt fmt] 
 -d]
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
```
---------------------------------------------------------------------
### rpt_value Report intrinsic values (Graham and modern), margin of safety 
```
USAGE: rpt_value -member # [-currentAAA #.#] [-index x] [-range #.#] [options]
options:
 -currentAAA #.# current yield of AAA corp bonds, default 3.0 percent
                 https://fred.stlouisfed.org/series/AAA
 -index x, default is A, can also use P or W
 -range #.#, percent for Buy/Hold/Sell, default is 15.0 percent
 -fmt {csv*|txt|html|email|xlsx
 -d   = debug, prints alert or report, does not send email
 * default value
```
---------------------------------------------------------------------
### rpt_volume Find increasing volume and price
```
ARG COUNT
USAGE: rpt_volume type index          [options]
USAGE: rpt_volume type -file filename [options]
USAGE: rpt_volume type -ticker TICKER [options]
type:
 I = Increasing volume
 A = Accumulate exceeds Distribution
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
 P = Portfolio
 W = Watchlist
 Z = Temp file
Options:
 -member id
 -pcnt pcnt = report above this percentage
 -vol vol = minimum volume
 -days curr = number of trading days
 -compare prev curr = number of trading days compare current to previous
 -fmt = csv (default), txt, html, xlsx, pdf (n/a), aw, rw or tmp
 -d = debug
```

# Portfolio Page Reports

---------------------------------------------------------------------
### rpt_portfolio A huge variety of portfolio reports.
```
USAGE: rpt_portfolio -member id [options]
options:
 -style {base*|mstar|full|fundamental|alerts|overnight|today|yoy}
 -type {all*|stock|etf|fixed|cash}
 -broker broker/account
 -sort {ticker*|gain|roi|compared|}
 -fmt {csv*|txt|html|email|xlsx|others not available
 -d   = debug, prints alert or report, does not send email
 * default value
```
---------------------------------------------------------------------
### rpt_buckets Portfolio allocation by "buckets".
```
USAGE: rpt_buckets -member id [options]
options:
 -fmt {csv*|txt|html|pdf (n/a)|xlsx}
 -d   = debug, prints alert or report, does not send email
 * default value
```
---------------------------------------------------------------------
### rpt_watchlist Print watchlist report, with either basic or fundamental data.
```
USAGE: rpt_watchlist -member id [options]
options:
 -style {base*|fundamental}
 -fmt = csv (default), txt, html, xlsx or pdf (n/a)
```
---------------------------------------------------------------------
### rpt_allocation Portfolio allocation by Sector or Industry or Both
```
USAGE: rpt_allocation -member id [options]
options:
 -style {sector*|industry|both}
 -fmt {csv*|txt|html|pdf (n/a)|xlsx}
 -d   = debug, prints alert or report, does not send email
 * default value
```
---------------------------------------------------------------------
### rpt_target 

Current Portfolio Asset Allocation vs Target Allocation
Add target percentages to member table and update from profile page.
```
USAGE: rpt_target -member id [options]
options:
 -fmt {html*|txt}
 -d   = debug, prints alert or report, does not send email
 * default value
```
