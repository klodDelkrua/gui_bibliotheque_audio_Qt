#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "authManager.h"
#include "audio_player.h"
#include "user_account.h"
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QLineEdit>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent, AuthManager *auth)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , authService(auth) // On affecte le pointeur reçu au membre de la classe
{
    ui->setupUi(this);
    setupTableSongs();

    // On vérifie quand même par sécurité
    if (!authService) {
        qDebug() << "Attention : authService est nul !";
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_return_to_main_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_11_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_sign_in_clicked()
{
    QString username = ui->editLogin->text();
    QString password = ui->editPassword->text();

    if(authService->login(username.toStdString(), password.toStdString())){
        QMessageBox::information(this, "Всё отлично", "Привет " + username + " !");
        ui->stackedWidget->setCurrentIndex(3);
    }else{
        QMessageBox::warning(this, "Error", "Password or Username not valide !");
    }
}


void MainWindow::on_pushButton_6_clicked()
{
    QString username = ui->inputName->text();
    QString email = ui->inputEmail->text();
    QString password = ui->inputPassword->text();
    QString confirm_password = ui->inputConfirm->text();

    if(username.isEmpty() || email.isEmpty() || password.isEmpty()){
        QMessageBox::warning(this, "Error", "Надо выполнять все поля");
        return;
    }
    if(password != confirm_password){
        QMessageBox::warning(this, "Error", "Пароли должны быть одиноково");
        return;
    }
    if(authService->register_user(username.toStdString(), password.toStdString(), email.toStdString())){
        QMessageBox::information(this, "Succes", "учетная запись создана успешно. Войдите !");
        ui->stackedWidget->setCurrentIndex(0);
    }else{
        QMessageBox::critical(this, "Не получилось", "Ошибка при создании...");
    }
}


void MainWindow::on_pushButton_18_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::setupTableSongs() {
    ui->tableSongs->setColumnCount(4);
    ui->tableSongs->setHorizontalHeaderLabels({"Titre", "Artiste", "Durée", "Favoris"});
    ui->tableSongs->verticalHeader()->setVisible(false);
    ui->tableSongs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableSongs->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableSongs->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MainWindow::displaySongsPage(bool showOnlyFavorites) {
    ui->tableSongs->setUpdatesEnabled(false);
    ui->tableSongs->setRowCount(0);

    AudioPlayer& player = authService->get_player();
    std::vector<Song> songs;

    // --- LOGIQUE DE SÉLECTION DU MODE ---
    if (showOnlyFavorites) {
        int userId = authService->get_current_user().get_id();
        songs = player.get_liked_songs(userId); // Ta méthode console
    } else {
        songs = player.get_all_songs();
    }

    ui->tableSongs->setRowCount(songs.size());

    for (int i = 0; i < (int)songs.size(); ++i) {
        // Colonne 0 : Titre + ID caché
        QTableWidgetItem* titleItem = new QTableWidgetItem(QString::fromStdString(songs[i].get_title()));
        titleItem->setData(Qt::UserRole, songs[i].get_id());
        ui->tableSongs->setItem(i, 0, titleItem);

        // Colonne 1 : Artiste
        ui->tableSongs->setItem(i, 1, new QTableWidgetItem(QString::number(songs[i].get_artist_id())));

        // Colonne 2 : Durée
        int totalSeconds = songs[i].get_duration();
        QString timeStr = QString("%1:%2").arg(totalSeconds / 60).arg(totalSeconds % 60, 2, 10, QChar('0'));
        ui->tableSongs->setItem(i, 2, new QTableWidgetItem(timeStr));

        // Colonne 3 : Favoris (On affiche un coeur si on est en mode favoris)
        ui->tableSongs->setItem(i, 3, new QTableWidgetItem(showOnlyFavorites ? "❤️" : ""));
    }

    ui->tableSongs->setUpdatesEnabled(true);
    ui->stackedWidget->setCurrentIndex(5);
}

void MainWindow::on_seeListSongs_clicked()
{
    displaySongsPage(false);
}

//charger les artists
void MainWindow::loadArtistsIntoCombo() {
    ui->comboArtists->clear();

    // On récupère l'instance d'AudioPlayer via ton service d'auth
    AudioPlayer& player = authService->get_player();
    std::vector<Artist> artists = player.get_all_artists();

    for (const auto& artist : artists) {
        // On affiche le nom, mais on stocke l'ID en donnée "cachée" (UserRole)
        ui->comboArtists->addItem(QString::fromStdString(artist.get_name()), artist.get_id());
    }
}

void MainWindow::on_btnAddSong_clicked()
{
    loadArtistsIntoCombo();
    ui->stackedWidget->setCurrentIndex(6);
}


void MainWindow::on_btnSaveSong_clicked()
{
    QString title = ui->editSongTitle->text().trimmed();
    int duration = ui->spinSongDuration->value();

    // On récupère l'ID caché dans l'item sélectionné du ComboBox
    int artistId = ui->comboArtists->currentData().toInt();

    if (title.isEmpty() || artistId <= 0) {
        QMessageBox::warning(this, "Champs vides", "Merci de saisir un titre et de choisir un artiste.");
        return;
    }

    try {
        AudioPlayer& player = authService->get_player();
        player.add_song(title.toStdString(), duration, artistId);

        QMessageBox::information(this, "Succès", "La chanson a été ajoutée à la base de données !");

        // Nettoyage et retour à la liste
        ui->editSongTitle->clear();
        displaySongsPage(); // On retourne voir la liste mise à jour
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur SQL", e.what());
    }
}


void MainWindow::on_btnAddArtist_clicked()
{
    bool ok;
    QString newName = QInputDialog::getText(this, "Nouvel Artiste",
                                            "Nom de l'artiste :", QLineEdit::Normal,
                                            "", &ok);
    if (ok && !newName.isEmpty()) {
        AudioPlayer& player = authService->get_player();
        int newId = player.add_artist(newName.toStdString());

        if (newId != -1) {
            loadArtistsIntoCombo(); // On recharge
            ui->comboArtists->setCurrentText(newName); // On sélectionne le nouveau automatiquement
        }
    }
}


void MainWindow::on_pushButton_12_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_btnDeleteSong_clicked()
{
    int currentRow = ui->tableSongs->currentRow();
    if(currentRow == -1){
        QMessageBox::warning(this, "Selection", "Пожалуйста, выберите песню из таблицы.");
        return;
    }
    int songId = ui->tableSongs->item(currentRow, 0)->data(Qt::UserRole).toInt();
    QString title = ui->tableSongs->item(currentRow, 0)->text();
    auto reply = QMessageBox::question(this, "Confirmation",
                                        "Вы хотите удалить " + title + "?",
                                       QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes){
        if(authService->get_player().delete_song(songId)){
            displaySongsPage();
        }
    }
}


void MainWindow::on_btnLikeSong_clicked()
{
    int currentRow = ui->tableSongs->currentRow();
    if(currentRow == -1){
        QMessageBox::warning(this, "Selection", "Выбирате песню.");
        return;
    }

    int songId = ui->tableSongs->item(currentRow, 0)->data(Qt::UserRole).toInt();
    int userId = authService->get_current_user().get_id();

    AudioPlayer& player = authService->get_player();
    player.like_song(userId, songId, true);
    ui->tableSongs->item(currentRow, 3)->setText("❤️");
    QMessageBox::information(this, "Succes", "Песня добавлена ​​в избранное!");
}


void MainWindow::on_btnLikedSong_clicked()
{
    displaySongsPage(true);
}

