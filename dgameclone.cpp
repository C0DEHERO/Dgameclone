#include "dgameclone.h"

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <curses.h>
#include <cstring>
#include <unistd.h>
#include <sqlite3.h>

#define USE_NCURSES_COLOR

User checkUser;

Dgamelaunch::Dgamelaunch() {
    //ctor
    loggedIn = false;

    Game cdda("Cataclysm-DDA", "CDDA" ,"/home/codehero/Projects/Cataclysm-DDA/cataclysm");
    games[cdda.getOption("shortname")] = cdda;
    serverID = "Dgameclone public server";
    version = "0.1";

    anonOpts = {
        {'l', "login"},
        {'r', "register"},
        {'w', "watch"},
        {'q', "quit"},
    };
    userOpts = {
        {'c', "password"},
        {'e', "email"},
        {'p', "CDDA"},
        {'q', "quit"},
    };
    adminOpts = {
        {'a', "adduser"},
        {'e', "deleteuser"},
        {'b', "banuser"},
        {'q', "quit"},
    };
}

Dgamelaunch::Dgamelaunch(const std::string serverID) {
    this->serverID = serverID;
    Dgamelaunch();
}

Dgamelaunch::~Dgamelaunch() {
    //dtor
}

void Dgamelaunch::showMenu() {
    char ch;
    ch = printMenu();
    if(!loggedIn) {
        // anonmenu
        if(anonOpts[ch] == "login") {
            loggedIn = loginDialog();
        } else if(anonOpts[ch] == "register") {
            registerDialog();
        } else if(anonOpts[ch] == "watch") {
            watchMenu();
        } else if(anonOpts[ch] == "quit") {
            exit(EXIT_SUCCESS);
        }
    } else if(me.getName() != "admin") {
        //usermenu
        if(userOpts[ch] == "password") {
            changePwDialog();
        } else if(userOpts[ch] == "email") {
            changeEmailDialog();
        } else if(userOpts[ch] == "watch") {
            //showWatchMenu();
        } else if(userOpts[ch] == "quit") {
            exit(EXIT_SUCCESS);
        } else {
            try {
                std::string shortname = userOpts.at(ch);
                games.at(shortname).play();
            } catch (const std::out_of_range& oor) {}
        }
    } else {
        //adminmenu
        if(adminOpts[ch] == "adduser") {
            registerDialog();
        } else if(adminOpts[ch] == "deleteuser") {
            //deleteDialog();
        } else if(adminOpts[ch] == "banuser") {
            //banDialog();
        } else if(adminOpts[ch] == "quit") {
            exit(EXIT_SUCCESS);
        }
    }
}

void Dgamelaunch::init() {
    options["utf8"] = true;
    initCurses();
    prepareDb();
}

int Dgamelaunch::watchMenu()
{
    return 1;
}

bool Dgamelaunch::loginDialog() {
    std::string banner, text;
    std::string username, password;

    banner = "## "+serverID+"\n\n";

    text = "Please enter your username. (blank entry aborts)\n\n";
    username = printPrompt(banner, text, false);
    fetchUser(username);
    if(checkUser.getName() != username) {
        return false;
    }

    text = "Please enter your password.\n\n";
    password = printPrompt(banner, text, true);

    if(checkUser.getPassword() == password) {
        me = checkUser;
        setGameArgs();
        return true;
    } else {
        return false;
    }
}

void Dgamelaunch::registerDialog() {
    std::string banner, text;
    std::string username, password, email;

    banner = "## "+serverID+"\n\n";

    text = "Please enter your username. (blank entry aborts)\n\n";
    username = printPrompt(banner, text, false);

    if(username.empty()) {
        return;
    } else if(username == fetchUser(username).getName()) {
        clear();
        refresh();
        printw("This username is already registered.");
        refresh();
        getch();
        return;
    }

    password = newPasswordPrompt(banner);
    if(password.empty()) {
        return;
    }

    text = "Please enter your email address (optional).\n\n";
    email = printPrompt(banner, text, false);

    me.setName(username);
    me.setPassword(password);
    me.setEmail(email);
    me.setEnv("");
    me.setFlags(0);

    writeUser(true);
}

