SQLCW - SQL CODE WRAPPER, VERSION 1.1
Utility to wrap SQL for embedding within a host programming language
--------------------------------------------------------------------------------
Version                  : 1.1
Release Date             : 5 July 2019
SourceForge project page : http://sourceforge.net/projects/sqlcw/
License                  : GNU General Public License version 3.0 (GPLv3)
--------------------------------------------------------------------------------
    Copyright (C) 2019

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------
    SQLCW is based in part on the work of the Boost C++ Libraries project,
    distributed under the Boost Software License Version 1.0 (a copy is included
    with the source code).
--------------------------------------------------------------------------------

SQLCW is a lightweight preprocessor to enable wrapping SQL statements within a
specified prefix and suffix. For example prefix "execute (" and suffix
") by cnxn;".

The tool is SQL syntax aware and correctly handles quoted expressions and
comments. Options are also provided to strip or convert comments and simplify
whitespace.

SQLCW is invoked from the command line. Use option -h or --help for full options
list.

--------------------------------------------------------------------------------
