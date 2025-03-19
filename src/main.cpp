// Compiled shell and platform agnostic core of cdls


// TODO: Make it so that cmdline args can be passed
// TODO: Allow outside callers to run a command, output table and exit


// ---------------
// SECTION: Header
// ---------------
#pragma region Header

#include <iostream>
#include <string>
#include <filesystem>

#include <vector> // for dynamic arrays
#include <iomanip> // for setw for formatted tables

// Required to extract last modified from files and directories
#include <chrono>
#include <ctime>

#include <sstream> // Alternative to format, works with older compilers


#include <cmath> // For abs() (absolute value)


#include <regex>

// If using Windows, include windows.h to detect symlinks
#ifdef _WIN32
#include <windows.h> // Needed to detect symlinks
#endif

namespace fs = std::filesystem;

#pragma endregion


// ------------------------
// SECTION: Constants
// ------------------------
#pragma region Constants
// RegEx Constants
std::regex COMMA_STRING_REGEX("^\\,([1-9]+[0-9]*|\\,*)$");
std::regex DOT_STRING_REGEX("^\\.([1-9]+[0-9]*|\\.*)$");

const char* DEFAULT_COMMAND = ".";

char separator = fs::path::preferred_separator; // platform specific separator
#pragma endregion



// -------------------------------
// SECTION: Interpretation Classes
// -------------------------------
#pragma region Interpretation Classes

//Class to represent a date-time
class datetime
{
	public:
		int Day;
		int Month;
		int Year;
		int Hour;
		int Minute;
		int Second;
	
	// Constructor (takes a file_time_type as an input)
	datetime(fs::file_time_type ftt)
	{
		using namespace std::chrono;
		
		// Convert to clock
		auto sys_time = time_point_cast<system_clock::duration>(
			ftt - fs::file_time_type::clock::now() + system_clock::now()
		);
		
		// Convert to time_t
		std::time_t tt = system_clock::to_time_t(
			sys_time
		);
		
		// Convert to tm for properties
		std::tm* localTime = std::localtime(&tt);
		
		
		Day = localTime->tm_mday;
		Month = (localTime->tm_mon + 1); // +1 since tm_mon is 0-11
		Year = (localTime->tm_year + 1900); // +1900 since tm_year is years since 1900
		
		Hour = localTime->tm_hour;
		Minute = localTime->tm_min;
		Second = localTime->tm_sec;
	}
	
	std::string OutputString()
	{
		std::ostringstream oss;
		
		// Output string
		oss << std::setw(2) << std::setfill('0') << Day;
		oss << "/" << std::setw(2) << std::setfill('0') << Month;
		oss << "/" << Year;
		
		oss << " " << std::setw(2) << std::setfill('0') << Hour;
		oss << ":" << std::setw(2) << std::setfill('0') << Minute;
		oss << ":" << std::setw(2) << std::setfill('0') << Second;
		
		return oss.str();
	}
};

//Class to represent files and directories
class ChildItem {
	private:
		int base_index = 0;
		// isDir will be one of IS_DIR or IS_FILE
		bool isDir;
		int size = 0;

	public:
		fs::path path;
		
		// Constructor
		ChildItem(fs::path path, int index = 0) : path(path), base_index(index) {}
		
		// STANDARD METHODS
		bool exists()
		{
			return fs::exists(path);
		}
		
		bool is_directory()
		{
			return fs::is_directory(path);
		}
		
		bool is_regular_file()
		{
			return fs::is_regular_file(path);
		}
		
