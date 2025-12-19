//
// Created by lcdelcroix on 12/11/2025.
//

#include "authManager.h"

std::string AuthManager::hash_password(const std::string &password) {
    return "HASHED_" + password;
}

bool AuthManager::verify_password(const std::string &input_password,const std::string& stored_hash) {
    return ("HASHED_" + input_password) == stored_hash;
}

AuthManager::AuthManager(AudioPlayer & p) : player(p),current_user(nullptr) {}

bool AuthManager::register_user(const std::string &username, const std::string &password, const std::string &email) {
    if (password.length() < 8) {
        std::cerr<<"Password too short"<<std::endl;
        std::cerr<<"Le mot de passe doit contenir au moins 8 caracteres.\n";
        return false;
    }

    std::string hashed_pass = hash_password(password);
    //appel de audio player et de la base de donnee
    const int new_id = player.add_user(username,hashed_pass,email);
    if (new_id > 0) {
        current_user = std::make_unique<UserAccount>(new_id,username,hashed_pass,email);
        return true;
    }else {
        return false;
    }
}

bool AuthManager::login(const std::string &username, const std::string &password) {
    std::unique_ptr<UserAccount> stored_user = player.get_user_by_username(username);
    if (!stored_user) {
        return false;
    }

    std::string stored_hash = stored_user->get_password();
    if (verify_password(password,stored_hash)) {
        current_user = std::move(stored_user);
        return true;
    }else {
        return false;
    }
}

void AuthManager::logout() {
    current_user.reset();
}

bool AuthManager::is_logged_in() const {
    return current_user != nullptr;
}

const UserAccount & AuthManager::get_current_user() const {
    if (!is_logged_in()) {
        throw std::runtime_error("AuthManager: Tentative d'acces a un utilisateur non connecte.");
    }
    return *current_user;
}
