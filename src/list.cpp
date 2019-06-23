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

struct File
{
	std::string name, icon;
	bool isDir;
	uint64_t size;

	File(const std::string &file, unsigned long index, bool dir, uint64_t size) : name(file.substr(index)), icon("\uf15b "), isDir(dir), size(size) {}

	friend std::ostream &operator<<(std::ostream &os, const File &file)
	{
		if (file.isDir)
			os << Color(21, 162, 252) << "\ue5fe " << file.name << '/' << RESET;
		else
			os << GREEN << file.icon << file.name << ' ' << RESET;
		return os;
	}

	std::string str() const
	{
		std::string temp = "";
		if (this->isDir)
			temp += Color(21, 162, 252).str() + "\ue5fe " + this->name + '/' + RESET;
		else
			temp += GREEN + this->icon + this->name + ' ' + RESET;
		return temp;
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

	const unsigned int term_width = getWidth();

	unsigned int max_dir_length = 0u;
	std::string directory = ".";
	for (auto &item : arg_parser.getOpts())
		if (item.second.mode == Option::str && item.second.name != argv[0])
			directory = item.second.name;

	// FIXME Make it so that the options(Arguments) can come before the directory name, instead of argv[1], arg_get(dir_name) or smth...
	for (const auto &entry : std::filesystem::directory_iterator(directory))
	{
		const std::string path = entry.path();
		File file(path, path.rfind('/') + 1u, entry.is_directory(), entry.is_regular_file() ? entry.file_size() : 0ul);
		dir.push_back(file);

		if (file.length() > max_dir_length)
			max_dir_length = file.length();
	}

	// Sort Directories Alphabetically
	// .dotfolders first, 'CAPITAL' before 'lower'
	// Dirs before Files
	std::sort(dir.begin(), dir.end());

	// Find the number of columns and rows to display in the Terminal
	const unsigned short cols = term_width / (max_dir_length + 8),
						 rows = cols == 0 ? 0 : dir.size() / cols;

	if (rows > 1)
	{
		for (size_t i = 0u; i < dir.size() - (dir.size() % cols); i += (cols))
		{
			std::cout << "    ";
			for (size_t n = 0u; n < cols; n++)
			{
				if (i + n != dir.size())
					std::cout << dir[i + n].str() << std::left << std::setw(max_dir_length - dir[i + n].length() + 4) << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "    ";
		for (size_t i = dir.size() - (dir.size() % cols); i < dir.size(); ++i)
			std::cout << dir[i].str() << std::left << std::setw(max_dir_length - dir[i].length() + 4) << " ";
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
				std::cout << dir[i].str() << std::left << std::setw(4) << " ";
			else
			{
				end_index = i;
				flag = true;
			}
			total_len -= (dir[i].length() + 4);
		}

		if (flag)
		{
			std::cout << std::endl
					  << "    ";

			for (size_t i = end_index; i < dir.size(); ++i)
				std::cout << dir[i].str() << std::left << std::setw(4) << " ";
		}
	}

	std::cout << RESET << std::endl;
	return 0;
}