		// Is symlink uses windows-specific code if on a windows system
		bool is_symlink()
		{
			#ifdef _WIN32
				// If windows
				std::string path_string = path.string();
				
				std::wstring wpath = std::wstring(path_string.begin(), path_string.end());
				DWORD attributes = GetFileAttributesW(wpath.c_str());
				
				return (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
			#else
				return fs::is_symlink(path);
			#endif
		}
		
		// Return filenamep
		std::string getFilename() {
			return path.filename().string();
		}
		
		int lenFilename()
		{
			return getFilename().length();
		}
		
		
		// Return index as a string (can be "")
		std::string getIndex()
		{
			if (is_directory()) {
				return std::to_string(base_index);
			} else {
				return "";
			}
		}
		
		int lenIndex()
		{
			return getIndex().length();
		}
		
		void setIndex(int index)
		{
			base_index = index;
		}
		
		
		// Return filesize as string (can be "DIR" for directory)
		std::string getFilesize()
		{
			if (is_directory()) {
				return "DIR";
			} else {
				return std::to_string(fs::file_size(path));
			}
		}
		
		
		int lenFilesize()
		{
			return getFilesize().length();
		}
		
		
		// Get last modified time and date
		std::string getModified()
		{
			// Get last write time as std::filesystem::file_time_type
			fs::file_time_type ftime = fs::last_write_time(path);
			
			// Convert to datetime
			datetime modified(ftime);
			
			// Format to string
			// std::string time_string = std::format("{:02d}/{:02d}/{} {:02d}:{:02d}:{:02d}", modified.Day, modified.Month, modified.Year, modified.Hour, modified.Minute, modified.Second);
			return modified.OutputString();
		}
		
		int lenModified()
		{
			return getModified().length();
		}
		
		
		// Get permissions
		std::string getMode()
		{
			fs::perms permissions = fs::status(path).permissions();
			
			std::string perm_string;
			if (is_symlink()) {
				perm_string += "l";
			} else if (is_directory()) {
				perm_string += "d";
			} else if (is_regular_file()) {
				perm_string += "-";
			}
			
			
			// Owner permissions
			perm_string += (permissions & fs::perms::owner_read) != fs::perms::none ? "r" : "-";
			perm_string += (permissions & fs::perms::owner_write) != fs::perms::none ? "w" : "-";
			perm_string += (permissions & fs::perms::owner_exec) != fs::perms::none ? "x" : "-";
			
			// Group permissions
			perm_string += (permissions & fs::perms::group_read) != fs::perms::none ? "r" : "-";
			perm_string += (permissions & fs::perms::group_write) != fs::perms::none ? "w" : "-";
			perm_string += (permissions & fs::perms::group_exec) != fs::perms::none ? "x" : "-";
			
			// Others permissions
			perm_string += (permissions & fs::perms::others_read) != fs::perms::none ? "r" : "-";
			perm_string += (permissions & fs::perms::others_write) != fs::perms::none ? "w" : "-";
			perm_string += (permissions & fs::perms::others_exec) != fs::perms::none ? "x" : "-";
			
			return perm_string;
		}
		
		int lenMode()
		{
			return 10; // Only length of permissions
		}
		
		
};
#pragma endregion



// -------------------------------
// SECTION: Interpretation Methods
// -------------------------------
#pragma region Interpretation Methods
// Check if a directory exists, and optionally make it the CWD
bool ChkDir(fs::path path, bool _change = false)
{
	try {
        bool _exists = fs::exists(path) && fs::is_directory(path);
		if (_exists && _change) { fs::current_path(path); }
		
		return _exists;
    }
    catch (const fs::filesystem_error& e) {
		std::cerr << "Error: invalid path " << path << std::endl;
        return false; // Return false if there's an error accessing the path
    }
}


// Dot-path (needed for Windows)
std::string DotPath(int num_dots)
{
	// Return single dot if number of dots is one, or double dots if two
	if (num_dots == 1) { return "."; }
	
	//Reduce number of dots by one to get correct number of array elements
	num_dots--;
	std::vector<std::string> dots(num_dots, "..");
	
	// Join with "/"
    std::string result = dots[0];
    for (int i = 1; i < num_dots; i++) {
        result += "/" + dots[i];
    }
	
	return result;
}

// Comma string
std::string CommaPath(int num_commas) {
	
	// If 0 or 1, return the root directory
	if (num_commas == 0 || num_commas == 1) {
		return fs::current_path().root_path().string();
	}
	
	std::string cur_path = fs::current_path().string();
	
	size_t pos = 0;
    int count = 0;
    while (pos != std::string::npos && count < num_commas) {
        pos = cur_path.find(separator, pos + 1);
        if (pos != std::string::npos) count++;
    }
	
	return (count == num_commas) ? cur_path.substr(0, pos) : cur_path;
}


std::string CodePath(char sought, std::string parse_string) {
	
	// Pointer to a function that will parse the resultant integer
	std::string (*action)(int);
	
	// Determine the correct function for pointer action
	switch(sought) {
		case '.':
			 action = DotPath;
			break;
		case ',':
			action = CommaPath;
			break;
	}
	
    // Case 1: Only dots (e.g., ".....")
    if (parse_string.find_first_not_of(sought) == std::string::npos) {
        return action(parse_string.length());  // Return the number of dots
    }

    // Case 2: A dot followed by an integer (e.g., ".123")
    if (parse_string[0] == sought && parse_string.length() > 1) {
        std::string numberPart = parse_string.substr(1);
        if (numberPart.find_first_not_of("0123456789") == std::string::npos) {
            return action(std::stoi(numberPart));  // Convert the number part to an integer
        }
    }

    // Invalid input case
    throw std::invalid_argument("Invalid input format");
}

std::string CommaPath(std::string comma_string) {
	return CodePath(',', comma_string);
}

std::string DotPath(std::string dot_string) {
	return CodePath('.', dot_string);
}
#pragma endregion


// ----------------------
// SECTION: Table Display
// ----------------------
#pragma region Table Display
// A simple class for storing column headers & widths
class Column {
	private:
		bool left_bool = true;
	public:
		std::string Name;
		int Width;
		
		
		// Constructor
		Column(std::string Name) : Name(Name) 
		{
			Width = Name.length();
		}
		
		
		// Change width to the higher of min_width and a number
		void check_width(int new_width)
		{
			if (Width < new_width)
			{
				Width = new_width;
			}
		}
		
