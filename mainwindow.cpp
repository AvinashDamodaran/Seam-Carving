#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;

string img_name = "";
int num_levels = 3;
bool with_impy = false;

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

void MainWindow::on_pushButton_gpy_clicked()
{
	Mat img = imread(img_name);
	vector<Mat> gpy = GPyramids(img, num_levels);
	Mat gpyimg = viewPyramids(gpy);
	imshow("Gaussian Pyramids",gpyimg);
	waitKey();
	destroyAllWindows();
}

void MainWindow::on_pushButton_lpy_clicked()
{
	Mat img = imread(img_name);
	vector<Mat> gpy = GPyramids(img, num_levels);
	vector<Mat> lpy = LPyramids(gpy);
	Mat lpyimg = viewPyramids(lpy);
	imshow("Laplacian Pyramids",lpyimg);
	waitKey();
	destroyAllWindows();
}

void MainWindow::on_checkBox_clicked(bool checked)
{
	with_impy = checked;
}

void MainWindow::on_pushButton_proceed_clicked()
{
	Mat img = imread(img_name);
	int rows_to_delete = ui->lineEdit_num_rows_del->text().toInt();
	int cols_to_delete = ui->lineEdit_num_cols_del->text().toInt();
	if (with_impy){
		vector<Mat> gpy = GPyramids(img, num_levels);
		img = seamPyramid(gpy,cols_to_delete);
		if (rows_to_delete > 0){
			img = img.t();
			vector<Mat> gpy = GPyramids(img, num_levels);
			img = seamPyramid(gpy,rows_to_delete);
			img = img.t();
		}
		imwrite("../DIA_Ass4/Seam Carving with pyramids.jpg",img);
	} else {
		img = seamNormal(img,cols_to_delete,VER,true);
		img = seamNormal(img,rows_to_delete,HOR,true);
		imshow("Seam Carving",img);
		imwrite("../DIA_Ass4/Seam Carving Normal.jpg",img);
	}
	imshow("Seam Carving",img);
	waitKey();
	destroyAllWindows();
}

void MainWindow::on_pushButton_save_rem_clicked()
{
	obj_img = imread(img_name);
	namedWindow(draw_winname,WINDOW_AUTOSIZE | WINDOW_OPENGL);
	setMouseCallback( draw_winname, obj_draw, NULL);
	imshow(draw_winname, obj_img);
	waitKey();
	destroyAllWindows();
}
