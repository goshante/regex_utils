#pragma once

#include <string>
#include <vector>

namespace ReUtils
{
	void ReplaceStrIndexRange(std::string& str, const std::string& replacingStr, size_t begin, size_t end);
	std::string IndexSubstr(const std::string& str, size_t begin, size_t end);

	class match
	{
	private:
		size_t _begin;
		size_t _end;
		std::vector<std::string> _groups;
		std::string* _str;

		match() = delete;

	public:
		match(std::string& str, size_t begin, size_t end, const std::vector<std::string>& groups);
		match(const match& copy);
		match& operator=(const match& copy);

		void Replace(const std::string& pattern);

		size_t Begin() const;
		size_t End() const;
		std::string GetMatchingString() const;
		std::string GetGroup(size_t n) const;
		bool IsMatching() const;
	};

	class matches
	{
	private:
		std::vector<match> _mvec;

	public:
		matches();
		matches(const matches& copy);
		matches& operator=(const matches& copy);
		matches& operator+=(const match& m);
		match& operator[](size_t i);

		void AddMatch(const match& m);
		void ReplaceAll(const std::string& pattern);

		match Get(size_t i) const;
		size_t Count() const;
		bool HasMatches() const;
	};

	match Search(std::string& str, const std::string re, size_t offset = 0);
	matches SearchAll(std::string& str, const std::string re, size_t offset = 0);
	bool IsMatching(const std::string& str, std::string re);
	size_t ReplaceAll(std::string& str, const std::string& re, const std::string& pattern);
}