		bool LoR () { // Left or right aligned
			return left_bool;
		}
};

// Print table to screen
void print_table(std::vector<ChildItem> children)
{
	std::vector<Column> COL;
	
	// Columns for displaying information
	COL.push_back(Column("Index"));
	COL.push_back(Column("Name"));
	COL.push_back(Column("Filesize"));
	COL.push_back(Column("Modified"));
	COL.push_back(Column("Permissions"));
	
	// Set column widths
	for (ChildItem kiddo : children)
	{
		// Reset widths of columns
		COL[0].check_width(kiddo.lenIndex());
		COL[1].check_width(kiddo.lenFilename());
		COL[2].check_width(kiddo.lenFilesize());
		COL[3].check_width(kiddo.lenModified());
		COL[4].check_width(10);
	}
	
	
	
	// Write column headers
	for (Column c : COL)
	{
		std::cout << std::left << std::setw(c.Width) << c.Name << " ";
	}
	std::cout << std::endl;
	
	// Write table divider
	for (Column c : COL)
	{
		std::cout << std::setfill('-') << std::setw(c.Width) << "" << " ";
	}
	std::cout << std::setfill(' ') << std::endl;

	
	//TODO: Automate table left/right and function by column
	// Write table
	for (ChildItem kiddo : children) {
		std::cout << std::right << std::setw(COL[0].Width) << kiddo.getIndex() << " ";
		std::cout << std::left << std::setw(COL[1].Width) << kiddo.getFilename() << " ";
		std::cout << std::right << std::setw(COL[2].Width) << kiddo.getFilesize() << " ";
		std::cout << std::setw(COL[3].Width) << kiddo.getModified() << " ";
		std::cout << std::left << std::setw(COL[4].Width) << kiddo.getMode();
		std::cout << std::endl;
	}
}
#pragma endregion




// -------------
// SECTION: Main
// -------------
#pragma region Main
// Global variables

// Reset every time CDLS is run
int index_count = 0;
std::vector<ChildItem> children; // array to hold childitems

bool initialised = false; // whether initialisation of above values has happened in order to interpret commands


// List the directory, and get directory indices
extern "C" void LS(bool verbose = true) { // verbose = false allows silent initialisation for command interpretation
	fs::path path = fs::current_path();
	std::cout << std::endl << "Directory: " << path.string() << std::endl <<std::endl;
	
	
	// Get list of child items for the current directory
	//
	
	children = std::vector<ChildItem>(); // array to hold childitems
	std::vector<ChildItem> child_files; // array to initially hold files
	index_count = 0; // counter for indexing directories
	
	// Add . and .. to directory tree
	ChildItem current_directory("..", -1);
	ChildItem parent_directory(".", 0);
	
	children.push_back(current_directory);
	children.push_back(parent_directory);
	
	// Loop through child items, add an index to directories and add files at the end
	for (const auto& entry : fs::directory_iterator(path)) {
		if (fs::is_directory(entry)) {
			index_count++;
			ChildItem newChild(entry.path(), index_count);
			
			children.push_back(newChild);
		} else if (fs::is_regular_file(entry)) {
			ChildItem newChild(entry.path());
			
			child_files.push_back(newChild);
		}
	}
	
	// Add files to end of directories
	children.insert(children.end(), child_files.begin(), child_files.end());
	
	
	initialised = true; // app is initialised after the first time this function is run, regardless of context
	
	// Print table (if verbose)
	if (verbose) { print_table(children); }
}

// Interpret an input string, and change CWD
int Interpret(std::string input = "") {
	// Check whether app is initialised, and initialise if not
	if (!initialised) { LS(false); }
	
	// Process input
	if (input == "") { // If blank, return to parent shell
		return 3; // Asked to exit
	}
	
	try { // Check for an integer argument
		int chosen = std::stoi(input);
		
		if (chosen >= 0) { // Non-negative integer
			if (chosen <= index_count) { // In range
				fs::current_path(children[chosen + 1].path);
			} else { // Out of range
				throw std::invalid_argument("Out of range.");
			}
		} else { // Negative integer
			
			fs::current_path(DotPath(std::abs(chosen - 1)));
		}
	}
	catch (std::invalid_argument& e) { // Non-integer argument
		if (std::regex_match(input, DOT_STRING_REGEX)) { // Is dot string
			fs::current_path(DotPath(input));
		} else if (std::regex_match(input, COMMA_STRING_REGEX)) { // Is comma string
			fs::current_path(CommaPath(input));
		} else if (ChkDir(input)) {
			fs::current_path(input);
		} else { // Invalid cdls input, pass to shell
			std::cout << input << std::endl;
			return 1; // Invalid shell argument
		}
	}
	
	return 0;
}

// Expose interpretation to C interfaces
extern "C" int Interpret(const char* input) {
	std::string s = input;
	int i = Interpret(s);
	return i;
}

extern "C" int CDLS(const char* input = DEFAULT_COMMAND) {
	int result = Interpret(input);
	LS();
	
	return result;
}

// List directory, take input and interpret it
int run_it() {
	// List directory
	LS();
	
	// Display instructions
	std::cout << "\n\n(Enter an index, path, dot string, comma string or expression:)\n";
	std::cout << "CDLS (C++) " << fs::current_path().string() << "> ";
	
	// Take input
	std::string input;
	std::getline(std::cin, input);
	
	// Interpret
	int result = Interpret(input);
	
	
	return result;
}

// Main function
int main(int argc, char* argv[]) {
	while (true) {
		int result = run_it();
		if (result == 3) {
			break;
		}
	}
	
	return 0;
}
#pragma endregion