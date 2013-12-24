#include "HuffmanZip.h"

#include <bitset>


HuffmanZip::HuffmanZip(size_t maxAlphabetSize)
	: _maxAlphabetSize(maxAlphabetSize)
{
	_tree = new HuffmanTree();
}

HuffmanZip::~HuffmanZip()
{
	delete _tree;
}

std::string HuffmanZip::encode(const char* filename)
{
	_filename = filename;

	std::ifstream ifs(_filename, std::ifstream::in | std::ifstream::binary);

	if (!ifs)
		throw FileNotFoundException("File \'" + _filename + "\' not found.\n");

	std::vector<int> freqs(_maxAlphabetSize);
	bool goodFile = countFreqs(&freqs);

	if (!goodFile)
		throw FileIsEmptyException("File \'" + _filename + "\' is empty.\n");

	_tree->build(freqs);
	
	auto hzipped = getHzipFilename();
	std::ofstream ofs(hzipped, std::ofstream::out | std::ofstream::binary);

	_tree->getTree(ofs);
	encodeContents(ifs, ofs);

	ofs.close();
	return hzipped;
}

bool HuffmanZip::countFreqs(std::vector<int>* freqs)
{
	bool good = true;
	int n = 0;
	int readBytes = 0;

	std::ifstream ifs;
	ifs.open(_filename, std::ifstream::in | std::ifstream::binary);

	if (!ifs)
		throw FileNotFoundException("File \'" + _filename + "\' not found");

	while (ifs.good())
	{
		char sym = 0;
		ifs.get(sym);
		if (!ifs.good())
		{
			if (n == 0)
				good = false;
			break;
		}
		++readBytes;
		n = 1;
		int id = sym >= 0 ? sym : (_maxAlphabetSize + sym);
		++(*freqs)[id];
	}

	ifs.close();
	return good;
}

void HuffmanZip::encodeContents(std::ifstream& ifs, std::ofstream& ofs) const
{
	auto codes = _tree->getCodes();

	byte currentByte = 0;
	int n = -1;
	byte writtenBytes = 0;
	while (!ifs.eof())
	{
		char sym = 0;
		ifs.get(sym);
		if (!ifs.good())
			break;

		int id = sym >= 0 ? sym : (_maxAlphabetSize + sym);

		auto code = codes[id];
		for (size_t i = 0; i < code.length(); ++i)
		{
			if (code[i] == '0')
			{
				++n;
			}
			else if (code[i] == '1')
			{
				currentByte |= (1 << (7-(n+1)));
				++n;
			}
			else
				throw std::exception();

			if (n >= 7)
			{
				ofs.write(reinterpret_cast<const char*>(&currentByte), sizeof (currentByte));
				currentByte = 0;
				n = -1;
				++writtenBytes;
			}
		}
	}
	if (n < 8 && n != -1)
	{
		ofs.write(reinterpret_cast<const char*>(&currentByte), sizeof (currentByte));
		currentByte = 0;
		n = -1;
		++writtenBytes;
	}

	ifs.close();
}

std::string HuffmanZip::decode(const char* filename)
{
	_filename = filename;
	delete _tree;
	_tree = new HuffmanTree();

	std::ifstream ifs(_filename, std::ifstream::in | std::ifstream::binary);

	if (!ifs)
		throw FileNotFoundException("File \'" + _filename + "\' not found");

	dbyte N; // 1 byte
	ifs.read(reinterpret_cast<char*>(&N), 2);

	if (!ifs.good())
		throw FileIsEmptyException("File \'" + _filename + "\' is empty.\n");

	std::vector<byte> alphabet; // N bytes
	for (auto n = 0; n < N; ++n)
	{
		char letter;
		ifs.get(letter);
		alphabet.push_back(letter);
	}

	dbyte L; // 2 bytes
	ifs.read(reinterpret_cast<char*>(&L), 2);

	std::vector<byte> tree(L); // L bytes
	for (auto j = 0; j < L; ++j)
	{
		char oneByte = 0;
		ifs.get(oneByte);
		tree[j] = oneByte;
	}

	char extraBits;
	ifs.get(extraBits);

	_tree->buildFromBytes(tree, alphabet);

	auto hzipped = getHzipFilename(".decoded");
	std::ofstream ofs(hzipped, std::ofstream::out | std::ofstream::binary);

	_tree->decode(ifs, static_cast<byte>(extraBits), ofs);

	ofs.close();
	return hzipped;
}

std::string HuffmanZip::getHzipFilename(std::string extension)
{
	return _filename + extension;
}