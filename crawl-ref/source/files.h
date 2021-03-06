/**
 * @file
 * @brief Functions used to save and load levels/games.
**/

#ifndef FILES_H
#define FILES_H

#include <cstdio>
#include <set>
#include <string>
#include <vector>

struct player_save_info;

enum load_mode_type
{
    LOAD_START_GAME,            // game has just begun
    LOAD_RESTART_GAME,          // loaded savefile
    LOAD_ENTER_LEVEL,           // entered a level normally
    LOAD_VISITOR,               // Visitor pattern to see all levels
};

bool file_exists(const string &name);
bool dir_exists(const string &dir);
bool is_absolute_path(const string &path);
void assert_read_safe_path(const string &path);
off_t file_size(FILE *handle);

vector<string> get_dir_files(const string &dir);
vector<string> get_dir_files_ext(const string &dir, const string &ext);
vector<string> get_dir_files_recursive(const string &dirname,
                                       const string &ext = "",
                                       int recursion_depth = -1,
                                       bool include_directories = false);

string datafile_path(string basename, bool croak_on_fail = true,
                     bool test_base_path = false,
                     bool (*thing_exists)(const string&) = file_exists);

string get_parent_directory(const string &filename);
string get_base_filename(const string &filename);
string get_cache_name(const string &filename);
string get_path_relative_to(const string &referencefile,
                            const string &relativepath);
string catpath(const string &first, const string &second);
string canonicalise_file_separator(const string &path);

bool check_mkdir(const string &what, string *dir, bool silent = false);

// Find saved games for all game types.
vector<player_save_info> find_all_saved_characters();

string get_save_filename(const string &name);
string get_savedir_filename(const string &name);
string savedir_versioned_path(const string &subdirs = "");
string get_prefs_filename();
string change_file_extension(const string &file, const string &ext);

time_t file_modtime(const string &file);
time_t file_modtime(FILE *f);
vector<string> get_title_files();

class level_id;

bool load_level(dungeon_feature_type stair_taken, load_mode_type load_mode,
                const level_id& old_level);
void delete_level(const level_id &level);

void save_game(bool leave_game, const char *bye = nullptr);

// Save game without exiting (used when changing levels).
void save_game_state();

bool get_save_version(reader &file, int &major, int &minor);

bool save_exists(const string& filename);
bool restore_game(const string& filename);

bool is_existing_level(const level_id &level);

class level_excursion
{
protected:
    level_id original;
    bool ever_changed_levels;

public:
    level_excursion();
    ~level_excursion();

    void go_to(const level_id &level);
};

void save_ghost(bool force = false);
bool load_ghost(bool creating_level, bool delete_file = true);

FILE *lk_open(const char *mode, const string &file);
FILE *lk_open_exclusive(const string &file);
void lk_close(FILE *handle, const string &file);

// file locking stuff
bool lock_file_handle(FILE *handle, bool write);
bool unlock_file_handle(FILE *handle);

class file_lock
{
public:
    file_lock(const string &filename, const char *mode, bool die_on_fail = true);
    ~file_lock();
private:
    FILE *handle;
    const char *mode;
    string filename;
};

FILE *fopen_replace(const char *name);
#endif
