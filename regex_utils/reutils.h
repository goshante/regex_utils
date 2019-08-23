#pragma once

#include <string>
#include <vector>

namespace reu
{
	void ReplaceStrIndexRange(std::string& str, const std::string& replacingStr, size_t begin, size_t end);
	std::string IndexSubstr(const std::string& str, size_t begin, size_t end);

	class match_t
	{
	public:
		struct grp_range_t
		{
			size_t begin;
			size_t end;
		};

	private:
		size_t _begin;
		size_t _end;
		std::vector<std::string> _groups;
		std::vector<grp_range_t> _granges;
		std::string* _str;

		match_t() = delete;

	public:
		match_t(std::string& str, size_t begin, size_t end,
			const std::vector<std::string>& groups, 
			const std::vector<grp_range_t>& granges);
		match_t(const match_t& copy);
		match_t& operator=(const match_t& copy);

		void Replace(const std::string& pattern);

		std::string operator[](size_t i) const;
		size_t Begin() const;
		size_t End() const;
		std::string GetMatchingString() const;
		std::string GetGroup(size_t n) const;
		grp_range_t GetGroupRange(size_t n);
		bool IsMatching() const;
	};

	class matches_t
	{
		friend matches_t SearchAll(std::string& str, const std::string re, size_t offset);

	private:
		std::vector<match_t> _mvec;

	protected:
		void AddMatch(const match_t& m);
		matches_t& operator+=(const match_t& m);

	public:
		matches_t();
		matches_t(const matches_t& copy);
		matches_t& operator=(const matches_t& copy);
		match_t& operator[](size_t i);

		void ReplaceAll(const std::string& pattern);

		std::vector<match_t>::iterator begin();
		std::vector<match_t>::iterator end();

		match_t Get(size_t i) const;
		size_t Count() const;
		bool HasMatches() const;
	};

	match_t Search(std::string& str, const std::string re, size_t offset = 0);
	matches_t SearchAll(std::string& str, const std::string re, size_t offset = 0);
	bool IsMatching(const std::string& str, std::string re);
	size_t ReplaceAll(std::string& str, const std::string& re, const std::string& pattern);
}