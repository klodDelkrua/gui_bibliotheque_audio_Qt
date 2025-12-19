#ifndef USER_ACCOUNT_H
#define USER_ACCOUNT_H

#include <iostream>
#include <string>

class UserAccount {
private:
    int id;
    std::string name;
    std::string password;
    std::string email;

public:
    UserAccount() : id(0){};
    UserAccount(int id_,const std::string& name_,const std::string& password_,const std::string& email_);
    void set_id(int id_);
    void set_name(const std::string& name_);
    void set_password(const std::string& password_);
    void set_email(const std::string& email_);
    int get_id() const;
    std::string get_name() const;
    std::string get_password() const;
    std::string get_email();
};

#endif // USER_ACCOUNT_H
