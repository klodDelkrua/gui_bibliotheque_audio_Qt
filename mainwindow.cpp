#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "authManager.h"
#include "audio_player.h"
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
    ui->tableSongs->setColumnCount(3);
    ui->tableSongs->setHorizontalHeaderLabels({"Titre", "Artiste ID", "Durée"});

    // 1. Étirer les colonnes pour prendre toute la place
    ui->tableSongs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 2. Cacher les numéros de ligne à gauche (les 1, 2, 3...)
    ui->tableSongs->verticalHeader()->setVisible(false);

    // 3. Rendre le tableau non-modifiable (lecture seule)
    ui->tableSongs->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 4. Sélectionner toute la ligne lors d'un clic
    ui->tableSongs->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::displaySongsPage() {
    // 1. On vide le tableau
    ui->tableSongs->setRowCount(0);

    // 2. Récupération des données via ton AudioPlayer
    AudioPlayer& player = authService->get_player();
    std::vector<Song> songs = player.get_all_songs();

    // 3. Remplissage ligne par ligne
    for (int i = 0; i < songs.size(); ++i) {
        ui->tableSongs->insertRow(i);

        // Colonne Titre
        ui->tableSongs->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(songs[i].get_title())));

        // Colonne Artiste ID
        ui->tableSongs->setItem(i, 1, new QTableWidgetItem(QString::number(songs[i].get_artist_id())));

        // Colonne Durée
        ui->tableSongs->setItem(i, 2, new QTableWidgetItem(QString::number(songs[i].get_duration())));
        int totalSeconds = songs[i].get_duration();
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        QString timeStr = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
        ui->tableSongs->setItem(i, 2, new QTableWidgetItem(timeStr));
    }

    // 4. Changer de page (remplace INDEX_PAGE_SONGS par le numéro de ta nouvelle page)
    ui->stackedWidget->setCurrentIndex(5);
}

void MainWindow::on_seeListSongs_clicked()
{
    displaySongsPage();
}

//charger les artists
