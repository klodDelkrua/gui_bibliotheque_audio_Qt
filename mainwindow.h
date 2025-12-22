#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "authManager.h"
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QLineEdit>
#include <QInputDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QHeaderView>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // On change la signature pour accepter le pointeur vers AuthManager
    explicit MainWindow(QWidget *parent = nullptr, AuthManager* auth = nullptr);
    ~MainWindow();

private slots:
    // Tes slots existants (on ne les change pas)
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_7_clicked();
    void on_return_to_main_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_4_clicked();
    void on_sign_in_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_18_clicked();
    void setupTableSongs();
    // Ajoute un paramètre optionnel "showOnlyFavorites"
    void displaySongsPage(bool showOnlyFavorites = false);
    void on_seeListSongs_clicked();
    void loadArtistsIntoCombo();

    void on_btnAddSong_clicked();

    void on_btnSaveSong_clicked();

    void on_btnAddArtist_clicked();

    void on_pushButton_12_clicked();

    void on_btnDeleteSong_clicked();

    void on_btnLikeSong_clicked();

    void on_btnLikedSong_clicked();

    void on_btnSearch_clicked();

private:
    Ui::MainWindow *ui;
    // C'est ici que le pointeur sera stocké après avoir été reçu du main
    AuthManager* authService;
};
#endif // MAINWINDOW_H
