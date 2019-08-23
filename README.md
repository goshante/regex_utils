# regex_utils
Wrapper for std::regex library to simplify work with regular expressions.

# Description
## Searching  
`match Search(std::string& str, const std::string re, size_t offset = 0);`  
### Purpose:
To search closest match by regular expression in string.
### Takes:  
* `std::string& str` - input text to search.
* `const std::string re` - regular expression string.
* `size_t offset` - Offset in input string to skip some part of string. [optional, 0 by default]
### Returns:  `match` - Container with your match. It has matching indexes, groups and all commonly used regular expression features.
  
`matches SearchAll(std::string& str, const std::string re, size_t offset = 0);` 
### Purpose:
To search all matches by regular expression in string.
### Takes:  
* `std::string& str` - input text to search.
* `const std::string re` - regular expression string.
* `size_t offset` - Offset in input string to skip some part of string. [optional, 0 by default]
### Returns:  `matches` - Container multiple `match` instances.
  
`bool IsMatching(const std::string& str, std::string re);` 
### Purpose:
Checks does input string matching expression or not.
### Takes:  
* `std::string& str` - input text to search.
* `const std::string re` - regular expression string.
### Returns:  `bool` - Returns true if matching, otherwise returns false.

`size_t ReplaceAll(std::string& str, const std::string& re, const std::string& pattern);` 
### Purpose:
Replaces all matches in string by Replace Pattern.
### Takes:  
* `std::string& str` - input text to search.
* `const std::string re` - regular expression string.
* `const std::string& pattern` - replace pattern.
### Returns:  `size_t` - Count of replaces.
  
## Replace Patterns
This is a string which replaces matching range of your regular expression. It can be just any text, for example qweqwe and your match will be replaced with qweqwe.  
It also can inline matching groups by indexes. To insert group just write in your pattern `$[<group number>]`. For example: qwe$[2]qwe$[5]asdf.
Your entire match will be replaced with qwe$[2]qwe$[5]asdf where $[2] and $[5] are 2 and 5 groups of your expression's match.

## match
todo
