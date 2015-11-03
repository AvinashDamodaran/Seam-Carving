#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <vector>
#include "seam.h"

extern std::string img_name;
extern bool obj;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void on_pushButton_choose_image_clicked();

	void on_pushButton_save_rem_clicked();

	void on_pushButton_proceed_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
