#include <iostream>
#include "args.hpp"
#include <iomanip>
#include <sys/ioctl.h>
#include <algorithm>
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
const static std::unordered_map<std::string, std::string> icons = {
	// Programming
	{"cpp", "\ue61d "},
	{"c++", "\ue61d "},
	{"C++", "\ue61d "},
	{"c", "\ue61e "},
	{"C", "\ue61e "},
	{"cc", "\ue61d "},
	{"hpp", "\ue61d "},
	{"h", "\ue61e "},
	{"hh", "\ue61e "},
	{"java", "\ue256 "},
	{"Java", "\ue256 "},
	{"class", "\ue256 "},
	{"jar", "\ue256 "},
	{"cs", "\uf81a "},
	{"c#", "\uf81a "},
	{"C#", "\uf81a "},
	{"sh", "\uf977"},
	{"py", "\ue235 "},
	{"python", "\ue235 "},
	{"Python", "\ue235 "},
	{"pyc", "\ue73c "},
	{"php", "\uf81e "},
	{"htm", "\ue60e "},
	{"html", "\ue736 "},
	{"css", "\ue749 "},
	{"js", "\ue74e "},
	{"ts", "\ue628 "},
	{"vscode", "\ufb0f "},
	{"code", "\ufb0f "},
	{"json", "\ue60b "},
	{"src", "\uf121 "},
	// Vim
	{"vim", "\ue62b "},
	{"nvim", "\ue62b "},
	{"vimrc", "\ue62b "},
	// Audio
	{"midi", "\uf886 "},
	{"mid", "\uf886 "},
	{"mp3", "\ufc58 "},
	{"ogg", "\uf886 "},
	{"mpa", "\ufc58 "},
	{"wav", "\uf886 "},
	{"wma", "\uf886 "},
	// Video
	{"avi", "\uf880 "},
	{"h264", "\uf880 "},
	{"m4v", "\uf880 "},
	{"mkv", "\uf880 "},
	{"mov", "\uf880 "},
	{"mp4", "\uf880 "},
	{"mpg", "\uf880 "},
	{"mpeg", "\uf880 "},
	{"wmv", "\uf880 "},
	// Image
	{"bmp", "\uf7e8 "},
	{"gif", "\uf952 "},
	{"jpg", "\uf03e "},
	{"jpeg", "\uf03e "},
	{"png", "\uf03e "},
	{"ps", "\uf7ea "},
	{"psd", "\ue7b8 "},
	{"svg", "\ufc1f "},
	{"tiff", "\ue60d "},
	{"tif", "\ue60d "},
	{"xcf", "\uf71e "},
	// System Stuff
	{"config", "\ue615 "},
	{"conf", "\ue615 "},
	{"zsh_history", "\uf7d9 "},
	{"bashrc", "\ue615 "},
	{"zshrc", "\ue615 "},
	{"cfg", "\ue615 "},
	{"bak", "\ue615 "},
	{"dll", "\uf830 "},
	{"ico", "\uf6f2 "},
	{"icon", "\uf6f2 "},
	{"icons", "\uf6f2 "},
	{"ini", "\ue70f "},
	{"lnk", "\ufab2"},
	{"msi", "\ufab2"},
	{"exe", "\ufab2"},
	{"sys", "\ue712 "},
	{"temp", "\uf651 "},
	{"tmp", "\uf651 "},
	{"bin", "\uf114 "},
	{"log", "\uf89d "},
	// Office Stuff
	{"doc", "\uf72c "},
	{"docx", "\uf72c "},
	{"odt", "\uf72c "},
	{"pdf", "\uf1c1 "},
	{"rtf", "\uf15c"},
	{"txt", "\uf0f6 "},
	{"xls", "\uf1c3 "},
	{"xlsx", "\uf1c3 "},
	{"ods", "\uf1c3 "},
	{"odp", "\uf1c4 "},
	{"ppt", "\uf1c4 "},
	{"pps", "\uf1c4 "},
	{"pptx", "\uf1c4 "},
	{"md", "\ue609 "},
	{"latex", "\ue612 "},
	{"tex", "\ue612 "},
	// Archive & Compress
	{"7z", "\uf1c6 "},
	{"deb", "\uf306 "},
	{"pkg", "\uf487 "},
	{"rar", "\ufac3 "},
	{"zip", "\uf1c6 "},
	{"rpm", "\uf316 "},
	{"tar", "\uf1c6 "},
	{"gz", "\uf1c6 "},
	{"bzip", "\uf066 "},
	{"bz2", "\uf066 "},
	{"bzip2", "\uf066 "},
	{"z", "\uf1c6 "},
	// Git
	{"git", "\ue5fb "},
	{"gitignore", "\uf1d3 "},
	{"gitconfig", "\uf1d3 "},
	{"HEAD", "\ue708 "},
	{"FETCH_HEAD", "\ue708 "},
	{"ORIG_HEAD", "\ue708 "},
	{"hooks", "\ufbe0 "},
	{"branches", "\ue725 "},
	// Games
	{"steam", "\uf9d2"},
	{"minecraft", "\uf872 "},
	{"Minecraft", "\uf872 "},
	{"Team Fortress 2", "\uf1b7 "}

};

