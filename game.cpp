#include "game.h"

#include <curses.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

Game::Game() {
    //ctor
    config["game_path"] = "";
    config["game_name"] = "";
    config["shortname"] = "";
    config["rcfile"] = "";
    config["ttyrecdir"] = "";
    config["spool"] = "";
    config["inprogressdir"] = "";
    config["encoding"] = "";
}

Game::Game(const std::string &name, const std::string &shortname, const std::string &path) {
    //ctor
    config["game_path"] = path;
    config["game_name"] = name;
    config["shortname"] = shortname;
    config["rcfile"] = "";
    config["ttyrecdir"] = "";
    config["spool"] = "";
    config["inprogressdir"] = "";
    config["encoding"] = "utf8";
}

Game::~Game() {
    //dtor
}

void Game::play() {
    int status;
    clear();
    refresh();

    //getslave();
    pid_t pid=fork();
    if (pid == 0) {
        std::vector<char *> argv(args.size() + 1);    // one extra for the null

        for (std::size_t i = 0; i != args.size(); ++i) {
            argv[i] = &args[i][0];
        }

        execvp(getOption("game_path").c_str(), argv.data());
    } else {
     /* This is run by the parent.  Wait for the child
        to terminate. */
        while (wait(&status) != pid)       /* wait for completion  */
               ;
    }
}
