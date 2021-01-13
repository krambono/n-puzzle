#include "Parser.class.hpp"

#include "Exception.class.hpp"
#include "utils.hpp"

#include <exception>
#include <iostream>

Parser::Parser(const std::optional<std::string> &file): _file(file), _rows(0), _pos { 1, 1 }
{}

Parser::~Parser()
{
	this->closeFile();
}

std::istream &Parser::getInputStream()
{
	if (!this->_file.has_value())
		return std::cin;

	this->_fileStream.open(this->_file.value());
	if (!this->_fileStream.is_open())
		throw std::runtime_error(catArgs("File ", this->_file.value(), " cannot be openned"));
	return this->_fileStream;
}

void Parser::closeFile()
{
	if (this->_file.has_value() && this->_fileStream.is_open())
		this->_fileStream.close();
}

void Parser::isDigitStr(const std::string &str, uint &indexEnd)
{
	uint i = 0;
	uint sign = 0;

	if (str[i] == '+' || str[i] == '-')
	{
		sign = 1;
		i++;
	}
	while (i < str.length() && str[i] >= '0' && str[i] <= '9')
		i++;
	if (i == sign || (str[i] != '\0' && str[i] != '#' && !std::isspace(str[i])))
		throw Exception::ParserLight("Argument is not a valid number");
	indexEnd = i;
}

uint Parser::strtou(const std::string &str, size_t *index, uint max, uint min)
{
	unsigned long n;
	uint          indexEnd;

	this->isDigitStr(str, indexEnd);
	std::string_view nStr(str.c_str(), indexEnd);
	try
	{
		n = std::stoi(str, index);
	}
	catch (const std::out_of_range &e)
	{
		throw Exception::ParserLight(catArgs(nStr, " is greater than ", max));
	}

	if (n < min)
		throw Exception::ParserLight(catArgs(nStr, " is lower than ", min));
	if (n > max)
	{
		if (str[0] == '-')
			throw Exception::ParserLight(catArgs(nStr, " is lower than ", min));
		else
			throw Exception::ParserLight(catArgs(nStr, " is greater than ", max));
	}

	return static_cast<uint>(n);
}

void Parser::setPuzzleSize(const std::string &line, uint &i)
{
	size_t nbChars;
	uint   n;

	this->_pos.col = i;
	n = this->strtou(line.c_str() + i, &nbChars, 100, 3);
	for (i += nbChars; i < line.length() && std::isspace(line[i]); i++)
		;
	if (i != line.length() && line[i] != '#')
		throw Exception::ParserLight("Puzzle's size is required", false);
	this->_puzzle = Puzzle(n);
	this->_hashTable = Array(n * n, false);
}

void Parser::setPuzzleRow(const std::string &line, uint &i)
{
	uint   cols = 0;
	uint   n;
	uint   size = this->_puzzle.getSize();
	size_t nbChars;

	if (size == this->_rows)
		throw Exception::ParserLight("The number of rows does not match the given size", false);
	for (; cols < size; cols++)
	{
		this->_pos.col = i;
		n = this->strtou(line.c_str() + i, &nbChars, size * size - 1, 0);
		if (this->_hashTable[n])
			throw Exception::ParserLight("Number already set");
		this->_hashTable[n] = true;
		this->_puzzle.at(this->_rows, cols) = n;
		if (n == 0)
			this->_puzzle.setZeroPosition({ _rows, cols });
		for (i += nbChars; i < line.length() && std::isspace(line[i]); i++)
			;
		if (i == line.length() || line[i] == '#')
			break;
	}
	this->_pos.col = i;
	if (cols + 1 < size)
		throw Exception::ParserLight(catArgs("Not enough arguments, ", size, " numbers are required"));
	if (cols + 1 > size)
		throw Exception::ParserLight(catArgs("Too much arguments, ", size, " numbers are required"));
	this->_rows++;
}

void Parser::parseFromStream(std::istream &stream, std::string &line)
{
	uint i;

	for (; std::getline(stream, line); this->_pos.line++)
	{
		for (i = 0; i < line.length() && std::isspace(line[i]); i++)
			;
		if (i == line.length() || line[i] == '#')
			continue;
		if (this->_puzzle.getSize() == 0)
			this->setPuzzleSize(line, i);
		else
			this->setPuzzleRow(line, i);
	}
	if (this->_puzzle.getSize() == 0)
		throw Exception::ParserLight("Invalid file", false);
	if (this->_puzzle.getSize() != this->_rows)
		throw Exception::ParserLight("The number of rows does not match the given size", false);
}

void Parser::parse()
{
	std::string line;

	try
	{
		this->parseFromStream(this->getInputStream(), line);
	}
	catch (const Exception::ParserLight &e)
	{
		if (e.getAddPosition())
			throw Exception::Parser(line, this->_pos, e.what());
		throw;
	}
	this->closeFile();
}

Puzzle &Parser::getPuzzle()
{
	return this->_puzzle;
}
