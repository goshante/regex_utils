#include "reutils.h"
#include <regex>
#include <stdexcept>

namespace ReUtils
{
	void ReplaceStrIndexRange(std::string& str, const std::string& replacingStr, size_t begin, size_t end)
	{
		size_t len = str.length();
		if (len < 2)
			return;

		if (begin > end)
		{
			size_t t;
			t = begin;
			begin = end;
			end = t;
		}

		if (begin < 0 || end >= len)
			throw std::runtime_error("ReUtils::ReplaceStrIndexRange - index out of range.");

		if (begin == end)
		{
			if (begin != 0)
				begin--;
			else
				end++;
		}

		std::string pre, post;
		pre = str.substr(0, begin);
		post = str.substr(end, len - end + 1);
		str = pre + replacingStr + post;
	}

	std::string IndexSubstr(const std::string& str, size_t begin, size_t end)
	{
		return str.substr(begin, end - begin + 1);
	}

	void PatternReplace(std::string& str, size_t begin, size_t end, const std::string pattern, const std::vector<std::string>& groups)
	{
		std::string insertion;
		auto isInt = [](const std::string& str) -> bool
		{
			std::string nums = "0123456789";
			
			for (size_t i = 0; i < str.length(); i++)
			{
				if (nums.find(str[i]) == std::string::npos)
					return false;
			}

			return true;
		};

		auto atou = [](const char *a) -> size_t
		{
			size_t u = 0;
			size_t t(1);

			for (int i = strlen(a) - 1; i >= 0; --i)
			{
				if (a[i] >= '0' && a[i] <= '9')
				{
					u += (a[i] - '0') * t;
					t *= 10;
				}
			}

			return u;
		};

		bool parsingFlag = false;
		bool bracketOpened = false;
		std::string sNum;
		size_t iNum;
		for (size_t i = 0; i < pattern.size(); i++)
		{
			if (!parsingFlag)
			{
				if (bracketOpened)
				{
					if (isInt(sNum))
					{
						iNum = atou(sNum.c_str());

						if (iNum == 0)
							insertion += IndexSubstr(str, begin, end);
						else
							insertion += groups[iNum - 1]; 
					}
					sNum = "";
					bracketOpened = false;
				}
				if ((pattern[i] != '$') || (pattern[i] == '$' && i > 0 && pattern[i - 1] == '\\'))
					insertion.push_back(pattern[i]);
				else
					parsingFlag = true;
			}
			else
			{
				if (pattern[i - 1] == '$' && pattern[i] == '[' && !bracketOpened)
				{
					bracketOpened = true;
					sNum = "";
					continue;
				}

				if (!bracketOpened)
				{
					parsingFlag = false;
					bracketOpened = false;
					sNum.push_back(pattern[i]);
				}
				else
				{
					if (pattern[i] == ']')
						parsingFlag = false;
					else
						sNum.push_back(pattern[i]);
				}
			}
		}

		ReplaceStrIndexRange(str, insertion, begin, end);
	}

	//match
	match::match(std::string& str, size_t begin, size_t end, const std::vector<std::string>& groups, const std::vector<grp_range>& granges)
		: _str(&str)
		, _begin(begin)
		, _end(end)
		, _groups(groups)
		, _granges(granges)
	{
	}

	match::match(const match& copy)
		: _str(copy._str)
		, _begin(copy._begin)
		, _end(copy._end)
		, _groups(copy._groups)
		, _granges(copy._granges)
	{
	}

	match& match::operator=(const match& copy)
	{
		_str = copy._str;
		_begin = copy._begin;
		_end = copy._end;
		_groups = copy._groups;
		_granges = copy._granges;
		return *this;
	}

	void match::Replace(const std::string& pattern)
	{
		if (IsMatching())
			PatternReplace( *_str, _begin, _end, pattern, _groups );
	}

	std::string match::operator[](size_t i) const
	{
		return GetGroup(i);
	}

	size_t match::Begin() const
	{
		return _begin;
	}

	size_t match::End() const
	{
		return _end;
	}

	std::string match::GetMatchingString() const
	{
		if (!IsMatching())
			return "";

		return IndexSubstr(*_str, _begin, _end);
	}

