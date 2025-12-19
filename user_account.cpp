//
// Created by lcdelcroix on 12/11/2025.
//

#include "user_account.h"

UserAccount::UserAccount(const int id_, const std::string &name_, const std::string &password_, const std::string &email_) {
    id = id_;
    name = name_;
    password = password_;
    email = email_;
}

void UserAccount::set_id(const int id_) {
    id = id_;
}

void UserAccount::set_name(const std::string &name_) {
    name = name_;
}

void UserAccount::set_password(const std::string &password_) {
    password = password_;
}

void UserAccount::set_email(const std::string &email_) {
    email = email_;
}

int UserAccount::get_id() const {
    return id;
}

std::string UserAccount::get_name() const {
    return name;
}

std::string UserAccount::get_password() const {
    return password;
}

std::string UserAccount::get_email() {
    return email;
}
