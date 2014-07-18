#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <map>
#include <cctype>


class Game
{
    public:
        Game();
        Game(const std::string &name, const std::string &shortname, const std::string &path);
        virtual ~Game();


        std::string getName() { return name; }
        void setName(const std::string &val) { name = val; }
        std::string getCommand() { return command; }
        void setCommand(const std::string &val) { command = val; }
        std::vector<std::string> getArgs() { return args; }
        void setArgs(const std::vector<std::string> &val) { args = val; }

        std::map<std::string,std::string> getConfig() { return config; }
        std::string getOption(const std::string &val) { return config[val]; }
        void setConfig(const std::map<std::string,std::string> &val) { config = val; }

        void play();
    protected:
    private:
        int pty;
        std::string name;
        std::string command;
        std::vector<std::string> args;
        std::map<std::string,std::string> config;

        char *argv[];
};

#endif // GAME_H