// Human Readable File Sizes
std::string humane(const uint64_t &size)
{
	char buff[8];
	bool giga = size / 1000000000U, mega = size / 1000000U, kilo = size / 1000U;
	if (kilo)
		sprintf(buff, "%.2f%c", giga ? (float)size / 1E9f : (mega ? (float)size / 1E6f : (kilo ? (float)size / 1E3f : (float)size)),
				giga ? 'G' : mega ? 'M' : kilo ? 'K' : 'B');
	else
		sprintf(buff, "%ld", size);

	return std::string(buff);
}

struct File
{
	std::string name, icon;
	bool isDir;
	uint64_t size;

	File(const std::string &file, unsigned long index, bool dir, uint64_t size) : name(file.substr(index)), icon("\uf15b "), isDir(dir), size(size) { this->findIcon(); }

	friend std::ostream &operator<<(std::ostream &os, const File &file)
	{
		if (file.isDir)
			os << Color(21, 162, 252) << (file.icon == "\uf15b " ? "\ue5fe " : file.icon) << file.name << '/' << RESET;
		else
			os << GREEN << file.icon << file.name << RESET << ' ';
		return os;
	}

	std::string str(bool human_read = false) const
	{
		std::string temp = "";
		if (this->isDir)
			temp += Color(21, 162, 252).str() + (this->icon == "\uf15b " ? "\ue5fe " : this->icon) + this->name + '/' + RESET;
		else
			// temp += GREEN + this->icon + this->name + ' ' + (human_read ? humane(this->size) : std::to_string(this->size)) + RESET;
			temp += GREEN + this->icon + this->name + RESET + ' ';

		return temp;
	}

	std::string inline getExtension() const
	{
		if (this->name.find('.') != std::string::npos)
			return this->name.substr(this->name.rfind('.') + 1);
		else
			return this->name;
	}

	void findIcon()
	{
		const std::string &extension = this->getExtension();
		if (icons.find(extension) != icons.end())
			this->icon = icons.at(extension);
	}

	unsigned long length() const { return name.size(); }

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
	// Parse the arguments
	Args arg_parser(argc, argv);
	arg_parser.convert();

	std::vector<File> dir;
	dir.reserve(32U);
	const bool show_all = arg_parser.optExists("-a"),
		  show_list = arg_parser.optExists("-l"),
		  human_readable = arg_parser.optExists("-h");

	std::string directory(".");
	for (const auto &item : arg_parser.getOpts())
		if (item.second.mode == Option::str && item.second.name != argv[0U])
		{
			directory = item.second.name;
			break;
		}
	// If the directory doesn't exist, print a message and `return 3`
	unsigned int max_dir_length = 0U;

	if (!std::filesystem::exists(std::filesystem::path(directory)))
	{
		std::cout << "    \033[1;31m"
			<< "Directory Not Found. " << RESET << std::endl;
		return 3;
	}

