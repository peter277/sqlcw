
@echo off

sqlcw infile.sql infile2.sql infile3 -p "execute (\n" -s "\n) by odbc;" -x sas --comments=convert -t "proc sql;\n\n" -b "\nquit;\n"

sqlcw infile4.sql -w nonewline

pause