void Dgamelaunch::changePwDialog() {
    std::string banner, text;
    std::string password;

    banner = "## "+serverID+"\n\n";

    password = newPasswordPrompt(banner);
    if(password.empty()) {
        return;
    } else {
        me.setPassword(password);
        writeUser(false);
    }
}

void Dgamelaunch::changeEmailDialog() {
    std::string banner, text;
    std::string email;

    banner = "## "+serverID+"\n\n";

    text = "Please enter your email address (blank clears email).\n\n";
    email = printPrompt(banner, text, false);

    me.setEmail(email);
    writeUser(false);
}

std::string Dgamelaunch::newPasswordPrompt(const std::string &banner) {
    std::string text, passworda, passwordb;

    text = "Please enter your password.\n\n";
    passworda = printPrompt(banner, text, true);

    text = "Enter your password again.\n\n";
    passwordb = printPrompt(banner, text, true);

    if(passworda != passwordb) {
        clear();
        refresh();
        printw("The given passwords are not the same.");
        refresh();
        getch();
        return std::string("");
    } else if(passworda.empty()) {
        clear();
        refresh();
        printw("The password can't be empty.");
        refresh();
        getch();
        return std::string("");
    } else {
        return passworda;
    }
}

char Dgamelaunch::printMenu() {
    std::string banner, text, options;

    banner += "## "+serverID+"\n";

    text += "##\n";
    text += "## Dgameclone "+version+"\n";
    text += "## Games on this server are (not yet) recorded for in-progress viewing and playback :(\n\n";
    if(!loggedIn) {
        text += "Not logged in\n\n";
    } else {
        text += "Logged in as " + me.getName() + "\n";
    }

    if(!loggedIn) {
        options += "l) Login\n";
        options += "r) Register new user\n";
        options += "w) Watch games in progress\n\n";
        options += "s) Server info\n";
        options += "m) MOTD/NEWS (updated <insert date here>)\n\n";
        options += "q) Quit";
    } else if (me.getName() != "admin") {
        options += "c) Change password\n";
        options += "e) Change email address\n";
        options += "p) Play game\n";
        options += "w) Watch games in progress\n\n";
        options += "q) Quit\n";
    } else {
        options += "a) Add user\n";
        options += "d) Delete user\n";
        options += "b) Ban user";
        options += "q) Quit";
    }

    clear();
    refresh();
    //mvprintw(0,0, banner.c_str());
    printw(banner.c_str());
    printw(text.c_str());
    printw(options.c_str());
    printw("\n\n\n=>");
    refresh();

    return getch();
}

std::string Dgamelaunch::printPrompt(const std::string &banner, const std::string &text, const bool &password) {
    char tmpstring[20];
    clear();
    refresh();
    mvprintw(0,0, banner.c_str());
    printw(text.c_str());
    printw("\n\n\n=>");
    refresh();
    if(!password) {
        echo();
        getstr(tmpstring);
        noecho();
    } else {
        getstr(tmpstring);
    }
    return std::string(tmpstring);
}

void Dgamelaunch::setGameArgs() {
    std::vector<std::string> args;
    args.push_back("cataclysm");
    args.push_back("--basepath");
    args.push_back("/home/codehero/Projects/Cataclysm-DDA/");
    args.push_back("--username");
    args.push_back(me.getName());
    args.push_back("--shared");

    games["CDDA"].setArgs(args);
}

// ***********************************************

void Dgamelaunch::initCurses() {
    printf("\033[2J");
    if (newterm(NULL, stdout, stdin) == NULL) {
        char tmpchar[20];
        strncpy(tmpchar, options["defterm"].c_str(), 20);
        if (options["defterm"].empty() || (newterm(tmpchar, stdout, stdin) == NULL) ) {
            //debug_write("cannot create newterm");
            //graceful_exit(60);
            std::cout << "cannot create newterm";
            endwin();
            exit(60);
            }
        setenv("TERM", options["defterm"].c_str(), 1);
        }
    cbreak ();
    noecho ();
    nonl ();
    intrflush (stdscr, FALSE);
    keypad (stdscr, TRUE);
    #ifdef USE_NCURSES_COLOR
    start_color();
    use_default_colors();

    init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(9, 0, COLOR_BLACK);
    init_pair(10, COLOR_BLACK, COLOR_BLACK);
    init_pair(11, -1, -1);

    if (options["utf8"] == "true") write(1, "\033%G", 3);
    #endif
    clear();
    refresh();
}

