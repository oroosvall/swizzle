#include "Parser.hpp"

#include "ScriptHelperFuncs.hpp"

namespace script
{

	std::vector<Token> Parse(langString_t str)
	{
		uint32_t lineCounter = 0U;
		uint32_t columnCounter = 0U;

		std::vector<Token> tokens;

		bool inString = false;
		bool isNum = false;

		size_t mStrSize = str.size();

		langString_t processed;

		for (size_t i = 0; i < mStrSize; i++)
		{
			char c = str[i];
			columnCounter++;

			if (inString)
			{
				switch (c)
				{
				case '\"':
				{
					Token t;
					t.mLineNumber = lineCounter;
					t.mColumn = columnCounter;
					t.mToken = processed.c_str();
					tokens.push_back(t);
					processed = "";
					break;
				}
				default:
					processed += c;
					break;
				}
			}
			else
			{
				switch (c)
				{
				case ' ':
				case '\t':
				{
					if (processed.size() != 0)
					{
						Token t;
						t.mLineNumber = lineCounter;
						t.mColumn = columnCounter;
						t.mToken = processed.c_str();
						tokens.push_back(t);
						processed = "";
						isNum = false;
					}
					break;
				}
				case '(':
				case ')':
				case '[':
				case ']':
				case '{':
				case '}':
				case ';':
				case ',':
				{
					Token t;
					t.mLineNumber = lineCounter;
					t.mColumn = columnCounter;
					if (processed.size() != 0)
					{
						t.mToken = processed.c_str();
						tokens.push_back(t);
						isNum = false;
					}
					t.mToken = "";
					t.mToken += c;
					tokens.push_back(t);
					processed = "";
					break;
				}
				case '.':
				{
					Token t;
					t.mLineNumber = lineCounter;
					t.mColumn = columnCounter;
					if (processed.size() != 0)
					{
						if (isNumber(processed.c_str()))
						{
							processed += c;
							isNum = true;
							break;
						}
						t.mToken = processed.c_str();
						tokens.push_back(t);
						isNum = false;
					}
					t.mToken = "";
					t.mToken += c;
					tokens.push_back(t);
					processed = "";
					isNum = false;
					break;
				}
				case '+':
				case '-':
				case '&':
				case '|':
				case '=':
				case '%':
				case '/':
				case '*':
				{
					Token t;
					t.mLineNumber = lineCounter;
					t.mColumn = columnCounter;
					if (processed.size() != 0)
					{
						t.mToken = processed.c_str();
						processed = "";
						tokens.push_back(t);
						isNum = false;
					}
					processed += c;
					if (str[i + 1] == c)
					{
						processed += c;
						i++;
					}
					else if (str[i + 1] == '=')
					{
						processed += '=';
						i++;
					}

					t.mToken = processed.c_str();
					processed = "";
					tokens.push_back(t);
					isNum = false;
					break;
				}
				case '<':
				case '>':
				{
					Token t;
					t.mLineNumber = lineCounter;
					t.mColumn = columnCounter;
					if (processed.size() != 0)
					{
						t.mToken = processed.c_str();
						processed = "";
						tokens.push_back(t);
						isNum = false;
					}
					processed += c;
					if (str[i + 1] == c)
					{
						processed += c;
						i++;
						if (str[i + 1] == '=')
						{
							processed += c;
							i++;
						}
					}
					else if (str[i + 1] == '=')
					{
						processed += c;
						i++;
					}

					t.mToken = processed.c_str();
					processed = "";
					tokens.push_back(t);
					isNum = false;
					break;
				}
				case '^':
				case '!':
				{
					Token t;
					t.mLineNumber = lineCounter;
					t.mColumn = columnCounter;
					if (processed.size() != 0)
					{
						t.mToken = processed.c_str();
						processed = "";
						tokens.push_back(t);
						isNum = false;
					}
					processed += c;
					if (str[i + 1] == '=')
					{
						processed += '=';
						i++;
					}
					t.mToken = processed.c_str();
					processed = "";
					tokens.push_back(t);
					isNum = false;
					break;
				}
				case '\"':
				{
					Token t;
					t.mLineNumber = lineCounter;
					t.mColumn = columnCounter;
					if (processed.size() != 0)
					{
						t.mToken = processed.c_str();
						tokens.push_back(t);
					}
					processed = "";
					processed += c;
					break;
				}
				case '\n':
					lineCounter++;
					columnCounter = 0U;
					break;
				default:
					if (isNum)
					{
						langString_t s = "";
						s += c;
						if (isNumber2(s))
						{
							processed += s;
						}
						else
						{
							Token t;
							t.mToken = processed.c_str();
							tokens.push_back(t);
							processed = "";
							processed += c;
							isNum = false;
						}
					}
					else
					{
						processed += c;
					}
					break;
				}
			}
		}

		return tokens;
	}

}