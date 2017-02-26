#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;

string img_name = "";
bool obj = false;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->lineEdit_num_cols_del->setEnabled(false);
	ui->lineEdit_num_cols_insert->setEnabled(false);
	ui->lineEdit_num_rows_del->setEnabled(false);
	ui->lineEdit_num_rows_insert->setEnabled(false);
	ui->pushButton_proceed->setEnabled(false);
	ui->pushButton_save_rem->setEnabled(false);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_choose_image_clicked()
{
	QString fname = QFileDialog::getOpenFileName(this,"Choose Image","",tr("Image Files (*.png *.jpg *.bmp)"));
	if (fname == ""){
		QMessageBox::information(this, tr("Usage Information"),tr("Please choose an image file to continue."));
	} else {
		img_name = fname.toStdString();
		Mat img = imread(img_name);

		ui->label_num_cols->setText(QString::fromStdString(to_string(img.cols)));
		ui->label_num_rows->setText(QString::fromStdString(to_string(img.rows)));

		ui->lineEdit_num_cols_del->setValidator(new QIntValidator(0,img.cols-2,this));
		ui->lineEdit_num_rows_del->setValidator(new QIntValidator(0,img.rows-2,this));

		ui->lineEdit_num_cols_insert->setValidator(new QIntValidator(0,img.cols,this));
		ui->lineEdit_num_rows_insert->setValidator(new QIntValidator(0,img.rows,this));

		ui->lineEdit_num_cols_del->setEnabled(true);
		ui->lineEdit_num_cols_insert->setEnabled(true);
		ui->lineEdit_num_rows_del->setEnabled(true);
		ui->lineEdit_num_rows_insert->setEnabled(true);
		ui->pushButton_proceed->setEnabled(true);
		ui->pushButton_save_rem->setEnabled(true);

		ui->pushButton_choose_image->setText("Choose another Image");
	}
}

void MainWindow::on_pushButton_save_rem_clicked()
{
	QMessageBox::information(this, tr("Usage Information"),tr("Press right mouse button and drag over an object to delete it. Similarly, press left mouse button and drag over an object to save it."));
	obj = true;
	obj_img = imread(img_name);
	bin_img = Mat::zeros(obj_img.rows, obj_img.cols,obj_img.type());
    namedWindow(draw_winname,WINDOW_AUTOSIZE);
	setMouseCallback( draw_winname, obj_draw, NULL);
	imshow(draw_winname, obj_img);
	waitKey();
	destroyAllWindows();
}

void MainWindow::on_pushButton_proceed_clicked()
{
	ui->lineEdit_num_cols_del->setEnabled(false);
	ui->lineEdit_num_cols_insert->setEnabled(false);
	ui->lineEdit_num_rows_del->setEnabled(false);
	ui->lineEdit_num_rows_insert->setEnabled(false);
	ui->pushButton_proceed->setEnabled(false);
	ui->pushButton_save_rem->setEnabled(false);

	Mat img = imread(img_name);
	int rows_to_delete = ui->lineEdit_num_rows_del->text().toInt();
	int cols_to_delete = ui->lineEdit_num_cols_del->text().toInt();

	img = seamNormal(img,cols_to_delete,VER);
	img = seamNormal(img,rows_to_delete,HOR);
	imwrite("../DIA_Ass4/Seam Carving Deletion.jpg",img);
	imshow("Seam Deletion",img);
	waitKey();
	destroyAllWindows();
	obj = false;

	int rows_to_insert = ui->lineEdit_num_rows_insert->text().toInt();
	int cols_to_insert = ui->lineEdit_num_cols_insert->text().toInt();
	img = seamInsertion(img,cols_to_insert,VER);
	img = seamInsertion(img,rows_to_insert,HOR);
	imwrite("../DIA_Ass4/Seam Carving Insertion.jpg",img);
	imshow("Seam Insertion",img);
	waitKey();
	destroyAllWindows();

	img_name = "";
}
