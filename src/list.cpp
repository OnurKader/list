#include <iostream>
#include "args.hpp"
#include <iomanip>
#include <algorithm>
#include <sys/ioctl.h>
#include <filesystem>

class Color
{
private:
	uint8_t r, g, b;

	friend std::ostream &operator<<(std::ostream &os, const Color &color)
	{
		char buffer[32];
		sprintf(buffer, "\033[38;2;%d;%d;%dm", color.r, color.g, color.b);
		os << buffer;
		return os;
	}

public:
	const std::string str() const
	{
		char buffer[32];
		sprintf(buffer, "\033[38;2;%d;%d;%dm", this->r, this->g, this->b);
		return std::string(buffer);
	}

	Color() : r(0), g(0), b(0) {}
	explicit Color(uint8_t gs) : r(gs), g(gs), b(gs) {}
	explicit Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

// COMMON COLORS
const static std::string RESET = "\033[m",
						 BLACK = "\033[38;2;0;0;0m",
						 WHITE = "\033[38;2;255;255;255m",
						 RED = "\033[38;2;255;0;0m",
						 GREEN = "\033[38;2;0;255;0m",
						 BUE = "\033[38;2;0;0;255m",
						 YELLOW = "\033[38;2;255;255;0m",
						 MAGENTA = "\033[38;2;255;0;255m",
						 CYAN = "\033[38;0;2;255;255m",
						 PURPE = "\033[38;2;127;32;183m",
						 LIME = "\033[38;2;111;255;8m",
						 BROWN = "\033[38;2;142;69;23m",
						 ORANGE = "\033[38;2;255;127;8m";

// Icon lookup map
const static std::unordered_map<std::string, std::string> icons = {{"cpp", "\ue61d "},
																   {"c", "\ue61e "},
																   {"cc", "\ue61d "},
																   {"hpp", "\ue61d "}};

// Human Readable File Sizes
std::string humane(const uint64_t &size)
{
	char buff[8];
	bool giga = size / 1000000000U, mega = size / 1000000U, kilo = size / 1000U;
	if (kilo)
		sprintf(buff, "%.2f%c", giga ? (float)size / 1E9f : (mega ? (float)size / 1E6f : (kilo ? (float)size / 1E3f : (float)size)),
				giga ? 'G' : mega ? 'M' : kilo ? 'K' : ' ');
	else
		sprintf(buff, "%ld", size);

	return std::string(buff);
}

struct File
{
	std::string name, icon;
	bool isDir;
	uint64_t size;

	// Add Icon Functionality, .cpp files \ue61d and stuff, vscode git python java js ...

	File(const std::string &file, unsigned long index, bool dir, uint64_t size) : name(file.substr(index)), icon("\uf15b "), isDir(dir), size(size) { this->findIcon(); }

	friend std::ostream &operator<<(std::ostream &os, const File &file)
	{
		if (file.isDir)
			os << Color(21, 162, 252) << "\ue5fe " << file.name << '/' << RESET;
		else
			os << GREEN << file.icon << file.name << ' ' << RESET;
		return os;
	}

	std::string str(bool human_read = false) const
	{
		std::string temp = "";
		if (this->isDir)
			temp += Color(21, 162, 252).str() + "\ue5fe " + this->name + '/' + RESET;
		else
			// temp += GREEN + this->icon + this->name + ' ' + (human_read ? humane(this->size) : std::to_string(this->size)) + RESET;
			temp += GREEN + this->icon + this->name + ' ' + RESET;

		return temp;
	}

	std::string inline getExtension() const { return (this->isDir ? "" : this->name.substr(this->name.rfind('.') + 1)); }

	void findIcon()
	{
		const std::string extension = this->getExtension();
		if (icons.find(extension) != icons.end())
			this->icon = icons.at(extension);
	}

	unsigned int length() const { return name.size(); }

	bool operator<(const File &file) const
	{
		if (!this->isDir && file.isDir)
			return false;
		else if (this->isDir && !file.isDir)
			return true;
		return (this->name < file.name);
	}
};

inline unsigned short getWidth()
{
	struct winsize size;
	ioctl(1, TIOCGWINSZ, &size);
	return size.ws_col;
}

int main(int argc, char **argv)
{
	Args arg_parser(argc, argv);
	arg_parser.convert();
	std::vector<File> dir;
	dir.reserve(64u);
	bool show_all = arg_parser.optExists("a"), show_list = arg_parser.optExists("l"), human_readable = arg_parser.optExists("h");

	const unsigned short term_width = getWidth();

	unsigned int max_dir_length = 0u;
	std::string directory(".");
	for (auto &item : arg_parser.getOpts())
		if (item.second.mode == Option::str && item.second.name != argv[0u])
		{
			directory = item.second.name;
			break;
		}

	for (const auto &entry : std::filesystem::directory_iterator(directory))
	{
		const std::string path = entry.path();
		File file(path, path.rfind('/') + 1u, entry.is_directory(), entry.is_regular_file() ? entry.file_size() : 0ul);

		if (show_all) // -a
			dir.push_back(file);
		else
		{
			if (file.name.front() == '.')
				continue;
			else
				dir.push_back(file);
		}

		if (file.length() > max_dir_length)
			max_dir_length = file.length();
	}

	// Sort Directories Alphabetically
	// .dotfolders first, 'CAPITAL' before 'lower'
	// Dirs before Files
	std::sort(dir.begin(), dir.end());

	// Find the number of columns and rows to display in the Terminal
	const unsigned short cols = term_width / (max_dir_length + 8u),
						 rows = cols == 0 ? 0 : dir.size() / cols;

	if (rows > 1)
	{
		for (size_t i = 0u; i < dir.size() - (dir.size() % cols); i += (cols))
		{
			std::cout << "    ";
			for (size_t n = 0u; n < cols; n++)
				if (i + n != dir.size())
					std::cout << dir[i + n].str(human_readable) << std::left << std::setw(max_dir_length - dir[i + n].length() + 4u) << " ";
			std::cout << std::endl;
		}
		std::cout << "    ";
		for (size_t i = dir.size() - (dir.size() % cols); i < dir.size(); ++i)
			std::cout << dir[i].str(human_readable) << std::left << std::setw(max_dir_length - dir[i].length() + 4u) << " ";
	}
	else
	{
		unsigned int total_len = 0u;
		size_t end_index = dir.size();
		// TODO Name Variables properly
		bool flag = false;

		for (auto iter = dir.begin(); iter != dir.end(); ++iter)
			total_len += iter->length() + 4u;

		std::cout << "    ";
		for (size_t i = 0; i < dir.size(); ++i)
		{
			if (total_len > dir[i].length())
				std::cout << dir[i].str(human_readable) << std::left << std::setw(4u) << " ";
			else
			{
				end_index = i;
				flag = true;
			}
			total_len -= (dir[i].length() + 4u);
		}

		if (flag)
		{
			std::cout << std::endl
					  << "    ";

			for (size_t i = end_index; i < dir.size(); ++i)
				std::cout << dir[i].str(human_readable) << std::left << std::setw(4u) << " ";
		}
	}

	if (dir.size() == 0)
		std::cout << Color(222, 198, 42) << "Nothing to show here..." << std::endl;
	else
		std::cout << RESET << std::endl;

	return 0;
}