	std::string match::GetGroup(size_t n) const
	{
		if (n == 0)
			return GetMatchingString();
		if (n > _groups.size())
			return "";
		return _groups[n-1];
	}

	match::grp_range match::GetGroupRange(size_t n)
	{
		if (n == 0)
			return { _begin, _end };

		if (n > _granges.size())
			return { std::string::npos, std::string::npos };

		return _granges[n-1];
	}

	bool match::IsMatching() const
	{
		return (_begin != std::string::npos);
	}

	//matches
	matches::matches()
	{
	}

	matches::matches(const matches& copy)
		: _mvec(copy._mvec)
	{
	}

	matches& matches::operator=(const matches& copy)
	{
		_mvec = copy._mvec;
		return *this;
	}

	matches& matches::operator+=(const match& m)
	{
		_mvec.push_back(m);
		return *this;
	}

	match& matches::operator[](size_t i)
	{
		return _mvec.at(i);
	}

	void matches::AddMatch(const match& m)
	{
		_mvec.push_back(m);
	}

	void matches::ReplaceAll(const std::string& pattern)
	{
		for (auto& m : _mvec)
			m.Replace(pattern);
	}

	match matches::Get(size_t i) const
	{
		return _mvec.at(i);
	}

	size_t matches::Count() const
	{
		return _mvec.size();
	}

	bool matches::HasMatches() const
	{
		if (_mvec.size() == 0)
			return false;
		return true;
	}

	//Utils
	match Search(std::string& str, const std::string re, size_t offset)
	{
		std::string a;
		std::regex exp(re);
		std::smatch sm;
		std::vector<std::string> groups;
		std::vector<match::grp_range> ranges;
		size_t begin = std::string::npos;
		size_t end = std::string::npos;
		std::string offstr = "";
		std::string* pstr = &str;

		auto countGroupRange = [](const std::smatch::value_type& g, const std::string& str, size_t offset) -> match::grp_range
		{
			size_t i = 0;
			int n = 0;
			match::grp_range range = { std::string::npos, std::string::npos };
			for (auto it = str.begin(); it != str.end(); it++)
			{
				if (it == g.first)
				{
					range.begin = i;
					n++;
				}

				if (it == g.second)
				{
					range.end = i;
					n++;
				}

				if (n == 2)
					break;
				i++;
			}

			if (range.begin != std::string::npos && range.end == std::string::npos)
				range.end = str.length() - 1;

			if (offset > 0)
			{
				range.begin += offset;
				range.end += offset;
			}

			return range;
		};

		if (offset > 0)
		{
			offstr = str.substr(offset, str.length() - offset);
			pstr = &offstr;
		}

		if (offset >= str.length())
			return match(str, begin, end, groups, ranges);

		if (std::regex_search(*pstr, sm, exp)) 
		{
			for (size_t i = 0; i < sm.size(); i++)
			{
				if (i > 0)
				{
					groups.push_back(sm[i]);
					ranges.push_back(countGroupRange(sm[i], *pstr, offset));
				}
				else
				{
					auto range = countGroupRange(sm[i], *pstr, offset);
					begin = range.begin;
					end = range.end;
				}
			}

		}

		return match(str, begin, end, groups, ranges);
	}

	matches SearchAll(std::string& str, const std::string re, size_t offset)
	{
		matches ms;
		match m = Search(str, re, offset);

		if (!m.IsMatching())
			return ms;
	
		do
		{
			ms += m;
			offset = m.End() + 1;
			m = Search(str, re, offset);
		} while (m.IsMatching());

		return ms;
	}

	bool IsMatching(const std::string& str, std::string re)
	{
		return std::regex_match(str, std::regex(re));
	}

	size_t ReplaceAll(std::string& str, const std::string& re, const std::string& pattern)
	{
		size_t off = 0;
		size_t replaces = 0;
		match m = Search(str, re, off);

		while (m.IsMatching())
		{
			m.Replace(pattern);
			replaces++;
			off = m.End() + 1;
			m = Search(str, re, off);
		}

		return replaces;
	}

} //namespace ReUtils