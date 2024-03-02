#!/bin/sh
#     Invest report
# 
#     Copyright (C)  2019 - 2024 Tom Stevelt
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU Affero General Public License as
#     published by the Free Software Foundation, either version 3 of the
#     License, or (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU Affero General Public License for more details.
# 
#     You should have received a copy of the GNU Affero General Public License
#     along with this program.  If not, see <https://www.gnu.org/licenses/>.

#volume I 1 -fmt csv
#volume I 1 -fmt html

#volume A 1 -fmt csv
#volume A W -fmt html -member 501


#USAGE: volume type index          [options]
#USAGE: volume type -file filename [options]
#USAGE: volume type -ticker TICKER [options]
#type:
# I = Increasing volume
# A = Accumulate exceeds Distribution
#index:
# S = S&P 500
# D = Dow Jones Industrials
# N = Nasdaq 100
# M = Midcap (Russell 1000 ex S&P 500)
# 1 = Russell 1000
# 2 = Russell 2000
# 3 = Russell 3000
# P = Portfolio
# W = Watchlist
#Options:
# -member id (required for Watchlist or Portfolio
# -method = O (compare to open) or P (compare to previous close)
# -pcnt pcnt = report above this percentage
# -vol vol = minimum volume
# -days curr = number of trading days
# -compare prev curr = number of trading days compare current to previous
# -fmt = csv (default), txt, html, pdf (n/a) or excel (n/a)
# -d = debug

# volume I S -member 501 -pcnt 120.00 -vol 250000 -compare 45 5 -fmt html 
# volume I -ticker GRMN -member 501 -pcnt 150.00 -vol 250000 -compare 45 5 -fmt html  -d

# volume A S -member 501 -pcnt 125.00 -vol 250000 -days 30 -method P -fmt html
volume A -ticker SYMC  -member 501 -pcnt 125.00 -vol 250000 -days 30 -method P -fmt html -d
