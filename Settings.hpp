#pragma once

#include <string>

#include <boost/optional.hpp>

class Settings {
public:
    std::string prefix, suffix, out_dir;

    boost::optional<std::string> out_ext;

    bool convert_comments = false, // convert_comments = false causes comments to be stripped
         strip_semicolons = false,
         // Whitespace options
         ws_single = false,
         ws_nonewline = false;
};
