// Compiled shell and platform agnostic core of cdls


// TODO: String parsing for "" and \ isn't working (though spaces in args are). Should really be able to evaluate more complex inputs.
// TODO: Decide if multiple inputs should be accepted, and what the parameters should be

// TODO: Error handling for access denied (CD)

// TODO: Allow sub-paths for dot paths and comma paths - e.g. ,,,/Documents
// TODO: Allow auto-completion: Doc (or doc) should go to Documents if nothing else starts with Doc
// TODO: Allowing autocompletion down the tree could be a very powerful tool for this app

// TODO: (Perhaps) Add the ability to uses fzf for auto completion (on Linux)
// TODO: Create man and cheat pages on Linux, and just general --help/-?


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
	#include <windows.h> // Needed to detect symlinks on Windows
#endif

namespace fs = std::filesystem;

#pragma endregion


// ------------------
// SECTION: Constants
// ------------------
#pragma region Constants
// RegEx Constants
extern "C" {
	const std::regex COMMA_STRING_REGEX("^\\,([1-9]+[0-9]*|\\,*)$");
	const std::regex DOT_STRING_REGEX("^\\.([1-9]+[0-9]*|\\.*)$");

	const std::regex INTEGER_REGEX("^-?[0-9]+$"); // checks if a string can be an integer
	
	const char* DEFAULT_COMMAND = "."; // if no path is specified, certain functions default to the current directory
	
	const char SEPARATOR = fs::path::preferred_separator; // platform specific separator
}
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
		std::tm localTime;
		#ifdef _WIN32 // If windows: thread safe solution
			localtime_s(&localTime, &tt);
		#else // If unix, thread safe solution
			localtime_r(&tt, &localTime);
		#endif
		
		
		Day = localTime.tm_mday;
		Month = (localTime.tm_mon + 1); // +1 since tm_mon is 0-11
		Year = (localTime.tm_year + 1900); // +1900 since tm_year is years since 1900
		
		Hour = localTime.tm_hour;
		Minute = localTime.tm_min;
		Second = localTime.tm_sec;
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
		
		#ifdef _WIN32
			bool isReparsePoint(const fs::path& path) {
				DWORD attributes = GetFileAttributesW(path.c_str());
				if (attributes == INVALID_FILE_ATTRIBUTES) {
					return false; // Path doesn't exist or error
				}
				return (attributes & FILE_ATTRIBUTE_REPARSE_POINT);
			}
		#endif

		// Get permissions
		std::string getMode()
		{
			fs::perms permissions = fs::status(path).permissions();
			
			std::string perm_string;
			if (is_symlink()) {
				perm_string += "l";

				// Check if symlink is accessible
				// (e.g. Windows can put symlinks in C:\Users\%USERNAME%\ that are not actually accessible
				// and will throw an error if you try to access them

				// Try to warn if a Windows symlink is not accessible
				#ifdef _WIN32
					if (isReparsePoint(path)) {
						perm_string += "----------";
						return perm_string;
					}
				#endif // _WIN32

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


// Class to represent a directory after walking it
class directory {
	public:
		int index_count = 0;
		std::vector<ChildItem> children;

		void flush() {
			index_count = 0;
			children = std::vector<ChildItem>();
		}
};
#pragma endregion



// -------------------------------
// SECTION: Interpretation Methods
// -------------------------------
#pragma region Interpretation Methods
// Check if directory is valid, and switch cwd to it if desired
bool ChkDir(fs::path path, bool _change = false)
{
	std::cout << "Trying path: " << path << std::endl;

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


// If passed a string, convert to path
bool ChkDir(std::string path_string, bool _change = false)
{
	fs::path path(path_string);
	return ChkDir(path, _change);
}


// Dot-path
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
        pos = cur_path.find(SEPARATOR, pos + 1);
        if (pos != std::string::npos) count++;
    }
	
	return (count == num_commas) ? cur_path.substr(0, pos) : cur_path;
}


std::string CodePath(char sought, std::string parse_string) {
	
	// Pointer to a function that will parse the resultant integer
	std::string (*action)(int) = NULL;
	
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
void print_table(const directory& dir)
{
	std::vector<Column> COL;
	
	// Columns for displaying information
	COL.push_back(Column("Index"));
	COL.push_back(Column("Name"));
	COL.push_back(Column("Filesize"));
	COL.push_back(Column("Modified"));
	COL.push_back(Column("Permissions"));
	
	// Set column widths
	for (ChildItem kiddo : dir.children)
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
	for (ChildItem kiddo : dir.children) {
		std::cout << std::right << std::setw(COL[0].Width) << kiddo.getIndex() << " ";
		std::cout << std::left << std::setw(COL[1].Width) << kiddo.getFilename() << " ";
		std::cout << std::right << std::setw(COL[2].Width) << kiddo.getFilesize() << " ";
		std::cout << std::setw(COL[3].Width) << kiddo.getModified() << " ";
		std::cout << std::left << std::setw(COL[4].Width) << kiddo.getMode();
		std::cout << std::endl;
	}
}
#pragma endregion




// -----------------------
// SECTION: CDLS_LIB Logic
// -----------------------
#pragma region CDLS_LIB Logic
// Walk the directory, order folders before files and assign indices
void walk_dir(directory &dir, fs::path path = fs::current_path()) { // verbose = false allows silent initialisation for command interpretation
	//fs::path path = fs::current_path();

	
	// Get list of child items for the current directory
	//
	dir.flush();
	std::vector<ChildItem> child_files; // array to initially hold files
	
	// Add . and .. to directory tree
	ChildItem current_directory("..", -1);
	ChildItem parent_directory(".", 0);
	
	dir.children.push_back(current_directory);
	dir.children.push_back(parent_directory);
	
	// Loop through child items, add an index to directories and add files at the end
	for (const auto& entry : fs::directory_iterator(path)) {
		if (fs::is_directory(entry)) {
			dir.index_count++;
			ChildItem newChild(entry.path(), dir.index_count);
			
			dir.children.push_back(newChild);
		} else if (fs::is_regular_file(entry)) {
			ChildItem newChild(entry.path());
			
			child_files.push_back(newChild);
		}
	}
	
	// Add files to end of directories
	dir.children.insert(dir.children.end(), child_files.begin(), child_files.end());
}


// Display the directory using print_table
void display_dir(const directory& dir, fs::path path = fs::current_path()) {
	std::cout << std::endl << "Directory: " << path.string() << std::endl << std::endl;
	print_table(dir);
}



// Interpret an input string, and change CWD
int ch_dir(const directory& dir, std::string input = "") {
	//// Check whether app is initialised, and initialise if not
	//if (!__initialised) { LS(false); }
	
	// Process input
	if (input == "") { // If blank, return to parent shell
		return 3; // Asked to exit
	}
	
	fs::path path(input);

	try { // Check for an integer argument
		//int chosen = std::stoi(input);
		int chosen;
		if (std::regex_match(input, INTEGER_REGEX)) { // input is integer
			chosen = std::stoi(input);
		} else { // not an integer
			throw std::invalid_argument("Not an integer.");
		}
		
		if (chosen >= 0) { // Non-negative integer
			if (chosen <= dir.index_count) { // In range
				try {
					fs::current_path(dir.children[chosen + 1].path);
				}
				catch (const fs::filesystem_error& e) { // Access to the specified directory is denied
					std::cout << "Nope!" << std::endl;
					std::cerr << "Filesystem Error: Access to the specified directory is denied." << std::endl;
				}
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
#pragma endregion

// -----------------------------------
// SECTION: CDLS_LIB Exposed Functions
// -----------------------------------
#pragma region CDLS_LIB Exposed Functions

// Templates to automate overloading


// Interpret command and change directory
int CD(directory& dir, const std::string input = ".") {
	// Walk the directory
	walk_dir(dir);
	// Interpret the input
	return ch_dir(dir, input);
}

int CD(const std::string input = ".") {
	directory dir;
	return CD(dir, input);
}

int CD(directory& dir, const char* input = DEFAULT_COMMAND) {
	// Convert to string
	std::string s = input;
	return CD(dir, s);
}

// Expose interpretation to C interfaces using C-style string inputs
extern "C" int CD(const char* input = DEFAULT_COMMAND) {
	// Create directory object
	directory dir;

	// Pass it to CD
	return CD(dir, input);
}

// Walk the current directory and display it
void LS(directory& dir) {
	walk_dir(dir);
	display_dir(dir);
}

extern "C" void LS() {
	directory dir;
}

// Populate the list, evaluate input, change directory and display result - the whole shebang
int CDLS(directory& dir, const std::string input = ".") {
	int retval = CD(dir, input);
	LS(dir);

	return retval;
}

int CDLS(const std::string input = ".") {
	directory dir;
	return CDLS(dir, input);
}

int CDLS(directory& dir, const char* input = DEFAULT_COMMAND) {
	// Convert to string
	std::string s = input;
	return CDLS(dir, s);
}

extern "C" int CDLS(const char* input = DEFAULT_COMMAND) {
	directory dir;
	return CDLS(dir, input);
}


// Process multiple inputs (i.e. command line args)
int CDLS(directory& dir, int argc, char* argv[]) {
	int retval = 0;

	// Check if there are any arguments
	if (argc > 1) {
		// If there are arguments, interpret them
		for (int i = 1; i < argc; i++) {
			retval = CD(argv[i]);
			dir.flush();

			if (retval != 0) {
				LS(dir);
				return retval;
			}
		}

		LS(dir);
		std::cout << std::endl;

	}

	return 0;
}
#pragma	endregion


#ifndef BUILDING_SHARED
// -------------
// SECTION: Main
// -------------
#pragma region Main
// List directory, take input and interpret it
int run_it(directory& dir) {
	// List directory
	LS(dir);

	// Display instructions
	std::cout << "\n\n(Enter an index, path, dot string, comma string or expression:)\n";
	std::cout << "CDLS (C++) " << fs::current_path().string() << "> ";

	// Take input
	std::string input;
	std::getline(std::cin, input);

	// Interpret
	const char* cstr = input.c_str();
	int result = CD(dir, cstr);

	return result;
}

int run_it() {
	directory dir;
	return run_it(dir);
}

// Main function
int main_real(int argc, char* argv[]) {
	directory dir;

	// If there are command line arguments, interpret them
	if (argc > 1) {
		return CDLS(dir, argc, argv);
	}

	// Else run the main loop
	while (true) {
		int result = run_it(dir);
		if (result == 3) {
			break;
		}
	}
	
	return 0;
}

// Test function
int main_test(int argc, char* argv[]) {
	return 0;
}

int main(int argc, char* argv[]) {
	return main_real(argc, argv);
}

#pragma endregion
#endif