// sqlcw: SQL Clean & Wrap
// Strips SQL files of comments/converts comments and wraps each statement
//   in specified text, e.g. prefix "execute (", suffix: ") by odbc;"

// TODO: have option to preserve comments, and convert "--" comments to "/* */" (complication: what if the -- comment contains the closing sequence "*/" ? - throw error/warning?)

#include <cstdio>
#include <cctype>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


using namespace std;

void process_file(boost::filesystem::path infile, po::variables_map& settings);

// Replace C++ escape sequences from string outside source code
std::string replace_escape_seq(std::string s)
{
    static vector< pair< string, string > > patterns = {
        { "\\\\", "\\" },
        { "\\n", "\n" },
        { "\\r", "\r" },
        { "\\t", "\t" },
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

int main(int argc, char **argv)
{
    po::variables_map vmSettings;
    std::string out_dir;

    try {
        std::string config_file;

        // Declare a group of options that will be
        // allowed only on command line
        po::options_description generic("Generic");
        generic.add_options()
            ("version,v", "Print version string")
            ("help,h", "Display this help information")
            ("config,c", po::value<string>(&config_file)->default_value("sqlcw.cfg"), "Configuration file to use (optional)")
            ;

        // Declare a group of options that will be
        // allowed both on command line and in
        // config file
        po::options_description config("Configuration");
        config.add_options()
            ("prefix,p", po::value<std::string>()->default_value(""), "Prefix to place before SQL statements")
            ("suffix,s", po::value<std::string>()->default_value(""), "Suffix to place after SQL statements")
            ("out-dir,o", po::value<std::string>(&out_dir)->default_value("sqlcw-out"), "Directory to write output files")
            ("out-ext,x", po::value<std::string>(), "Extension of output files")
            ("comments", po::value<std::string>()->default_value("convert"), "Handling of comments: 'strip' or 'convert' to /* */ style")
            ;

        // Hidden options, will be allowed both on command line and
        // in config file, but will not be shown to the user.
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

        if ( !(vmSettings["comments"].as<std::string>() == "convert" || vmSettings["comments"].as<std::string>() == "strip" ) )
        {
            cout << "Invalid value specified for --comments option." << endl;
            return 1;
        }

        // Create output directory
        fs::create_directory(out_dir);

        // Process potential escape characters used in prefix & suffix strings
        po::variable_value escaped_prefix = po::variable_value(replace_escape_seq(vmSettings["prefix"].as<std::string>()), vmSettings["prefix"].defaulted());
        vmSettings.erase("prefix");
        vmSettings.insert( std::make_pair( std::string("prefix"), escaped_prefix) );

        po::variable_value escaped_suffix = po::variable_value(replace_escape_seq(vmSettings["suffix"].as<std::string>()), vmSettings["suffix"].defaulted());
        vmSettings.erase("suffix");
        vmSettings.insert( std::make_pair( std::string("suffix"), escaped_suffix) );
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }

    // Loop through files
    for (auto &f : vmSettings["input-file"].as< vector<string> >())
    {
        process_file(f, vmSettings);
    }

    return 0;
}

void process_file( boost::filesystem::path infile, po::variables_map& settings)
{
    char ch1, ch2;
    boost::filesystem::path outfile;
    std::string prefix = settings["prefix"].as<std::string>(),
                suffix = settings["suffix"].as<std::string>();

    bool convert_comments;
    if ( settings["comments"].as<std::string>() == "convert" ) convert_comments = true;
    else convert_comments = false;

    if (settings.count("out-ext"))
    {
        std::string out_ext = std::string(".")+settings["out-ext"].as<std::string>();
        outfile = boost::filesystem::path(settings["out-dir"].as<std::string>()) / (infile.stem().string()+out_ext);
    }
    else
    {
        outfile = boost::filesystem::path(settings["out-dir"].as<std::string>()) / infile.filename();
    }

    FILE *fin = fopen(infile.string().c_str(),"r"),
         *fout = fopen(outfile.string().c_str(),"w");

    //const char *prefix = "", *suffix = "\nGO\n";
    bool wrote_prefix = false;

    // Initialise state machine
    ch1 = fgetc(fin);
    ch2 = fgetc(fin);

    while (true) {
        // Handle comments
        if (ch1 == '-' && ch2 == '-')
        {
            ch1 = fgetc(fin); ch2 = fgetc(fin); // Read past "--" characters

            bool space_flag = isspace(ch1); // Comment starts with space?

            if (convert_comments) fputs("/*", fout);

            while (ch1 != '\n' && ch1 != EOF)
            {
                if (convert_comments) fputc(ch1, fout);

                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            if (convert_comments)
            {
                if (space_flag) fputc(' ', fout); // If started with space, end with one before the closing "*/"
                fputs("*/", fout);
            }

            continue;
        }

        if (ch1 == '/' && ch2 == '*')
        {
            while ( !(ch1 == '*' && ch2 == '/') && ch1 != EOF )
            {
                if (convert_comments) fputc(ch1, fout);

                ch1 = ch2;
                ch2 = fgetc(fin);
            }

            if (convert_comments) fputs("*/", fout);

            ch1 = fgetc(fin);
            ch2 = fgetc(fin);
            continue;
        }

        // Handle quotes
        if (ch1 == '\'' || ch1 == '"' || ch1 == '`' || ch1 == '[')
        {
            bool wrote_closing_quote = false;
            char closing_quote;

            if (ch1 == '[') closing_quote = ']'; else closing_quote = ch1;

            // Write opening quote & move cursor
            fputc(ch1, fout);
            ch1 = ch2;
            ch2 = fgetc(fin);

            while (true)
            {
                fputc(ch1, fout);
                if (ch1 == closing_quote) wrote_closing_quote = true;

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
                fprintf(fout, "%s", suffix.c_str());
                wrote_prefix = false; // Reset prefix flag
            }
            else
            {
                if (!wrote_prefix && !isspace(ch1))
                {
                    fprintf(fout, "%s", prefix.c_str());
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
}
