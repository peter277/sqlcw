#include <cstdio>
#include <cctype>
#include <iostream>

#include "Settings.hpp"
#include "LineBuffer.hpp"

#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

using namespace std;

// Declarations
void process_file(boost::filesystem::path infile, Settings& settings);
std::string replace_escape_seq(std::string s);


int main(int argc, char **argv)
{
    po::variables_map vmSettings; // Boost program_options settings structure
    Settings progSettings; // Own settings structure

    try {
        std::string config_file;

        // Declare a group of options that will be allowed only on command line
        po::options_description generic("Generic");
        generic.add_options()
            ("version,v", "Print version string")
            ("help,h", "Display this help information")
            ("config,c", po::value<string>(&config_file)->default_value("sqlcw.cfg"), "Configuration file to use (optional)")
            ;

        // Declare a group of options that will be allowed both on command line and in config file
        po::options_description config("Configuration");
        config.add_options()
            ("prefix,p", po::value<std::string>(&progSettings.prefix)->default_value(""), "Prefix to place before SQL statements")
            ("suffix,s", po::value<std::string>(&progSettings.suffix)->default_value(""), "Suffix to place after SQL statements")
            ("out-dir,o", po::value<std::string>(&progSettings.out_dir)->default_value("sqlcw-out"), "Directory to write output files")
            ("out-ext,x", po::value<std::string>(), "Extension of output files")
            ("comments,m", po::value<std::string>()->default_value("convert"), "Handling of comments: 'strip' or 'convert' to /* */ style")
            ("whitespace,w", po::value< std::vector<std::string> >()->multitoken(), "Whitespace processing switches list: 'single', 'nonewline'")
            ("strip-semicolons", po::bool_switch()->default_value(false), "Strip semicolons from SQL statements")
            ;

        // Hidden options, will be allowed both on command line and in config file, but will not be shown to the user
        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-file", po::value< std::vector<std::string> >(), "Input file")
            ;

        po::options_description cmdline_options;
        cmdline_options.add(generic).add(config).add(hidden);

        po::options_description config_file_options;
        config_file_options.add(config).add(hidden);

        po::options_description visible("Options");
        visible.add(generic).add(config);

        po::positional_options_description pos_desc;
        pos_desc.add("input-file", -1);

        po::store(po::command_line_parser(argc, argv).
            options(cmdline_options).positional(pos_desc).run(), vmSettings);
        po::notify(vmSettings);

        if (vmSettings.count("help")) {
            cout << visible << endl;
            return 0;
        }

        if (vmSettings.count("version")) {
            cout << "SQLCW - SQL Code Wrapper, v1.0" << endl;
            return 0;
        }

        if (!vmSettings.count("input-file")) {
            cout << "No input files specified. Use option -h or --help for help." << endl;
            return 1;
        }

        if (vmSettings.count("comments"))
        {
            std::string s = vmSettings["comments"].as<std::string>();

            if (s == "convert")
            {
                progSettings.convert_comments = true;
            }
            else if (s == "strip")
            {
                progSettings.convert_comments = false;
            }
            else
            {
                cout << "Invalid value specified for --comments option." << endl;
                return 1;
            }
        }

        if (vmSettings.count("out-ext")) {
            progSettings.out_ext = vmSettings["out-ext"].as<std::string>();
        }

        if(vmSettings.count("whitespace")) {
            for (auto &s : vmSettings["whitespace"].as<std::vector<std::string>>())
            {
                if (s == "single")
                {
                    progSettings.ws_single = true;
                }
                else if (s == "nonewline")
                {
                    progSettings.ws_nonewline = true;
                }
                else
                {
                    cout << "Invalid value specified for --whitespace option." << endl;
                    return 1;
                }
            }
        }

        progSettings.strip_semicolons = vmSettings["strip-semicolons"].as<bool>();

        // Process potential escape characters used in prefix & suffix strings
        progSettings.prefix = replace_escape_seq(progSettings.prefix);
        progSettings.suffix = replace_escape_seq(progSettings.suffix);


        // Create output directory
        fs::create_directory(progSettings.out_dir);
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }

    // Loop through files
    for (auto &f : vmSettings["input-file"].as< vector<string> >())
    {
        process_file(f, progSettings);
    }

    return 0;
}


