// Read an INI file into easy-to-access name/value pairs.

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// http://code.google.com/p/inih/

#ifndef __INIREADER_H__
#define __INIREADER_H__

#include <map>
#include <string>

#include <cstdint>

// Read an INI file into easy-to-access name/value pairs. (Note that I've gone
// for simplicity here rather than speed, but it should be pretty decent.)
class INIReader
{
public:
    // Construct INIReader and parse given filename. See ini.h for more info
    // about the parsing.
    INIReader(std::string filename);

    // Return the result of ini_parse(), i.e., 0 on success, line number of
    // first error on parse error, or -1 on file open error.
    int parseResult();

    // Get a string value from INI file, returning default_value if not found.
    std::string get(std::string section, std::string name,
                    std::string default_value);

    // Get an integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
    int64_t getInt(std::string section, std::string name, int64_t default_value);

    // Get an unsigned integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", or hex "0x4d2").
    uint64_t getUInt(std::string section, std::string name, uint64_t default_value);

    // Get a real (floating point double) value from INI file, returning
    // default_value if not found or not a valid floating point value
    // according to strtod().
    double getFloat(std::string section, std::string name, double default_value);

    // Get a boolean value from INI file, returning default_value if not found or if
    // not a valid true/false value. Valid true values are "true", "yes", "on", "1",
    // and valid false values are "false", "no", "off", "0" (not case sensitive).
    bool getBool(std::string section, std::string name, bool default_value);

    // Check if key exist
    bool hasKey(std::string section, std::string name);

private:
    int _error;
    std::map<std::string, std::string> _values;
    static std::string makeKey(std::string section, std::string name);
    static int valueHandler(void* user, const char* section, const char* name,
                            const char* value);
};

#endif  // __INIREADER_H__
