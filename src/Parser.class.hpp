#ifndef PARSER_HPP
#define PARSER_HPP

#include "Array.class.hpp"
#include "FilePosition.struct.hpp"
#include "Puzzle.class.hpp"

#include <fstream>
#include <string>

typedef unsigned int uint;

class Parser
{
	private:
	const char *  _file;
	std::ifstream _fileStream;
	std::istream &_stream;

	Puzzle      _puzzle;
	Array<bool> _hashTable;

	uint         _rows;
	FilePosition _pos;

	public:
	Parser() = delete;
	Parser(const char *file = nullptr);
	Parser(const Parser &) = delete;
	Parser &operator=(const Parser &) = delete;
	Parser(Parser &&) = delete;
	Parser &operator=(Parser &&) = delete;
	~Parser();

	void          isDigitStr(const std::string &str, uint &index_end);
	uint          strtou(const std::string &str, size_t *index = nullptr, uint max = std::numeric_limits<uint>::max(),
	                     uint min = 0);
	void          setPuzzleSize(const std::string &line, uint &i);
	void          setPuzzleRow(const std::string &line, uint &i);
	void          parseFromStream(std::string &line);
	std::istream &openFile();
	void          closeFile();
	void          parse();
	Puzzle &      getPuzzle();
};

#endif // !PARSER_HPP
