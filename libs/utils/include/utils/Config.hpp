#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace utils
{
    class Value
    {
    public:
        Value();
        ~Value();

        void setString(std::string value);
        void setInt(int32_t value);
        void setFloat(float value);
        void setBool(bool value);

        std::string getString() const;
        int32_t getInt() const;
        float getFloat() const;
        bool getBool() const;

    private:
        std::string mValue;
    };

    class Config
    {
    public:
        Config();
        Config(const char* file);

        ~Config();

        Value getValue(std::string name, std::string defaultValue) const;
        Value getValue(std::string name, int32_t defaultValue) const;
        Value getValue(std::string name, float defaultValue) const;
        Value getValue(std::string name, bool defaultValue) const;

        void setValue(std::string name, const Value& value);

        void read(const char* file);
        void write(const char* file);

    private:
        void getValue(std::string name, Value& v) const;

        std::vector<std::pair<std::string, Value>> mValues;
    };
} // namespace utils

#endif