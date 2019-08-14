// Includes
#include <grp.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>

#include "args.hpp"
#include "icons.cpp"

// Defines
#define DAY 86400U  // Day in seconds
#define HOUR 3600U  // Hour in seconds

class Color {
	private:
		const uint8_t r, g, b;

		friend std::ostream &operator<<(std::ostream &os, const Color &color) {
			char buffer[32];
			sprintf(buffer, "\033[38;2;%d;%d;%dm", color.r, color.g, color.b);
			os << buffer;
			return os;
		}

	public:
		const std::string str() const {
			char buffer[32];
			sprintf(buffer, "\033[38;2;%d;%d;%dm", this->r, this->g, this->b);
			return std::string(buffer);
		}

		Color() : r(0), g(0), b(0) {}
		explicit Color(uint8_t gs) : r(gs), g(gs), b(gs) {}
		explicit Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

// COMMON COLORS
const static std::string RESET = "\033[m", BLACK = "\033[38;2;0;0;0m",
	  WHITE = "\033[38;2;255;255;255m",
	  RED = "\033[38;2;255;0;0m",
	  GREEN = "\033[38;2;0;255;0m",
	  BUE = "\033[38;2;0;0;255m",
	  YELLOW = "\033[38;2;255;255;0m",
	  MAGENTA = "\033[38;2;255;0;255m",
	  CYAN = "\033[38;0;2;255;255m",
	  PURPLE = "\033[38;2;127;32;183m",
	  LIME = "\033[38;2;111;255;8m",
	  BROWN = "\033[38;2;142;69;23m",
	  ORANGE = "\033[38;2;255;127;8m";

// Human Readable File Sizes
std::string humane(const uint64_t &size) {
	char buff[16U];
	bool giga = size / 1000000000U, mega = size / 1000000U, kilo = size / 1000U;
	if (kilo)
		sprintf(buff, "%lu%c",
				giga ? size / 1000000000U
				: (mega ? size / 1000000U : (kilo ? size / 1000U : size)),
				giga ? 'G' : mega ? 'M' : kilo ? 'K' : 'B');
	else
		sprintf(buff, "%luB", size);

	return std::string(buff);
}

const std::string to_lower(const std::string &str) {
	std::string temp = "";
	for (const unsigned char &letter : str)
		temp += (unsigned char)std::tolower(letter);

	return temp;
}

struct File {
	std::string name, short_name, icon;
	bool isDir;
	uint64_t _size;

	File(const std::string &file, unsigned long index, bool dir, uint64_t size)
		: name(file),
		short_name(file.substr(index)),
		icon("\uf15b "),
		isDir(dir),
		_size(size) {
			this->findIcon();
		}
	File(const std::string &file) : name(file) {}

	friend std::ostream &operator<<(std::ostream &os, const File &file) {
		if (file.isDir)
			os << Color(21, 162, 252)
				<< (file.icon == "\uf15b " ? "\ue5fe " : file.icon)
				<< file.short_name << '/' << RESET;
		else
			os << GREEN << file.icon << file.short_name << RESET << ' ';
		return os;
	}

	std::string str() const {
		std::string temp = "";
		if (this->isDir)
			temp += Color(21, 162, 252).str() +
				(this->icon == "\uf15b " ? "\ue5fe " : this->icon) +
				this->short_name + '/' + RESET;
		else
			temp += GREEN + this->icon + this->short_name + RESET + ' ';

		return temp;
	}

	std::string inline getExtension() const {
		if (this->short_name.find('.') != std::string::npos)
			return this->short_name.substr(this->short_name.rfind('.') + 1);
		else
			return this->short_name;
	}

	void findIcon() {
		const std::string &extension = this->getExtension();
		if (icons.find(extension) != icons.end())
			this->icon = icons.at(extension);
	}

	unsigned long length() const { return short_name.size(); }

	std::string inline size(const bool &human_readable = false) const {
		if (this->isDir)
			return (human_readable ? humane(4096) : std::to_string(4096));
		else
			return (human_readable ? humane(this->_size)
					: std::to_string(this->_size));
	}

	bool operator<(const File &file) const {
		/*[](const File &a, const File &b) { return (to_lower(a.name) <
		 * to_lower(b.name)); } */
		if (!this->isDir && file.isDir)
			return false;
		else if (this->isDir && !file.isDir)
			return true;
		return (to_lower(this->short_name) < to_lower(file.short_name));
	}

