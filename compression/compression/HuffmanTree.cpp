#include "HuffmanTree.h"
#include "pq_min.h"

#include <stack>
#include <cstdint>

int HuffmanTree::Node::NoLetter = -1;

HuffmanTree::HuffmanTree()
	: _bitsInText(0)
{
}

HuffmanTree::~HuffmanTree()
{
}

std::map<int, std::string> HuffmanTree::getCodes() const
{
	return _codes;
}

void HuffmanTree::build(const std::vector<int>& f)
{
	pq_min<std::shared_ptr<Node>, NodeComparator> freqs(f.size());
	
	for (size_t i = 0; i < f.size(); ++i)
	{
		if (f[i] != 0)
			freqs.insert(std::make_shared<Node>(i, f[i]));
	}
	if (freqs.size() == 1)
	{
		_root = std::make_shared<Node>(Node::NoLetter, freqs.min()->freq);
		_root->left = freqs.min();
		freqs.del_min();
	}
	else
	{
		while (freqs.size() != 1)
		{
			auto n1 = freqs.min(); freqs.del_min();
			auto n2 = freqs.min(); freqs.del_min();

			auto n1n2 = merge(n1, n2);
			freqs.insert(n1n2);
		}

		_root = freqs.min();
		freqs.del_min();
	}
	findCodes();
	countLength();
}

void HuffmanTree::countLength()
{
	for (auto code : _codes)
		_bitsInText += code.second.length() * frequency(code.second);
}

int HuffmanTree::frequency(const std::string& code)
{
	auto node = _root;
	for (auto& ch : code)
	{
		if (node->letter == Node::NoLetter)
		{
			if (ch == '0')
			{
				node = node->left;
			}
			else if (ch == '1')
			{
				node = node->right;
			}
		}			
	}
	return node->freq;
}

void HuffmanTree::findCodes()
{
	findCodes(_root, "");
}

void HuffmanTree::findCodes(const std::shared_ptr<Node>& node, std::string code)
{
	if (node == nullptr)
		return;
	findCodes(node->left, code + "0");

	// processing
	if (node->letter != Node::NoLetter)
		_codes[node->letter] = code;

	findCodes(node->right, code + "1");
}

std::shared_ptr<HuffmanTree::Node> HuffmanTree::merge(const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2)
{
	auto n = std::make_shared<Node>(Node::NoLetter, n1->freq + n2->freq);
	n->left = n1;
	n->right = n2;
	return n;
}


// Format: N|alphabet|L|tree
// N: number of charaters in alphabet = 2 bytes
// alphabet: N characters = N bytes
// L: length of the tree = 2 bytes
// tree: DFS tree representation = L bytes
// extraBits: extra bits that don't fit into bytes = 1 byte
void HuffmanTree::getTree(std::ofstream& ofs) const 
{
	auto N = static_cast<dbyte>(_codes.size());
	ofs.write(reinterpret_cast<const char*>(&N), sizeof(N));

	std::string tree;
	std::vector<int> alphabet_temp;
	DfsCodes(_root, tree, alphabet_temp);
	
	for (byte letter : alphabet_temp)
		ofs.write(reinterpret_cast<const char*>(&letter), sizeof(letter));

	auto L = countBytes(tree);
	ofs.write(reinterpret_cast<const char*>(&L), sizeof(L));

	transferString(tree, ofs);

	byte extraBits = _bitsInText % 8;
	ofs.write(reinterpret_cast<const char*>(&extraBits), sizeof (extraBits));
}

void HuffmanTree::DfsCodes(const std::shared_ptr<Node>& node, std::string& res, std::vector<int>& alphabet) const
{
	if (node == nullptr)
		return;
	DfsCodes(node->left, res += "0", alphabet);
	if (node->letter != Node::NoLetter)
		alphabet.push_back(node->letter);
	res.pop_back();
	DfsCodes(node->right, res += "1", alphabet);
}

dbyte HuffmanTree::countBytes(const std::string& str) const
{
	dbyte count = 0;
	byte n = 0;
	for (auto i : str)
	{
		if (i == '0' || i == '1')
		{
			++n;
		}
		else
			throw std::exception();

		if (n >= 8)
		{
			n = 0;
			++count;
		}
	}
	if (n < 8)
		++count;
	return count;
}

void HuffmanTree::transferString(const std::string& str, std::ofstream& ofs) const
{
	byte currentByte = 0;
	byte n = 0;
	for (auto i : str)
	{
		if (i == '0')
		{
			++n;
		}
		else if (i == '1')
		{
			currentByte |= (1 << (7-n));
			++n;
		}
		else
			throw std::exception();

		if (n >= 8)
		{
			ofs.write(reinterpret_cast<const char*>(&currentByte), sizeof(currentByte));
			currentByte = 0;
			n = 0;
		}
	}
	if (n < 8)
		ofs.write(reinterpret_cast<const char*>(&currentByte), sizeof(currentByte));
}

void HuffmanTree::buildFromBytes(const std::vector<byte>& input, const std::vector<byte>& alphabet)
{
	size_t i = 0;

	std::stack<std::shared_ptr<Node>> nodes;
	_root = std::make_shared<Node>(Node::NoLetter);
	std::shared_ptr<Node> node = _root;

	if (alphabet.size() == 1)
	{
		_root->left = std::make_shared<Node>(*(alphabet.begin()));
	}
	else
	{
		for (const byte& currentByte : input)
		{
			for (byte n = 0; n < 8; ++n)
			{
				byte bit = ((currentByte & (1 << (7-n))) >> (7 - n));
				if (bit == 0)
				{
					node->left = std::make_shared<Node>(Node::NoLetter);
					nodes.push(node);
					node = node->left;
				}
				else if (bit == 1)
				{
					node->letter = alphabet[i++];
					while (!nodes.empty() && nodes.top()->right != nullptr)
						nodes.pop();
					if (!nodes.empty())
					{
						node = nodes.top(); nodes.pop();
						node->right = std::make_shared<Node>(Node::NoLetter);
						nodes.push(node);
						node = node->right;
					}
				}
				else
					throw std::exception();

				if (i >= alphabet.size())
					break;
			}
		}
	}

	findCodes();
}



void HuffmanTree::decode(std::ifstream& input, byte extraBitsCount, std::ofstream& ofs)
{
	std::shared_ptr<Node> node = _root;
	bool continueFlag = true;

	byte bytes = 0;

	while (!input.eof())
	{
		char currentByte = 0;
		input.get(currentByte);
		++bytes;

		byte maxBits = 8;

		char nextByte = 0;
		input.get(nextByte);
		if (input.eof())
		{
			maxBits = extraBitsCount;
		}
		else
			input.seekg(-1, input.cur); // move 1 byte back

		for (byte n = 0; n < maxBits; )
		{
			if (node->letter == Node::NoLetter)
			{
				byte bit = ((currentByte & (1 << (7-n))) >> (7 - n));
				if (bit == 0)
					node = node->left;
				else if (bit == 1)
					node = node->right;
				++n;
			}
			if (node->letter != Node::NoLetter)
			{
				auto ch = static_cast<char>(node->letter);
				ofs.write(reinterpret_cast<const char*>(&ch), sizeof(char));
				node = _root;
			}
		}
	}
}