#ifndef USER_H
#define USER_H

#include <string>

class User
{
    public:
        User();
        virtual ~User();
        unsigned long getId() { return id; }
        void setId(const unsigned long &val) { id = val; }
        void setId(const std::string &val) { id = std::stol(val); }
        std::string getName() { return name; }
        void setName(const std::string &val) { name = val; }
        std::string getPassword() { return password; }
        void setPassword(const std::string &val) { password = val; }
        std::string getEmail() { return email; }
        void setEmail(const std::string &val) { email = val; }
        std::string getEnv() { return env; }
        void setEnv(const std::string &val) { env = val; }
        int getFlags() { return flags; }
        void setFlags(const int &val) { flags = val; }
        void setFlags(const std::string &val) { flags = std::stoi(val); }
    protected:
    private:
        unsigned long id;
        int flags;
        std::string name;
        std::string password;
        std::string email;
        std::string env;
};

#endif // USER_H
