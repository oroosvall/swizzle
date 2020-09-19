#include <utils/Config.hpp>

#include <fstream>

namespace utils
{
    /// Value
    Value::Value()
        : mValue("")
    {
    }

    Value::~Value() {}

    void Value::setString(std::string value)
    {
        mValue = value;
    }

    void Value::setInt(int32_t value)
    {
        mValue = std::to_string(value);
    }

    void Value::setFloat(float value)
    {
        mValue = std::to_string(value);
    }

    void Value::setBool(bool value)
    {
        mValue = value ? "true" : "false";
    }

    std::string Value::getString() const
    {
        return mValue;
    }

    int32_t Value::getInt() const
    {
        return std::atoi(mValue.c_str());
    }

    float Value::getFloat() const
    {
        return static_cast<float>(std::atof(mValue.c_str()));
    }

    bool Value::getBool() const
    {
        bool val = false;
        if (mValue == "true")
        {
            val = true;
        }
        return val;
    }

    /// Config
    Config::Config()
        : mValues()
    {
    }

    Config::Config(const char* file)
        : mValues()
    {
        read(file);
    }

    Config::~Config() {}

    Value Config::getValue(std::string name, std::string defaultValue) const
    {
        Value val;
        val.setString(defaultValue);
        getValue(name, val);
        return val;
    }

    Value Config::getValue(std::string name, int32_t defaultValue) const
    {
        Value val;
        val.setInt(defaultValue);
        getValue(name, val);
        return val;
    }

    Value Config::getValue(std::string name, float defaultValue) const
    {
        Value val;
        val.setFloat(defaultValue);
        getValue(name, val);
        return val;
    }

    Value Config::getValue(std::string name, bool defaultValue) const
    {
        Value val;
        val.setBool(defaultValue);
        getValue(name, val);
        return val;
    }

    void Config::setValue(std::string name, const Value& value)
    {
        bool added = false;

        for (auto it : mValues)
        {
            if (it.first == name)
            {
                it.second = value;
                added = true;
            }
        }

        if (!added)
        {
            mValues.push_back(std::make_pair(name, value));
        }
    }

    void Config::read(const char* file)
    {
        mValues.clear();

        std::ifstream inFile(file);

        if (inFile.is_open())
        {

            std::string line;
            std::getline(inFile, line);

            if (!line.empty())
            {

                size_t indexOfEq = line.find("=");
                std::string firstPart = line.substr(0, indexOfEq);
                std::string lastPart = line.substr(indexOfEq + 1);

                Value v;
                v.setString(lastPart);
                mValues.push_back(std::make_pair(firstPart, v));
            }
        }

        inFile.close();
    }

    void Config::write(const char* file)
    {
        std::ofstream outFile(file);

        for (auto it : mValues)
        {

            auto name = it.first;
            Value v = it.second;

            outFile << name;
            outFile << "=";
            outFile << v.getString();
            outFile << "\n";
        }

        outFile.close();
    }

    void Config::getValue(std::string name, Value& value) const
    {
        for (auto it : mValues)
        {
            if (it.first == name)
            {
                value = it.second;
            }
        }
    }

} // namespace utils