	// Push Files into dir Vector, if -a isn't specified don't put dotfiles in
>>>>>>> fixme
	for (const auto &entry : std::filesystem::directory_iterator(directory))
	{
		const std::string &path = entry.path();
		File file(path, path.rfind('/') + 1U, entry.is_directory(), entry.is_regular_file() ? entry.file_size() : 0UL);

		// Don't push .dotfiles in the vector if `-a` isn't specified
		if (show_all) // -a
			dir.push_back(file);
		else
		{
			if (file.name.front() == '.')
				continue;
			else
				dir.push_back(file);
		}

		// Get the longest file/directory in the directory, to be used when spacing the columns
		if (file.length() > max_dir_length)
			max_dir_length = file.length();
	}

<<<<<<< HEAD
	// If the directory is empty
	if (dir.size() == 0)
	{
		std::cout << Color(229, 194, 37) << "Nothing to show here..." << RESET << std::endl;
		return 3; // Maybe return 0, nothing went wrong...
=======
	// Empty Directory
	if (dir.size() == 0U)
	{
		std::cout << "    " << Color(229, 195, 38) << "Nothing to show here..." << RESET << std::endl;
		return 0;
>>>>>>> fixme
	}

	// Sort Directories Alphabetically
	// .dotfolders first, 'CAPITAL' before 'lower'
	// Dirs before Files
	std::sort(dir.begin(), dir.end());

	// Find the number of columns and rows to display in the Terminal
	const unsigned short term_width = getWidth();
	const unsigned short cols = term_width / (max_dir_length + 8U);
<<<<<<< HEAD
	unsigned short rows = dir.size() / cols;

	unsigned int total_length = 6U;
=======

	// Determine if every file/dir name combined with spaces can fit in a single row
	const bool long_filename = cols == 0U;
	unsigned short rows = long_filename ? 0U : dir.size() / cols;
	unsigned int total_length = 4U;
>>>>>>> fixme
	for (const File &item : dir)
	{
		// 7U because of the file icon and the space after it and the occasional '/'
		// My rows & cols counting sucks so I do an extra check for one row cases
		total_length += item.length() + 7U;
		if (total_length >= term_width)
		{
			++rows;
			break;
		}
	}

	// If max_dir_length > term_width
	if(long_filename && rows == 1U)
		for(const File& item : dir)
			std::cout << "    " << item.str(human_readable) << std::endl;
	// Regular printing for multiple rows
	else if (rows > 1U)
	{
		// If the maximum file length doesn't fit the terminal, print each file on a new line
		if (max_dir_length >= term_width)
			for (const File &item : dir)
				std::cout << item.str(human_readable) << std::endl;
		else
		{
			for (size_t i = 0U; i < dir.size() - (dir.size() % cols); i += (cols))
			{
				std::cout << "    ";
				for (size_t n = 0U; n < cols; n++)
					if (i + n != dir.size())
						std::cout << dir[i + n].str(human_readable) << std::left << std::setw(max_dir_length - dir[i + n].length() + 4U) << ' ';
				std::cout << std::endl;
			}
			std::cout << "    ";
			for (size_t i = dir.size() - (dir.size() % cols); i < dir.size(); ++i)
				std::cout << dir[i].str(human_readable) << std::left << std::setw(max_dir_length - dir[i].length() + 4U) << ' ';
		}
<<<<<<< HEAD
=======
		if (dir.size() % cols != 0)
		{
			// If there are any files on the last row
			std::cout << "    ";
			for (size_t i = dir.size() - (dir.size() % cols); i < dir.size(); ++i)
				std::cout << dir[i].str(human_readable) << std::left << std::setw(max_dir_length - dir[i].length() + 4U) << ' ';
			std::cout << std::endl;
		}
>>>>>>> fixme
	}
	// Single Row
	else
	{
		// Single Row Printing
		std::cout << "    ";
		for (const File &item : dir)
			std::cout << item.str(human_readable) << std::left << std::setw(4U) << ' ';
<<<<<<< HEAD
=======
		std::cout << std::endl;
>>>>>>> fixme
	}

	return 0;
}

