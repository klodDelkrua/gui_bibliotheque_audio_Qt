#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include <memory>
#include "audio_player.h"
#include "user_account.h"

class AuthManager {
private:
    AudioPlayer& player;
    std::unique_ptr<UserAccount> current_user;

    std::string hash_password(const std::string& password);
    bool verify_password(const std::string& input_password,const std::string& stored_hash);

public:
    explicit AuthManager(AudioPlayer& p);
    bool register_user(const std::string& username,const std::string& password,const std::string& email);
    bool login(const std::string& username,const std::string& password);
    void logout();
    bool is_logged_in() const;
    [[nodiscard]] const UserAccount& get_current_user() const;
};

#endif // AUTHMANAGER_H