void readConfig(const std::string &path) {

}

// ************ SQLITE STUFF **********************

void Dgamelaunch::prepareDb() {
    sqlite3 *db;
    char *qbuf;
    char *errmsg = NULL;
    int ret, retry = 10;


    qbuf = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS dglusers (id integer primary key, username text, email text, env text, password text, flags integer)");

    ret = sqlite3_open("database.db", &db);
    if (ret) {
        sqlite3_close_v2(db);
        //debug_write("sqlite3_open failed");
        exit(99);
    }

    sqlite3_busy_timeout(db, 10000);
    ret = sqlite3_exec(db, qbuf, 0, 0, &errmsg);

    sqlite3_free(qbuf);

    if (ret != SQLITE_OK) {
        sqlite3_close(db);
        //debug_write("sqlite3_exec failed");
        exit(120);
    }
    sqlite3_close_v2(db);
}

User Dgamelaunch::fetchUser(const std::string &username) {
    checkUser.setName(":::::");
    sqlite3 *db;
    char *qbuf;
    char *errmsg = NULL;
    int ret, retry = 10;

    qbuf = sqlite3_mprintf("select * from dglusers where username like '%q' limit 1", username.c_str());

    //ret = sqlite3_open_v2("database.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, );
    ret = sqlite3_open("database.db", &db);
    if (ret) {
        sqlite3_close_v2(db);
        //debug_write("sqlite3_open failed");
        exit(96);
    }

    sqlite3_busy_timeout(db, 10000);
    ret = sqlite3_exec(db, qbuf, getUserCallback, 0, &errmsg);

    sqlite3_free(qbuf);

    if (ret != SQLITE_OK) {
        sqlite3_close(db);
        //debug_write("sqlite3_exec failed");
        exit(108);
    }
    sqlite3_close_v2(db);

    return checkUser;
}

void Dgamelaunch::writeUser(const bool &requirenew) {
    sqlite3 *db;
    char *errmsg = NULL;
    int ret, retry = 10;

    char *qbuf;

    if (requirenew) {
	qbuf = sqlite3_mprintf("insert into dglusers (username, email, env, password, flags) values ('%q', '%q', '%q', '%q', %li)", me.getName().c_str(), me.getEmail().c_str(), me.getEnv().c_str(), me.getPassword().c_str(), me.getFlags());
    } else {
	qbuf = sqlite3_mprintf("update dglusers set username='%q', email='%q', env='%q', password='%q', flags=%li where id=%i", me.getName().c_str(), me.getEmail().c_str(), me.getEnv().c_str(), me.getPassword().c_str(), me.getFlags(), me.getId());
    }

    ret = sqlite3_open("database.db", &db);
    if (ret) {
	sqlite3_close(db);
	//debug_write("writefile sqlite3_open failed");
	//graceful_exit(97);
	exit(97);
    }

    sqlite3_busy_timeout(db, 10000);
    ret = sqlite3_exec(db, qbuf, NULL, NULL, &errmsg);

    sqlite3_free(qbuf);

    if (ret != SQLITE_OK) {
	sqlite3_close(db);
	//debug_write("writefile sqlite3_exec failed");
	//graceful_exit(98);
	exit(98);
    }
    sqlite3_close(db);
}

static int getUserCallback(void *NotUsed, int argc, char **argv, char **colname) {
    for(int i = 0; i < argc; i++) {
        if(strcmp(colname[i], "username") == 0)
            checkUser.setName(argv[i]);
        else if(strcmp(colname[i], "email") == 0)
            checkUser.setEmail(argv[i]);
        else if(strcmp(colname[i], "env") == 0)
            checkUser.setEnv(argv[i]);
        else if(strcmp(colname[i], "password") == 0)
            checkUser.setPassword(argv[i]);
        else if(strcmp(colname[i], "flags") == 0)
            checkUser.setFlags(argv[i]);
        else if(strcmp(colname[i], "id") == 0)
            checkUser.setId(argv[i]);
    }
    return 0;
}
