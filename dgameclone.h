#ifndef DGAMELAUNCH_H
#define DGAMELAUNCH_H

#include "user.h"
#include "game.h"
#include "menu.h"
#include "dialog.h"

#include <string>
#include <vector>
#include <map>

static int getUserCallback(void *NotUsed, int argc, char **argv, char **colname); // for sqlite

class Dgamelaunch
{
    public:
        Dgamelaunch();
        Dgamelaunch(const std::string serverID);
        virtual ~Dgamelaunch();
        User Getme() { return me; }
        void Setme(User val) { me = val; }

        void prepareDb();
        User fetchUser(const std::string &username);
        void writeUser(const bool &requirenew);

        void showMenu();
        void showDialog();
        std::string getOption(const std::string &option) { return options[option]; }
        void setOption(const std::string &option, const std::string &val) { options[option] = val; }

        void init();
        void initCurses();

        char anonMenu();
        char userMenu();
        int watchMenu();

        bool loginDialog();
        void registerDialog();
        void changePwDialog();
        void changeEmailDialog();
        std::string newPasswordPrompt(const std::string &banner);

        char printMenu();
        std::string printPrompt(const std::string &banner, const std::string &text, const bool &password);

        void play();
    protected:
    private:
        bool loggedIn;
        User me;
        std::map<std::string,Game> games;
        std::map<std::string,std::string> options;

        std::map<char,std::string> anonOpts;
        std::map<char,std::string> userOpts;
        std::map<char,std::string> adminOpts;

        std::string serverID;
        std::string version;

        void setGameArgs();
};

#endif // DGAMELAUNCH_H
