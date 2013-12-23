#ifndef HUFFMANZIP_H
#define HUFFMANZIP_H

#include <vector>
#include <string>
#include <fstream>

#include "HuffmanTree.h"
#include "pq_min.h"

class HuffmanZip
{
public:
	HuffmanZip(size_t maxAlphabetSize = 256);
	~HuffmanZip();

	std::string encode(const char* filename);
	std::string decode(const char* filename);

private:
	HuffmanTree* _tree;
	size_t _maxAlphabetSize;
	std::string _filename;

	bool countFreqs(std::vector<int>* freqs);
	void encodeContents(std::ifstream& ifs, std::ofstream& ofs) const;
	std::string getHzipFilename(std::string extension = ".hzip");

	HuffmanZip(const HuffmanZip& hz);
	HuffmanZip& operator= (const HuffmanZip& hz);
};

class FileNotFoundException : public std::exception
{
public:
	FileNotFoundException(const char* what)
		: _what(what)
	{ }

	FileNotFoundException(const std::string& what)
		: _what(what)
	{ }

	const char* what() const 
	{
		return _what.c_str();
	}

private:
	std::string _what;
};

class FileIsEmptyException : public std::exception
{
public:
	FileIsEmptyException(const char* what)
		: _what(what)
	{ }

	FileIsEmptyException(const std::string& what)
		: _what(what)
	{ }

	const char* what() const
	{
		return _what.c_str();
	}

private:
	std::string _what;
};

#endif // HUFFMANZIP_H