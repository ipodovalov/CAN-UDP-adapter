// Read an INI file into easy-to-access name/value pairs.

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include "ini.h"
#include "inireader.h"

using std::string;

INIReader::INIReader(string filename)
{
    _error = ini_parse(filename.c_str(), valueHandler, this);
}

int INIReader::parseResult()
{
    return _error;
}

string INIReader::get(string section, string name, string default_value)
{
    string key = makeKey(section, name);
    return _values.count(key) ? _values[key] : default_value;
}

int64_t INIReader::getInt(string section, string name, int64_t default_value)
{
    string valstr = get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    int64_t n = strtoll(value, &end, 0);
    return end > value ? n : default_value;
}

uint64_t INIReader::getUInt(string section, string name, uint64_t default_value)
{
    string valstr = get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    uint64_t n = strtoull(value, &end, 0);
    return end > value ? n : default_value;
}

double INIReader::getFloat(string section, string name, double default_value)
{
    string valstr = get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}

bool INIReader::getBool(string section, string name, bool default_value)
{
    string valstr = get(section, name, "");
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
}

bool INIReader::hasKey(std::string section, std::string name) {
    string key = makeKey(section, name);
    return _values.count(key) ? true : false;
}

string INIReader::makeKey(string section, string name)
{
    string key = section + "=" + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::valueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    INIReader* reader = (INIReader*)user;
    string key = makeKey(section, name);
    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";
    reader->_values[key] += value;
    return 1;
}