	std::string getPerms() const {
		namespace fs = std::filesystem;
		std::stringstream permissions;
		fs::perms p(std::filesystem::status(this->name).permissions());
		permissions << ((p & fs::perms::owner_read) != fs::perms::none
				? (Color(230, 220, 59).str() + "r")
				: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::owner_write) != fs::perms::none
					? (Color(42, 228, 52).str() + "w")
					: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::owner_exec) != fs::perms::none
					? (Color(224, 58, 32).str() + "x")
					: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::group_read) != fs::perms::none
					? (Color(230, 220, 59).str() + "r")
					: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::group_write) != fs::perms::none
					? (Color(42, 228, 52).str() + "w")
					: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::group_exec) != fs::perms::none
					? (Color(224, 58, 32).str() + "x")
					: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::others_read) != fs::perms::none
					? (Color(230, 220, 59).str() + "r")
					: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::others_write) != fs::perms::none
					? (Color(42, 228, 52).str() + "w")
					: (Color(88, 40, 128).str() + "-"))
			<< ((p & fs::perms::others_exec) != fs::perms::none
					? (Color(224, 58, 32).str() + "x")
					: (Color(88, 40, 128).str() + "-"))
			<< RESET << ' ';
		return permissions.str();
	}
};

inline unsigned short getWidth() {
	struct winsize size;
	ioctl(1, TIOCGWINSZ, &size);
	return size.ws_col;
}

