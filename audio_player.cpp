//
// Created by lcdelcroix on 09/11/2025.
//

#include "audio_player.h"
#include <pqxx/pqxx>
#include <memory>
#include <chrono> // Pour le chronomètre
//#include <pqxx/params>

//Methode de la class Song
void Song::set_id(const int id_) {
    id = id_;
}

void Song::set_title(const std::string &title_) {
    title = title_;
}

void Song::set_artist_id(const int artist_id_) {
    artist_id = artist_id_;
}

void Song::set_duration(const int duration_) {
    duration = duration_;
}

int Song::get_id() const {
    return id;
}

std::string Song::get_title() const {
    return title;
}

int Song::get_artist_id() const {
    return artist_id;
}

int Song::get_duration() const {
    return duration;
}

void Artist::set_id(const int id_) {
    id = id_;
}

void Artist::set_name(const std::string &name_) {
    name = name_;
}

int Artist::get_id() const {
    return id;
}

std::string Artist::get_name() const {
    return name;
}

void Playlist::set_id(const int id_) {
    id = id_;
}

int Playlist::get_id() const {
    return id;
}

void Playlist::set_name(const std::string &name_) {
    name = name_;
}

std::string Playlist::get_name() {
    return name;
}

void Album::set_id(const int id_) {
    id = id_;
}

void Album::set_name(const std::string &name_) {
    name = name_;
}

void Album::set_artist_id(const int artist_id_) {
    artist_id = artist_id_;
}

int Album::get_id() const {
    return  id;
}

std::string Album::get_name() const {
    return name;
}

int Album::get_artist_id() const {
    return artist_id;
}


//Methode de la class Audio_Player

