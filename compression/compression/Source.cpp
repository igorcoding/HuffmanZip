// Haffman's things...

#include <iostream>

#include "HuffmanZip.h"


enum Command
{
	None,
	Help,
	Pack,
	Unpack
};

struct CommandEntry
{
	std::string shortName;
	std::string longName;

	CommandEntry(const char* shortName = "", const char* longName = "")
		: shortName(shortName),
		  longName(longName)
	{ }

	bool check(const std::string& cmd)
	{
		return shortName == cmd || longName == cmd;
	}
};

void printHelp()
{
	std::cout << "Huffman zipper." << std::endl
			  << "Usage: hzip [-h,--help] [-p,--pack filename] [-u,--unpack filename]" << std::endl
		      << "-h, --help: prints this help" << std::endl
			  << "-p, --pack filename: hzips a file \'filename\'" << std::endl
			  << "-u, --unpack filename: unhzips a file \'filename\'" << std::endl;
}

int main(int argc, const char* argv[])
{
	std::map<Command, CommandEntry> commands;
	commands[Command::Help] = CommandEntry("-h", "--help");
	commands[Command::Pack] = CommandEntry("-p", "--pack");
	commands[Command::Unpack] = CommandEntry("-u", "--unpack");

	std::vector<std::string> args;
	if (argc == 1)
		args.push_back(commands[Command::Help].shortName);
	else
	{
		for (int i = 1; i < argc; ++i)
			args.push_back(argv[i]);
	}	

	for (auto& arg : args)
	{
		if (commands[Command::Help].check(arg))
		{
			printHelp();
			return 0;
		}
	}

	bool pack = false;
	bool unpack = false;
	std::string filename;
	for (size_t i = 0; i < args.size(); ++i)
	{
		if (commands[Command::Pack].check(args[i]))
		{
			pack = true;
			if (i+1 == args.size())
			{
				std::cout << "You did not specify the file. Check out hzip --help." << std::endl;
				return 1;
			}
			filename = args[i+1];
		}

		else if (commands[Command::Unpack].check(args[i]))
		{
			unpack = true;
			if (i+1 == args.size())
			{
				std::cout << "You did not specify the file. Check out hzip --help." << std::endl;
				return 1;
			}
			filename = args[i+1];
		}
	}

	if (pack && unpack)
	{
		std::cout << "You can use only pack or unpack in one time" << std::endl;
		return 1;
	}

	HuffmanZip hzip;
	if (pack)
	{
		try
		{
			auto encodedFilename = hzip.encode(filename.c_str());
			std::cout << "File was successfully hzipped: \'" << encodedFilename << "\'" << std::endl;
		}
		catch(std::exception& e)
		{
			std::cerr << e.what();
		}
	}

	if (unpack)
	{
		try
		{
			auto decodedFilename = hzip.decode(filename.c_str());
			std::cout << "File was successfully unhzipped: \'" << decodedFilename << "\'" << std::endl;
		}
		catch(std::exception& e)
		{
			std::cerr << e.what();
		}

	}
/*

	auto file = "test3.txt";
	HuffmanZip hzip;
	try
	{
		auto encodedFilename = hzip.encode(file);
		auto decodedFilename = hzip.decode(encodedFilename.c_str());
	}
	catch(std::exception& e)
	{
		std::cerr << e.what();
	}
	*/
}

/*

T(84): 1 = 1110
R(82): 2 = 1111
A(65): 5 = 10
M(77): 4 = 01
P(80): 3 = 110
 (32): 3 = 00

*/