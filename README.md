<img src="icon/workflow.png" align="left" width="80" height="80" style="margin-right: 10px; margin-bottom: 10px;" alt="LOGO">

# sqlcw - SQL Code Wrapper
**A lightweight preprocessor for embedding SQL code in host programming languages**
<br clear="left"/>

sqlcw wraps SQL statements with configurable prefix and suffix strings, making it easy to embed SQL in C++, Python, or other languages. For example, wrapping with prefix `execute(` and suffix `) by cnxn;` transforms standalone SQL into ready-to-use function calls.

The tool is SQL syntax-aware and correctly handles quoted strings, identifiers, and comments. Additional options allow you to strip or convert comments and simplify whitespace.

## Usage
sqlcw is invoked from the command line. Run with `sqlcw --help` to see all available program options:
<pre>
Options:

Generic:
  -v [ --version ]                   Print version string
  -h [ --help ]                      Display this help information

Configuration:
  -p [ --prefix ] arg                Prefix to place before SQL statements
  -s [ --suffix ] arg                Suffix to place after SQL statements
  -t [ --top ] arg                   Header to place at top of output files
  -b [ --bottom ] arg                Footer to place at bottom of output files
  -o [ --out-dir ] arg (=sqlcw-out)  Directory to write output files
  -x [ --out-ext ] arg               Extension of output files
  -m [ --comments ] arg (=convert)   Handling of comments: 'strip' or 'convert'
                                     to /* */ style
  -w [ --whitespace ] arg            Whitespace processing switches list
                                     (multitoken option). Values:
                                       single: compress multiple whitespace
                                               characters into one (excl.
                                               newline)
                                       nonewline: strip off newlines from
                                                  output
  --strip-semicolons                 Strip semicolons from SQL statements
</pre>

## License

sqlcw is distributed under the [MIT License](LICENSE.md). See the license file for full details and third-party attributions.
