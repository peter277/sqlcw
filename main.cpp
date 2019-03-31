// sqlcw: SQL Clean & Wrap
// Strips SQL files of comments/converts comments and wraps each statement
//   in specified text, e.g. prefix "execute (", suffix: ") by odbc;"

// TODO: have option to preserve comments, and convert "--" comments to "/* */" (complication: what if the -- comment contains the closing sequence "*/" ? - throw error/warning?)

#include <cstdio>
#include <cctype>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

int main(int argc, char **argv)
{
    char ch1, ch2;

    FILE *fin = fopen("infile.sql","r"), *fout = fopen("outfile.sql","w");

    const char *prefix = "execute (\n", *suffix = "\n) by odbc;";
    //const char *prefix = "", *suffix = "\nGO\n";
    bool wrote_prefix = false;

    // Initialise state machine
    ch1 = fgetc(fin);
    ch2 = fgetc(fin);

    while (true) {
        // If we'are at a comment, consume characters until the end of the line/file/comment block
        if (ch1 == '-' && ch2 == '-')
        {
            while (ch1 != '\n' && ch1 != EOF)
            {
                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            continue;
        }

        if (ch1 == '/' && ch2 == '*')
        {
            while ( !(ch1 == '*' && ch2 == '/') && ch1 != EOF )
            {
                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            ch1 = fgetc(fin);
            ch2 = fgetc(fin);
            continue;
        }

        // Handle quotes
        if (ch1 == '\'' || ch1 == '"')
        {
            bool wrote_closing_quote = false;
            char quote_type = ch1;

            // Write opening quote & move cursor
            fputc(ch1, fout);
            ch1 = ch2;
            ch2 = fgetc(fin);

            while (true)
            {
                fputc(ch1, fout);
                if (ch1 == quote_type) wrote_closing_quote = true;

                // Move cursor
                ch1 = ch2;
                ch2 = fgetc(fin);

                if (ch1 == EOF || wrote_closing_quote) break;
            }

            continue;
        }

        if (ch1 == EOF)
        {
            break;
        }
        else
        {
            if (ch1 == ';')
            {
                fputc(ch1, fout);
                fprintf(fout, "%s", suffix);
                wrote_prefix = false; // Reset prefix flag
            }
            else
            {
                if (!wrote_prefix && !isspace(ch1))
                {
                    fprintf(fout, "%s", prefix);
                    wrote_prefix = true;
                }

                fputc(ch1, fout);
            }
        }

        // Move cursor
        ch1 = ch2;
        ch2 = fgetc(fin);
    }

    fclose(fin);
    fclose(fout);

    return 0;
}
