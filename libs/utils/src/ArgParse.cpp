
#include <utils/ArgParse.hpp>

namespace utils
{
    ArgOption::ArgOption(const char* option, const char* description, ArgType type)
        : mOption(option)
        , mDescription(description)
        , mType(type)
        , mIsSet(false)
        , mValues()
    {
    }

    ArgParser::ArgParser(const char* appInfo)
        : mAppInfo(appInfo)
        , mOptions()
    {
    }

    // This code is ugly and needs comments
    bool ArgParser::parse(int argv, char** argc)
    {
        // just some states
        bool processed = false;
        ArgOption* option = nullptr; // keep track of last found option incase it is supposed to be a list

        for (int i = 1; i < argv; i++)
        {
            processed = true;
            char* str = argc[i];
            // Lazy but check if first element is beginning of an argument .. force that arguments start with -
            if (str[0] == '-')
            {
                for (ArgOption* o : mOptions)
                {
                    if (strcmp(o->mOption, str) == 0)
                    {
                        // ignore option if it occurs multiple times
                        if (!o->mIsSet)
                        {
                            o->mIsSet = true;
                            // if not flag remember fouund option else set it to nullptr
                            if (o->mType != ArgType::Flag)
                            {
                                option = o;
                            }
                            else
                            {
                                option = nullptr;
                            }
                        }
                        else
                        {
                            printf("option '%s' has been previously set ignoring\n", str);
                        }
                    }
                }
            }
            else
            {
                // store the value
                if (option)
                {
                    option->mValues.push_back(str);
                    // clear pointer if the type is a single value
                    if (option->mType == ArgType::Value)
                    {
                        option = nullptr;
                    }
                }
                else
                {
                    printf("Unrecognized option '%s'\n", str);
                }
            }
        }

        return processed;
    }

    void ArgParser::printHelp()
    {
        printf("%s\n", mAppInfo);

        char* type = "";

        for (const ArgOption* o : mOptions)
        {
            if (o->mType == ArgType::Flag)
            {
                type = "FLAG";
            }
            else if (o->mType == ArgType::List)
            {
                type = "LIST";
            }
            else if (o->mType == ArgType::Value)
            {
                type = "VALUE";
            }

            printf("\t %s [%s] : %s \n", o->mOption, type, o->mDescription);
        }
    }

    void ArgParser::addOption(ArgOption* option)
    {
        mOptions.push_back(option);
    }

} // namespace utils