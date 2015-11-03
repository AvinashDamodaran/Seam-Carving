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
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_choose_image_clicked()
{
	QString fname = QFileDialog::getOpenFileName(this,"Choose Image","",tr("Image Files (*.png *.jpg *.bmp)"));
	if (fname == ""){
		QMessageBox::information(this, tr("Usage Information"),tr("Please choose an image file. Exitting program."));
		QApplication::exit();
	} else {
		img_name = fname.toStdString();
		Mat img = imread(img_name);

		ui->label_num_cols->setText(QString::fromStdString(to_string(img.cols)));
		ui->label_num_rows->setText(QString::fromStdString(to_string(img.rows)));

		ui->lineEdit_num_cols_del->setValidator(new QIntValidator(0,img.cols-2,this));
		ui->lineEdit_num_rows_del->setValidator(new QIntValidator(0,img.rows-2,this));

		imshow("Image File",img);
		waitKey();
		destroyAllWindows();
	}
}

void MainWindow::on_pushButton_save_rem_clicked()
{
	obj = true;
	obj_img = imread(img_name);
	bin_img = Mat::zeros(obj_img.rows, obj_img.cols,obj_img.type());
	namedWindow(draw_winname,WINDOW_AUTOSIZE | WINDOW_OPENGL);
	setMouseCallback( draw_winname, obj_draw, NULL);
	imshow(draw_winname, obj_img);
	waitKey();
	destroyAllWindows();
}

void MainWindow::on_pushButton_proceed_clicked()
{
	Mat img = imread(img_name);
	int rows_to_delete = ui->lineEdit_num_rows_del->text().toInt();
	int cols_to_delete = ui->lineEdit_num_cols_del->text().toInt();

	img = seamNormal(img,cols_to_delete,VER);
	img = seamNormal(img,rows_to_delete,HOR);
	imwrite("../DIA_Ass4/Seam Carving Normal.jpg",img);
	imshow("Seam Carving",img);
	waitKey();
	destroyAllWindows();
	obj = false;
}
