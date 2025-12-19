#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>

// Inclusions de ta logique métier
#include "audio_player.h"
#include "authManager.h"
#include <pqxx/pqxx>
#include <memory>

const std::string DB_CONN_STRING =
    "dbname=audio_player "
    "user=lounou "
    "password=clemence15lcd "
    "host=localhost "
    "port=5432";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- Gestion des traductions (généré par Qt) ---
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "bibliotheque_audio_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    try {
        // 1. Initialisation BDD
        auto db_conn = std::make_unique<pqxx::connection>(DB_CONN_STRING);
        if (!db_conn->is_open()) {
            throw std::runtime_error("Impossible d'ouvrir la base de données.");
        }

        // 2. Initialisation des Services
        // On crée AudioPlayer (qui prend possession de la connexion)
        // Note: Vérifie si ton constructeur AudioPlayer accepte std::unique_ptr ou un pointeur brut
        AudioPlayer audio_player(std::move(db_conn));

        // On crée AuthManager
        AuthManager auth_manager(audio_player);

        // 3. Lancement de la fenêtre principale en lui passant auth_manager
        MainWindow w(nullptr, &auth_manager);
        w.show();

        return a.exec();

    } catch (const std::exception &e) {
        // En cas d'erreur (BDD non lancée, etc.), on affiche une alerte graphique
        QMessageBox::critical(nullptr, "Erreur au démarrage", e.what());
        return 1;
    }
}
