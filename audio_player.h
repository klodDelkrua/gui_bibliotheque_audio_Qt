#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include <iostream>
#include <memory>
#include "user_account.h"

class Song {
private:
    int id;
    std::string title;
    int artist_id;
    int duration;

public:
    Song() {
        id = 0;
        artist_id = 0;
        duration = 0;
    }
    void set_id(int id_);
    void set_title(const std::string& title_);
    void set_artist_id(int artist_id_);
    void set_duration(int duration_);
    int get_id() const;
    std::string get_title() const;
    int get_artist_id() const;
    int get_duration() const;
};

class Artist {
private:
    int id;
    std::string name;

public:
    Artist() {
        id = 0;
    }
    void set_id(int id_);
    void set_name(const std::string& name_);
    int get_id() const;
    std::string get_name() const;
};

class Playlist {
private:
    int id;
    std::string name;

public:
    Playlist() {
        id = 0;
    }
    void set_id(int id_);
    int get_id() const;
    void set_name(const std::string& name_);
    std::string get_name();
};

class Album {
private:
    int id;
    std::string name;
    int artist_id;

public:
    Album() {
        id = 0;
        artist_id = 0;
    }
    void set_id(int id_);
    void set_name(const std::string& name_);
    void set_artist_id(int artist_id_);
    int get_id() const;
    std::string get_name() const;
    int get_artist_id() const;
};

class AudioPlayer {
private:
    std::unique_ptr<pqxx::connection> db_connection;

public:
    explicit AudioPlayer(const std::string& conn_string ) {
        try {
            db_connection = std::make_unique<pqxx::connection>(conn_string);
            std::cout<< "Connexion a "<<db_connection->dbname()<<"Reussie."<<std::endl;
        }catch (const std::exception &e) {
            std::cerr<<e.what()<<std::endl;
        }
    }

    explicit AudioPlayer(std::unique_ptr<pqxx::connection> conn)
        : db_connection(std::move(conn)) // Utilisation de la liste d'initialisation pour le déplacement
    {
        // Pas besoin de try/catch ici, la connexion est supposée établie.
        std::cout << "Connexion BDD transferee avec succes." << std::endl;
    }

    std::vector<Song> get_all_songs() ;

    int add_artist(const std::string &name) ;
    void add_song(const std::string& title,int duration,int artist_id) ;
    void add_playlist(const std::string& name,const std::vector<int>& song_ids,int user_id) ;
    void add_album(const std::string &name,int artist_id,const std::vector<int>& songs_id) ;
    void delete_artist(const std::string& name,int artist_id) ;
    bool delete_song(int song_id);
    void delete_playlist(const std::string& name,int playlist_id);
    void delete_album(const std::string& name,int album_id);
    std::vector<Playlist> get_playlist_by_user(int user_id);
    std::vector<Artist> get_all_artists();
    std::vector<Album> get_all_albums();
    int add_user(const std::string& username,const std::string& hash,const std::string& email);
    std::unique_ptr<UserAccount> get_user_by_username(const std::string& username);
    void like_song(int user_id, int song_id, bool is_liked);
    std::vector<Song> get_liked_songs(int user_id);
    std::vector<Song> search_song_by_title(const std::string& query);
};

#endif // AUDIO_PLAYER_H
