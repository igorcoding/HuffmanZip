#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <fstream>

typedef uint8_t byte;
typedef uint16_t dbyte;

class HuffmanTree
{
	struct Node
	{
		std::shared_ptr<Node> left;
		std::shared_ptr<Node> right;
		int letter;
		int freq;

		Node(int l, int frequency = -1)
			: letter(l),
			  freq(frequency),
			  left(nullptr),
			  right(nullptr)
		{ }

		~Node()
		{
			freq = -1;
			letter = -1;
		}
		static int NoLetter;
	};

	struct NodeComparator
	{
		bool operator() (const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2)
		{
			return n1->freq < n2->freq;
		}
	};

public:
	HuffmanTree();
	~HuffmanTree();

	std::map<int, std::string> getCodes() const;
	void build(const std::vector<int>& freqs);
	void getTree(std::ofstream& ofs) const;
	
	void buildFromBytes(const std::vector<byte>& input, const std::vector<byte>& alphabet);
	void decode(std::ifstream& input, byte extraBitsCount, std::ofstream& ofs);
	
private:
	std::shared_ptr<Node> _root;
	std::map<int, std::string> _codes;
	unsigned long long _bitsInText;

	void countLength();
	int frequency(const std::string& code);
	void findCodes();
	void findCodes(const std::shared_ptr<Node>& node, std::string code);
	std::shared_ptr<Node> merge(const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2);

	void DfsCodes(const std::shared_ptr<Node>& node, std::string& res, std::vector<int>& alphabet) const;
	dbyte countBytes(const std::string& str) const;
	void transferString(const std::string& str, std::ofstream& ofs) const;
	
	

	HuffmanTree(const HuffmanTree& ht);
	HuffmanTree& operator= (const HuffmanTree& ht);
};

#endif // HUFFMANTREE_H