/// Process input SQL file using configuration in provided Settings object
void process_file(boost::filesystem::path infile, Settings& settings)
{
    char ch1, ch2;
    boost::filesystem::path outfile;

    if (settings.out_ext.is_initialized())
    {
        std::string out_ext = std::string(".")+settings.out_ext.value();
        outfile = boost::filesystem::path(settings.out_dir) / (infile.stem().string()+out_ext);
    }
    else
    {
        outfile = boost::filesystem::path(settings.out_dir) / infile.filename();
    }

    FILE *fin = fopen(infile.string().c_str(),"r"),
         *fout = fopen(outfile.string().c_str(),"w");

    bool wrote_prefix = false;

    // Initialise state machine
    ch1 = fgetc(fin);
    ch2 = fgetc(fin);

    // Current line being processed
    LineBuffer currLine;

    while (true) {
        // Handle comments
        if (ch1 == '-' && ch2 == '-')
        {
            ch1 = fgetc(fin); ch2 = fgetc(fin); // Read past "--" characters

            bool space_flag = isspace(ch1); // Comment starts with space?

            if (settings.convert_comments) currLine += "/*";

            while ( !(ch1 == '\n' || ch1 == EOF) )
            {
                if (settings.convert_comments)
                {
                    currLine += ch1;

                    if (ch1 == '*' && ch2 == '/')
                    {
                        cout << "Warning (" << infile.filename() << R"(): Found comment closure "*/" while converting "--" style comment. Space inserted inbetween.)" << endl;
                        currLine += ' ';
                    }
                }

                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            if (settings.convert_comments)
            {
                if (space_flag) currLine += ' '; // If comment started with a space, end with one before the closing "*/"
                currLine += "*/";
            }

            if (ch1 == '\n')
            {
                currLine += ch1;
                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            // Write line
            if (!settings.convert_comments) currLine.write_nonspace_line(fout);
            else currLine.write_line(fout);

            continue;
        }

        if (ch1 == '/' && ch2 == '*')
        {
            while ( !(ch1 == '*' && ch2 == '/') && ch1 != EOF )
            {
                if (settings.convert_comments) currLine += ch1;

                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            if (settings.convert_comments) currLine += "*/";

            ch1 = fgetc(fin);
            ch2 = fgetc(fin);

            // Read up until the next non-space character or newline
            while (isspace(ch1) && ch1 != '\n')
            {
                currLine += ch1;
                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            if (ch1 == '\n')
            {
                currLine += ch1;
                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            // Write line(s)
            if (!settings.convert_comments) currLine.write_nonspace_line(fout);
            else currLine.write_line(fout);

            continue;
        }

        // Handle quotes
        if (ch1 == '\'' || ch1 == '"' || ch1 == '`' || ch1 == '[')
        {
            bool wrote_closing_quote = false;
            char closing_quote;

            if (ch1 == '[') closing_quote = ']'; else closing_quote = ch1;

            // Write opening quote & move cursor
            currLine += ch1;
            ch1 = ch2;
            ch2 = fgetc(fin);

            while (true)
            {
                currLine += ch1;
                if (ch1 == closing_quote) wrote_closing_quote = true;

                // Move cursor
                ch1 = ch2;
                ch2 = fgetc(fin);

                if (ch1 == EOF || wrote_closing_quote) break;
            }

            continue;
        }

        // Handle whitespace processing options
        // nonewline option: strip off new lines
        if (ch1 == '\n' && settings.ws_nonewline)
        {
            // Move cursor
            ch1 = ch2;
            ch2 = fgetc(fin);

            if (!isspace(ch1) && ch1 != ';') currLine += ' ';

            continue;
        }

        // single option: compress multiple spaces into only one
        if (isspace(ch1) && ch1 != '\n' && settings.ws_single)
        {
            currLine += ' ';

            // Consume remaining spaces
            while( isspace(ch1) && ch1 != '\n' && ch1 != EOF)
            {
                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            continue;
        }

        if (ch1 == EOF)
        {
            if (wrote_prefix) currLine += settings.suffix;
            currLine.write_line(fout);
            wrote_prefix = false;
            break;
        }
        else
        {
            if (ch1 == ';')
            {
                if (!settings.strip_semicolons) currLine += ch1;
                currLine += settings.suffix;
                wrote_prefix = false; // Reset prefix flag
            }
            else
            {
                if (!wrote_prefix && !isspace(ch1))
                {
                    currLine += settings.prefix;
                    wrote_prefix = true;
                }

                currLine += ch1;
            }
        }

        // Reached end of character processing loop iteration - write line if hit a newline
        if (ch1 == '\n') currLine.write_line(fout);

        // Move cursor
        ch1 = ch2;
        ch2 = fgetc(fin);
    }

    fclose(fin);
    fclose(fout);
}


/// Replace C++ escape sequences within string outside source code
std::string replace_escape_seq(std::string s)
{
    static vector< pair< string, string > > patterns = {
        { "\\n", "\n" },
        { "\\r", "\r" },
        { "\\t", "\t" },
        { "\\\\", "\\" },
        { "\\\"", "\"" }
    };

    if (s.size() <= 1) return s;

    std::string result;
    result.reserve(s.size());

    for (unsigned i = 0; i<s.size(); i++)
    {
        bool found_pattern = false;

        if ( i<(s.size()-1) )
        {
            for (auto &p : patterns)
            {
                if (s[i] == p.first[0] && s[i+1] == p.first[1])
                {
                    found_pattern = true;
                    result += p.second;
                    i++;
                    break;
                }
            }

            if (!found_pattern)
            {
                result += s[i];
            }
        }
        else
        {
            result += s[i];
        }
    }

    return result;
}

