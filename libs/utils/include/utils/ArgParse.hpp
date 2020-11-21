#ifndef ARG_PARSE_HPP
#define ARG_PARSE_HPP

#include <vector>

namespace utils
{
    enum class ArgType
    {
        Flag,
        Value,
        List,
    };

    class ArgOption
    {
    public:
        ArgOption(const char* option, const char* description, ArgType type);

        const char* mOption;
        const char* mDescription;

        ArgType mType;

        bool mIsSet;
        std::vector<char*> mValues;
    };

    class ArgParser
    {
    public:
        ArgParser(const char* appInfo);

        bool parse(int argv, char** argc);
        void printHelp();

        void addOption(ArgOption* option);

    private:
        const char* mAppInfo;
        std::vector<ArgOption*> mOptions;
    };

} // namespace utils

#endif