int main(int argc, char **argv) {
	// Parse the arguments
	Args arg_parser(argc, argv);
	arg_parser.convert();

	std::vector<File> dir;
	dir.reserve(16U);
	const bool show_all = arg_parser.optExists("-a"),
		  long_list = arg_parser.optExists("-l"),
		  human_readable = arg_parser.optExists("-h");

	std::string directory(".");
	for (const auto &item : arg_parser.getOpts())
		if (item.second.mode == Option::str && item.second.name != argv[0U]) {
			directory = item.second.name;
			break;
		}

	// If the directory doesn't exist, print a message and `return 3`
	unsigned int max_dir_length = 0U;

	if (!std::filesystem::exists(std::filesystem::path(directory))) {
		std::cout << "    \033[1;31m"
			<< "Directory Not Found. " << RESET << std::endl;
		return 3;
	}

	uint64_t largest_size = 1ULL;
	// Push Files into dir Vector, if -a isn't specified don't put dotfiles in
	for (const auto &entry : std::filesystem::directory_iterator(directory)) {
		File *file;
		const std::string &path = entry.path();

		if (entry.is_symlink() &&
				std::filesystem::is_directory(std::filesystem::read_symlink(entry)))
			file = new File(path, path.rfind('/') + 1U, entry.is_directory(),
					4096UL);
		else
			file = new File(path, path.rfind('/') + 1U, entry.is_directory(),
					entry.is_regular_file() ? entry.file_size() : 0UL);

		// Don't push .dotfiles in the vector if `-a` isn't specified
		if (show_all)  // -a
			dir.push_back(*file);
		else {
			if (file->short_name.front() == '.')
				continue;
			else
				dir.push_back(*file);
		}

		// Get the longest file/directory in the directory, to be used when
		// spacing the columns
		if (file->length() > max_dir_length) max_dir_length = file->length();

		if (file->_size > largest_size) largest_size = file->_size;

		delete file;
	}

	// Empty Directory
	if (dir.size() == 0U) {
		std::cout << "    " << Color(229U, 195U, 38U)
			<< "Nothing to show here...\n"
			<< RESET;
		return 0;
	}

	// Sort Directories Alphabetically
	// .dotfolders first, 'CAPITAL' and 'lower' mixed
	// Dirs before Files
	std::sort(dir.begin(), dir.end());

	// Find the number of columns and rows to display in the Terminal
	const unsigned short term_width = getWidth();
	const unsigned short cols = term_width / (max_dir_length + 8U);

	// Determine if every file/dir name combined with spaces can fit in a single
	// row
	const bool long_filename = cols == 0U;
	unsigned short rows = long_filename ? 0U : dir.size() / cols;
	unsigned int total_length = 4U;
	for (const File &item : dir) {
		// 7U because of the file icon and the space after it and the occasional
		// '/' My rows & cols counting sucks so I do an extra check for one row
		// cases
		total_length += item.length() + 7U;

		if (total_length >= term_width) {
			++rows;
			break;
		}
	}

	/// PRINTING
	if (long_list)  // -l option
	{
		for (const File &item : dir) {
			const std::string &size = item.size(human_readable);

			struct stat info;
			stat(item.name.c_str(), &info);
			struct passwd *pw = getpwuid(info.st_uid);
			struct group *gr = getgrgid(info.st_gid);
			std::string uname(pw->pw_name);	       // Owner-User
			std::string group(gr->gr_name);	       // Owner-Group
			std::time_t modify = info.st_mtim.tv_sec;  // Last Modified Time
			std::time_t now = std::time(NULL);
			std::string m_time(ctime(
						&modify));  // Convert time_t into a prettier string equivelant
			m_time.erase(m_time.end() - 1);  // Get rid of the last '\n'

			// FIXME fix the blue colors, too similar

			// Set the color of the Modification Time, 3 Days -> 1 Day -> 6
			// Hours -> 1 Hour
			std::time_t diff = now - modify;
			std::string time_color;
			if (diff > 3 * DAY)
				time_color = Color(32, 123, 121).str();
			else if (diff > DAY && diff < 3 * DAY)
				time_color = Color(72, 144, 240).str();
			else if (diff < DAY && diff > 6 * HOUR)
				time_color = Color(108, 222, 171).str();
			else if (diff < 6 * HOUR && diff > HOUR)
				time_color = Color(146, 240, 190).str();
			else
				time_color = Color(20, 255, 40).str();

			// Size Colors
			std::string size_color;
			if (item._size < 1000000ULL)  // item < 1 MB
				size_color = WHITE;
			else if (item._size < 128000000ULL)	 // item < 128 MB
				size_color = Color(113, 220, 208).str();
			else if (item._size < 512000000ULL)	 // item < 512 MB
				size_color = Color(253, 254, 42).str();
			else if (item._size < 1000000000ULL)  // item < 1 GB
				size_color = Color(80, 255, 228).str();
			else
				size_color = CYAN;

			std::cout
				<< "    " << item.getPerms() << std::right
				<< std::setw(
						std::string(getpwuid(geteuid())->pw_name).length() -
						uname.length() + 1)
				<< ' '
				<< (pw == 0 ? (RED + "ERROR " + RESET) : (uname + RESET + ' '))
				<< std::setw(
						std::string(getgrgid(geteuid())->gr_name).length() -
						group.length() + 1)
				<< ' '
				<< (gr == 0
						? (RED + "ERROR " + RESET)
						: (Color(205, 196, 101).str() + group + RESET + ' '))
				<< std::right
				<< std::setw((human_readable
							? 4
							: std::to_string(largest_size).length()) -
						size.length() + 1)
				<< ' ' << size_color << size << RESET << "  " << time_color
				<< m_time << RESET << "   " << item.str() << std::endl;
		}
	} else {
		/// REGULAR PRINTING
		// If max_dir_length > term_width
		if (long_filename && rows == 1U)
			for (const File &item : dir)
				std::cout << "    " << item.str() << std::endl;
		// Regular printing for multiple rows
		else if (rows > 1U) {
			// If the maximum file length doesn't fit the terminal, print each
			// file on a new line
			if (max_dir_length >= term_width)
				for (const File &item : dir)
					std::cout << item.str() << std::endl;
			else {
				for (size_t i = 0U; i < dir.size() - (dir.size() % cols);
						i += (cols)) {
					std::cout << "    ";
					for (size_t n = 0U; n < cols; n++)
						if (i + n != dir.size())
							std::cout << dir[i + n].str() << std::left
								<< std::setw(max_dir_length -
										dir[i + n].length() + 4U)
								<< ' ';
					std::cout << std::endl;
				}
				if (dir.size() % cols > 0) {
					std::cout << "    ";
					for (size_t i = dir.size() - (dir.size() % cols);
							i < dir.size(); ++i)
						std::cout
							<< dir[i].str() << std::left
							<< std::setw(max_dir_length - dir[i].length() + 4U)
							<< ' ';
					std::cout << std::endl;
				}
			}
		}
		// Single Row
		else {
			// Single Row Printing
			std::cout << "    ";
			for (const File &item : dir)
				std::cout << item.str() << std::left << std::setw(4U) << ' ';
			std::cout << std::endl;
		}
	}

	return 0;
}
