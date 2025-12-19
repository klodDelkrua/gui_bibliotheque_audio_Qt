#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "authManager.h"
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent, AuthManager *auth)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , authService(auth) // On affecte le pointeur reçu au membre de la classe
{
    ui->setupUi(this);

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