std::vector<Song> AudioPlayer::get_all_songs() {

    std::vector<Song> results;
    try {
        pqxx::read_transaction R(*db_connection);

        for (auto row : R.exec("SELECT id,name, artist_id, duration FROM song")) {
            Song s;
            s.set_id(row["id"].as<int>());
            s.set_title(row["name"].as<std::string>());
            s.set_artist_id(row["artist_id"].as<int>());
            s.set_duration(row["duration"].as<int>());
            results.push_back(s);
        }
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
    return results;
}

int AudioPlayer::add_artist(const std::string &name) {
    try {
        pqxx::work txn(*db_connection);
        pqxx::params p;
        p.append(name);

        // 1. Exécuter la requête avec RETURNING id
        // Note: Utilisation de exec_params est généralement plus sûr que exec simple
        pqxx::result res = txn.exec(
            "INSERT INTO artist (name) VALUES ($1) RETURNING id",
            p
            );

        // 2. Vérifier si l'insertion a réussi (une ligne doit être retournée)
        // Suppression de res.at(0).empty() qui est obsolète et redondante
        if (res.empty()) {
            std::cerr << "Erreur : L'insertion de l'artiste a echoue.\n";
            return -1;
        }

        // 3. Extraire l'ID. Nous savons que res.at(0) existe.
        // Utilisation de .at(0) ou .front() pour la première ligne, et .front() pour la première colonne.
        // Utiliser .at(0) pour les colonnes est parfois considéré comme obsolète,
        // mais .at(0) sur la ligne est encore souvent utilisé.
        // Une manière plus moderne est :
        const int new_artist_id = res[0][0].as<int>();

        // (La version res.at(0).at(0).as<int>() reste fonctionnelle pour l'instant)

        // 4. Valider la transaction
        txn.commit();

        std::cout << "Artiste '" << name << "' ajoute avec succes (ID: " << new_artist_id << ").\n";

        // 5. Retourner l'ID
        return new_artist_id;

    } catch (const pqxx::sql_error& e) {
        // ... (gestion des erreurs inchangée)
        std::cerr << "Erreur SQL lors de l'ajout d'artiste: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        // ...
        std::cerr << "Erreur generale lors de l'ajout d'artiste: " << e.what() << std::endl;
        return -1;
    }
}

void AudioPlayer::add_song(const std::string &title,const int duration,const int artist_id) {
    try {
        pqxx::work txn(*db_connection);
        pqxx::params p;
        p.append(title);
        p.append(duration);
        p.append(artist_id);

        txn.exec("INSERT INTO SONG (name,duration, artist_id)"
                 "VALUES ($1, $2, $3)",
                 p);
        txn.commit();
        std::cout<<"Song : " <<title<<" added"<<std::endl;
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
}

void AudioPlayer::add_playlist(const std::string &name, const std::vector<int> &song_ids,int user_id) {
    if (name.empty() || song_ids.empty()) {
        std::cerr<<"Erreur : Une playlist doit avoir un nom et au moins une chanson\n";
        return;
    }
    try {
        pqxx::work W(*db_connection);
        pqxx::params p_name;
        p_name.append(name);
        p_name.append(user_id);
        //on ajoute "RETURNING id" a la requete.
        //on utilise exec1() qui signifie "execute et attend UNE ligne en retour"
        const pqxx::result res = W.exec("INSERT INTO playlist (name,user_id) VALUES ($1,$2) RETURNING id",p_name);

        if (res.size() != 1) {
            throw std::logic_error("L'insertion de la playlist n'a pas retourne d'ID\n");
        }

        const int new_playlist_id = res[0]["id"].as<int>();

        std::cout <<"la playlist '" <<name<<"' cree avec l'ID : "<<new_playlist_id<<std::endl;
        std::cout<<"Ajout des chansons..."<<std::endl;

        for (int song_id : song_ids) {
            pqxx::params p_link;
            p_link.append(song_id);
            p_link.append(new_playlist_id);

            W.exec("INSERT INTO playlist_song (song_id, playlist_id) VALUES ($1,$2)",p_link);
        }
        W.commit();
        std::cout << "Playlist '" << name << "' ajoutee avec " << song_ids.size() << " chansons.\n";
    }catch (const std::exception& e) {
        std::cerr<<"Erreur lors de la creation de la playliste : "<<e.what()<<std::endl;
        std::cerr << "ANNULATION : la playlist n'a pas ete cree" << std::endl;
    }
}

void AudioPlayer::add_album(const std::string &name,const int artist_id,const std::vector<int>& songs_id) {
    if (name.empty() || songs_id.empty()) {
        std::cerr<<"Erreur : un album doit avoir un nom et au moins une chanson\n";
        return;
    }
    try {
        pqxx::work W(*db_connection);
        pqxx::params p_name;
        p_name.append(name);
        p_name.append(artist_id);

        const pqxx::result res = W.exec("INSERT INTO album (name,artist_id) VALUES ($1,$2) RETURNING id",p_name);

        if (res.size() != 1) {
            throw std::logic_error("L'insertion de l'album n'a pas retourne d'ID\n");
        }
        const int new_album_id = res[0]["id"].as<int>();

        std::cout <<"L'album " <<name<<" cree avec l'ID "<<new_album_id<<std::endl;

        for (int song_id : songs_id) {
            pqxx::params p_link;
            p_link.append(song_id);
            p_link.append(new_album_id);

            W.exec("INSERT INTO album_song (song_id, album_id) VALUES ($1,$2)",p_link);
        }
        W.commit();
        std::cout<<"L'album "<<name <<" ajoutee avec "<<songs_id.size()<<" chansons\n";
    }catch (const std::exception& e) {
        std::cerr<<"Erreur lors de la creation de la playliste : "<<e.what()<<std::endl;
        std::cerr << "ANNULATION : la playlist n'a pas ete cree" << std::endl;
    }
}

void AudioPlayer::delete_artist(const std::string& name,const int artist_id) {
    try {
        pqxx::work W(*db_connection);
        pqxx::params p_link;
        p_link.append(artist_id);

        const pqxx::result res  = W.exec("DELETE FROM artist WHERE id = ($1)",p_link);
        W.commit();
        if (res.affected_rows() > 0) {
            std::cout<<"L'artist '"<<name<<"' (ID : "<<artist_id<<") a ete supprime"<<std::endl;
        }else {
            std::cout <<"ATTENTION : Aucun artiste trouve ave l'ID "<<artist_id<<std::endl;
        }
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
}

bool AudioPlayer::delete_song(const int song_id) {
    try {
        pqxx::work W(*db_connection);
        pqxx::params p_link;
        p_link.append(song_id);

        const pqxx::result res = W.exec("DELETE FROM song WHERE id = ($1)",p_link);
        W.commit();
        if (res.affected_rows() > 0) {
            std::cout <<"Le song (ID : "<<song_id<<") a ete supprime"<<std::endl;
            return true;
        }else {
            std::cout<<"ATTENTION : Aucun song de l'ID "<<song_id<<std::endl;
            return false;
        }
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
    std::cerr<<"comportement inattendu du programme \n";
    return false;
}

void AudioPlayer::delete_playlist(const std::string &name,const int playlist_id) {
    try {
        pqxx::work W(*db_connection);
        pqxx::params p_link;
        p_link.append(playlist_id);

        const pqxx::result res = W.exec("DELETE FROM playlist WHERE id = ($1)",p_link);
        W.commit();
        if (res.affected_rows() > 0) {
            std::cout <<"La playlist '" <<name<<"' (ID : "<<playlist_id<<") a ete supprime"<<std::endl;
        }else {
            std::cout<<"ATTENTION : Aucune playlist de l'ID : "<<playlist_id<<std::endl;
        }
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
}

void AudioPlayer::delete_album(const std::string &name, int album_id) {
    try {
        pqxx::work W(*db_connection);
        pqxx::params p_link;
        p_link.append(album_id);

        const pqxx::result res = W.exec("DELETE FROM album WHERE id = ($1)",p_link);
        W.commit();
        if (res.affected_rows() > 0) {
            std::cout <<"L'album '"<<name<<"' (ID : "<<album_id<<") a ete supprime"<<std::endl;
        }else {
            std::cout <<"ATTENTION : Aucun album de l'ID : "<<album_id<<std::endl;
        }
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
}

std::vector<Playlist> AudioPlayer::get_playlist_by_user(const int user_id){
    std::vector<Playlist> playlist;
    try {
        pqxx::read_transaction W(*db_connection);
        pqxx::params p_link;
        p_link.append(user_id);
        for (auto row : W.exec("SELECT id,name FROM playlist WHERE user_id = $1",p_link)) {
            Playlist s;
            s.set_id(row["id"].as<int>());
            s.set_name(row["name"].as<std::string>());
            playlist.push_back(s);
        }
        W.commit();
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
    return playlist;
}

std::vector<Artist> AudioPlayer::get_all_artists() {
    std::vector<Artist> artists;
    try {
        pqxx::read_transaction R(*db_connection);
        for (auto row : R.exec("SELECT id,name FROM artist")) {
            Artist a;
            a.set_id(row["id"].as<int>());
            a.set_name(row["name"].as<std::string>());
            artists.push_back(a);
        }
        R.commit();
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
    return std::move(artists);
}

std::vector<Album> AudioPlayer::get_all_albums() {
    std::vector<Album> albums;
    try {
        pqxx::read_transaction R(*db_connection);
        for (auto row : R.exec("SELECT id,name,artist_id FROM album")) {
            Album a;
            a.set_id(row["id"].as<int>());
            a.set_name(row["name"].as<std::string>());
            a.set_artist_id(row["artist_id"].as<int>());
            albums.push_back(a);
        }
        R.commit();
    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
    return albums;
}

int AudioPlayer::add_user(const std::string &username, const std::string &hash, const std::string &email) {
    if (username.empty() || hash.empty() || email.empty()) {
        std::cerr<<"Erreur aucune information ne doit etre vide\n";
        return -1;
    }
    int new_id = -1;
    try {
        pqxx::work W(*db_connection);
        pqxx::params p;
        p.append(username);
        p.append(hash);
        p.append(email);

        const pqxx::result res = W.exec("INSERT INTO user_account (username,password_hash,email) VALUES ($1, $2, $3) RETURNING id",p );
        new_id = res[0]["id"].as<int>();
        W.commit();
        std::cout<<" Ajout de l'utilisateur << '"<<username<<"' cree avec l'ID : "<<new_id<<std::endl;

    }catch (const pqxx::sql_error& e) {
        std::cerr<<"Erreur SQL : "<<e.what()<<std::endl;
        std::cerr <<"Requete : "<<e.query()<<std::endl;
    }catch (const std::exception& e) {
        std::cerr<<"Erreur : "<<e.what()<<std::endl;
    }
    return new_id;
}

std::unique_ptr<UserAccount> AudioPlayer::get_user_by_username(const std::string &username) {
    try {
        pqxx::read_transaction R(*db_connection);
        pqxx::params p;
        p.append(username);

        const pqxx::result res = R.exec("SELECT id, username, password_hash, email "
                                        "FROM user_account WHERE username = $1",p);

        if (res.empty()) {
            std::cout<<"Utilisateur non trouve \n";
            return nullptr;
        }
        const pqxx::row row = res[0];

        return std::make_unique<UserAccount>(
            row["id"].as<int>(),
            row["username"].as<std::string>(),
            row["password_hash"].as<std::string>(),
            row["email"].as<std::string>());
    }catch (const std::exception& e) {
        // En cas d'erreur BDD (connexion perdue, etc.), on signale l'échec
        // en retournant aussi un pointeur nul.
        std::cerr << "Erreur BDD [get_user_by_username]: " << e.what() << std::endl;
        return nullptr;
    }
}

// Dans AudioPlayer.cpp

void AudioPlayer::like_song(const int user_id, const int song_id, const bool is_liked) {

    // Le statut 'is_liked' est converti en valeur booléenne pour la BDD (true/false)
    std::string liked_status = is_liked ? "TRUE" : "FALSE";

    try {
        pqxx::work W(*db_connection);

        pqxx::params p;
        p.append(user_id);
        p.append(song_id);

        W.exec(
            "INSERT INTO user_song_interaction (user_id, song_id, is_liked) "
            "VALUES ($1, $2, " + liked_status + ") " // $3 est remplacé par la chaîne "TRUE" ou "FALSE"
                                 "ON CONFLICT (user_id, song_id) DO UPDATE "
                                 "SET is_liked = EXCLUDED.is_liked", // EXCLUDED.is_liked est la nouvelle valeur que l'on tente d'insérer
            p
            );

        W.commit();

        std::cout << "Interaction enregistrée pour l'utilisateur ID " << user_id
                  << " et la chanson ID " << song_id
                  << " (Liked: " << (is_liked ? "Oui" : "Non") << ").\n";

    } catch (const pqxx::sql_error& e) {
        std::cerr << "Erreur SQL lors de l'enregistrement de l'interaction : " << e.what() << std::endl;
        std::cerr << "Requete : " << e.query() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Erreur non-SQL lors de l'enregistrement de l'interaction : " << e.what() << std::endl;
    }
}

std::vector<Song> AudioPlayer::get_liked_songs(const int user_id) {
    std::vector<Song> liked_songs;

    try {
        // Lecture seule
        pqxx::read_transaction R(*db_connection);

        pqxx::params p;
        p.append(user_id);

        // Requête SELECT avec jointure et filtre
        // ------------------------------------
        // Nous joignons 'song' et 'user_song_interaction' sur le champ song_id.
        // Nous filtrons pour l'utilisateur ($1) et uniquement les chansons aimées (is_liked = TRUE).
        // Nous sélectionnons les champs de la table 'song' pour construire nos objets Song.
        // ------------------------------------
        pqxx::result res = R.exec(
            "SELECT "
            "s.id, s.name, s.duration, s.artist_id "
            "FROM "
            "song s "
            "JOIN "
            "user_song_interaction usi ON s.id = usi.song_id "
            "WHERE "
            "usi.user_id = $1 AND usi.is_liked = TRUE",
            p
            );

        for (const auto& row : res) {
            Song s;
            s.set_id(row["id"].as<int>());
            s.set_title(row["name"].as<std::string>());
            s.set_duration(row["duration"].as<int>());
            s.set_artist_id(row["artist_id"].as<int>());

            liked_songs.push_back(s);
        }

        R.commit();

        std::cout << "Récupération réussie de " << liked_songs.size()
                  << " chansons aimées pour l'utilisateur ID " << user_id << ".\n";

    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de la récupération des chansons aimées : " << e.what() << std::endl;
        // Retourne un vecteur vide en cas d'erreur.
    }

    return liked_songs;
}

std::vector<Song> AudioPlayer::search_song_by_title(const std::string& query) {
    std::vector<Song> results;

    // Début du chronomètre
    auto start = std::chrono::high_resolution_clock::now();

    try {
        pqxx::read_transaction R(*db_connection);
        pqxx::params p;
        // On ajoute % autour du terme pour chercher "contient ce mot"
        // Note pour le prof : Pour utiliser pleinement l'index standard B-Tree,
        // une recherche par préfixe (ex: "Mot%") est idéale.
        // Ici on fait une recherche flexible.
        std::string search_term = query + "%";
        p.append(search_term);

        // ILIKE est insensible à la casse (Majuscule/minuscule)
        pqxx::result res = R.exec("SELECT id, name, duration, artist_id FROM song WHERE name = $1",p);

        for (const auto& row : res) {
            Song s;
            s.set_id(row["id"].as<int>());
            s.set_title(row["name"].as<std::string>());
            s.set_duration(row["duration"].as<int>());
            s.set_artist_id(row["artist_id"].as<int>());
            results.push_back(s);
        }

        // Fin du chronomètre (après récupération des données)
        auto end = std::chrono::high_resolution_clock::now();

        // Calcul de la durée en millisecondes
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "\n--- STATISTIQUES DE PERFORMANCE ---\n";
        std::cout << "Recherche terminee en : " << elapsed.count() << " ms\n";
        std::cout << "Nombre de resultats trouves : " << results.size() << "\n";
        std::cout << "-----------------------------------\n";

    } catch (const std::exception& e) {
        std::cerr << "Erreur recherche: " << e.what() << std::endl;
    }

    return results;
}
