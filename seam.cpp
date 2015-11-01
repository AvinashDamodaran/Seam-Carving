#include "seam.h"

using namespace std;
using namespace cv;


//--------------------------------------------------------Seam Carving--------------------------------------//

Mat seamNormal(Mat img, int cols_to_delete, int mode, bool show){
	if (mode == HOR)
		img = img.t();

	if (cols_to_delete > img.cols-2){
		cout << "Too many seams to delete. Give correct parameters.\n";
		exit(0);
	}

	int wait = 60;
	for (int z=0;z<cols_to_delete;z++){

		// Calculate Laplacian
		Mat blured, gray, dst, grad;
		GaussianBlur( img, blured, Size(3,3), 0, 0, BORDER_DEFAULT );
		cvtColor( blured, gray, CV_BGR2GRAY );
		Laplacian( gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT );
		convertScaleAbs( dst, grad );

		vector< vector<node> > table(0);
		for (int i=0;i<img.rows;i++){
			vector<node> temp(0);
			for (int j=0;j<img.cols;j++){
				node n{0,grad.at<uchar>(i,j)};

				auto it = save.find(Point(j,i));
				if (it != save.end()){
					n.energy = INT_MAX;
				} else{
					it = rem.find(Point(j,i));
					if (it != rem.end())
						n.energy = INT_MIN;
				}

				temp.push_back(n);
			}
			table.push_back(temp);
		}

		for (int i=0;i<img.rows;i++){
			for (int j=0;j<img.cols;j++){
				if (i==0){
					continue;
				} else if (j==0){		// extreme left
					long long int mid = table[i-1][j].energy;
					long long int right = table[i-1][j+1].energy;
					long long int min_energy = min(mid,right);
					table[i][j].energy += min_energy;
					if (min_energy == mid)
						table[i][j].path = 0;
					else
						table[i][j].path = 1;
				} else if (j==img.cols-1){		// extreme right
					long long int mid = table[i-1][j].energy;
					long long int left = table[i-1][j-1].energy;
					long long int min_energy = min(mid,left);
					table[i][j].energy += min_energy;
					if (min_energy == mid)
						table[i][j].path = 0;
					else
						table[i][j].path = -1;
				} else {
					long long int mid = table[i-1][j].energy;
					long long int left = table[i-1][j-1].energy;
					long long int right = table[i-1][j+1].energy;
					long long int min_energy = min(right,min(mid,left));
					table[i][j].energy += min_energy;
					if (min_energy == mid)
						table[i][j].path = 0;
					else if (min_energy == left)
						table[i][j].path = -1;
					else
						table[i][j].path = 1;
				}
			}
		}

		// find the column corresponding to minimum energy
		int min_col = 0;
		long long int min_energy = table[img.rows-1][0].energy;
		for (int i=1;i<img.cols;i++){
			if (table[img.rows-1][i].energy < min_energy){
				min_energy = table[img.rows-1][i].energy;
				min_col = i;
			}
		}

		if (show){
			// paint the corresponding seam RED
			int index = min_col;
			for (int j=img.rows-1; j>=0; j--){
				img.at<Vec3b>(j,index) = {0,0,255};
				index += table[j][index].path;
			}

			// paint save points
			qDebug() << "save size : " << save.size();
			for (auto it = save.begin(); it != save.end(); it++){
				Point temp = *it;
				img.at<Vec3b>(temp.y,temp.x) = {0,255,0};
			}

			// paint rem points
			qDebug() << "rem size : " << rem.size();
			for (auto it = rem.begin(); it != rem.end(); it++){
				Point temp = *it;
				img.at<Vec3b>(temp.y,temp.x) = {0,0,255};
			}
		}

		if (show){
			if (mode == HOR){
				img = img.t();
			}

			imshow("Seam Carving",img);
			waitKey(wait);

			if (mode == HOR){
				img = img.t();
			}
		}



		// update save/remove
		int index = min_col;
		for (int i=img.rows-1;i>=0;i--){
			auto it = save.find(Point(index,i));
			if (it != save.end()){
				save.erase(it);
			} else {
				it = rem.find(Point(index,i));
				if (it != rem.end()){
					rem.erase(it);
				}
			}
			index += table[i][index].path;
		}

		index = min_col;
		for (int i=img.rows-1;i>=0;i--){
			for (int j=index+1;j<img.cols;j++){
				auto it = save.find(Point(j,i));
				if (it != save.end()){
					save.erase(it);
					save.insert(Point(j-1,i));
				} else {
					it = rem.find(Point(j,i));
					if (it != rem.end()){
						rem.erase(it);
						rem.insert(Point(j-1,i));
					}
				}
			}
			index += table[i][index].path;
		}

		// remove the corresponding column
		index = min_col;
		for (int i=img.rows-1;i>=0;i--){
			for (int j=index;j<img.cols-1;j++){
				img.at<Vec3b>(i,j) = img.at<Vec3b>(i,j+1);
			}
			index += table[i][index].path;
		}
		Rect crop_region(0, 0, img.cols-1, img.rows);
		img = img(crop_region);
	}

	if (mode == HOR)
		img =img.t();

	return img;
}



//--------------------------------------------------Pyramids-----------------------------------------------//

Mat viewPyramids(vector<Mat> pyramids){
	int rows = pyramids[0].rows;
	int cols = 0;
	for (unsigned int i=0;i<pyramids.size();i++){
		cols += pyramids[i].cols;
	}

	Mat ret = Mat::zeros(rows,cols,pyramids[0].type());

	int col_index = 0;
	for (unsigned int i=0;i<pyramids.size();i++){
		pyramids[i].copyTo(ret(Rect(col_index, 0, pyramids[i].cols, pyramids[i].rows)));
		col_index += pyramids[i].cols;
	}
	return ret;
}

// return the downsampled image(at a above level of pyramid)
Mat pyUp(Mat img){
	Mat ret;
	GaussianBlur(img,ret,Size(5,5),0);
	resize(ret,ret,Size(img.cols/2,img.rows/2));
	return ret;
}

vector<Mat> GPyramids(Mat img, int num_levels){
	vector<Mat> ret(1,img);
	for (int i=1;i<num_levels;i++){
		Mat temp_img = pyUp(ret.back());
		ret.push_back(temp_img);
	}
	return ret;
}

vector<Mat> LPyramids(vector<Mat> gaussianPyramids){
	vector<Mat> ret(0);
	for (unsigned int i=0;i<gaussianPyramids.size();i++){
		Mat blured, gray, dst, abs_dst;
		GaussianBlur( gaussianPyramids[i], blured, Size(3,3), 0, 0, BORDER_DEFAULT );
		cvtColor( blured, gray, CV_BGR2GRAY );
		Laplacian( gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT );
		convertScaleAbs( dst, abs_dst );
		ret.push_back(abs_dst);
	}
	return ret;
}


//--------------------------------------------------Seam Carving with Pyramids-----------------------------------------------//

pair< Mat , vector< vector< coord > > > seam(Mat img, int cols_to_delete){
	vector< vector< coord > > seams(0);

	if (cols_to_delete > img.cols-2){
		cout << "Too many seams to delete. Give correct parameters.\n";
		exit(0);
	}

	for (int z=0;z<cols_to_delete;z++){

		vector< coord > new_seam(0);

		// Calculate Laplacian
		Mat blured, gray, dst, grad;
		GaussianBlur( img, blured, Size(3,3), 0, 0, BORDER_DEFAULT );
		cvtColor( blured, gray, CV_BGR2GRAY );
		Laplacian( gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT );
		convertScaleAbs( dst, grad );


		vector< vector<node> > table(0);
		for (int i=0;i<img.rows;i++){
			vector<node> temp(0);
			for (int j=0;j<img.cols;j++){
				node n{0,grad.at<uchar>(i,j)};
				temp.push_back(n);
			}
			table.push_back(temp);
		}

		for (int i=0;i<img.rows;i++){
			for (int j=0;j<img.cols;j++){
				if (i==0){
					continue;
				} else if (j==0){		// extreme left
					long long int mid = table[i-1][j].energy;
					long long int right = table[i-1][j+1].energy;
					long long int min_energy = min(mid,right);
					table[i][j].energy += min_energy;
					if (min_energy == mid)
						table[i][j].path = 0;
					else
						table[i][j].path = 1;
				} else if (j==img.cols-1){		// extreme right
					long long int mid = table[i-1][j].energy;
					long long int left = table[i-1][j-1].energy;
					long long int min_energy = min(mid,left);
					table[i][j].energy += min_energy;
					if (min_energy == mid)
						table[i][j].path = 0;
					else
						table[i][j].path = -1;
				} else {
					long long int mid = table[i-1][j].energy;
					long long int left = table[i-1][j-1].energy;
					long long int right = table[i-1][j+1].energy;
					long long int min_energy = min(right,min(mid,left));
					table[i][j].energy += min_energy;
					if (min_energy == mid)
						table[i][j].path = 0;
					else if (min_energy == left)
						table[i][j].path = -1;
					else
						table[i][j].path = 1;
				}
			}
		}
		// find the column corresponding to minimum energy
		int min_col = 0;
		long long int min_energy = table[img.rows-1][0].energy;
		for (int i=1;i<img.cols;i++){
			if (table[img.rows-1][i].energy < min_energy){
				min_energy = table[img.rows-1][i].energy;
				min_col = i;
			}
		}

		// remove the corresponding column
		int index = min_col;
		for (int i=img.rows-1;i>=0;i--){
			for (int j=index;j<img.cols-1;j++){
				img.at<Vec3b>(i,j) = img.at<Vec3b>(i,j+1);
			}
			index += table[i][index].path;
		}

		Rect crop_region(0, 0, img.cols-1, img.rows);
		img = img(crop_region);

		seams.push_back(new_seam);
	}
	return make_pair(img , seams);
}



// return image with seams removed to given image
Mat removeSeams(Mat img, vector< vector< coord > > seams){
	Mat ret = img.clone();
	for (unsigned int i=0;i<seams.size();i++){
		for (unsigned int j=0;j<seams[i].size();j++){
			int row = seams[i][j].first;
			int col = seams[i][j].second;
			for (int k=col;k<img.cols;k++){
				ret.at<Vec3b>(row,k) = ret.at<Vec3b>(row,k+1);
			}
		}
		Rect crop_region(0, 0, ret.cols-1, ret.rows);
		ret = ret(crop_region);
	}
	return ret;
}

// return seams at lower level given seams of upper level
vector< vector< coord > > mapSeams(vector< vector< coord > > seams){
	vector< vector< coord > > seams_ret(0);
	for (unsigned int i=0;i<seams.size();i++){
		vector< coord > new_seam(0);
		for (unsigned int j=0;j<seams[i].size();j++){
			int a = seams[i][j].first;
			int b = seams[i][j].second;
			new_seam.push_back(make_pair(2*a,2*b));
			new_seam.push_back(make_pair(2*a+1,2*b));
		}
		seams_ret.push_back(new_seam);
		new_seam.clear();
		for (unsigned int j=0;j<seams[i].size();j++){
			int a = seams[i][j].first;
			int b = seams[i][j].second;
			new_seam.push_back(make_pair(2*a,2*b+1));
			new_seam.push_back(make_pair(2*a+1,2*b+1));
		}
		seams_ret.push_back(new_seam);
	}
	return seams_ret;
}

Mat seamPyramid(vector<Mat> &gaussians, int cols_to_delete)
{
	int num_levels = gaussians.size();
	vector<int> seam_cols_to_delete(num_levels);
	vector< vector< coord > > seams_deleted_at_previous_level(0);
	for(int l=num_levels-1;l>=0;l--){
		cout << "At level : "<< l << endl;
		int sigma = 0;
		for(int j=l+1;j<num_levels;j++){
			sigma += seam_cols_to_delete[j] * int(pow(2 , j));
		}

		seam_cols_to_delete[l] = int((cols_to_delete - sigma) / pow(2 , l));

		// remove seams because of previous level
		vector < vector < coord > > mapped_seams = mapSeams(seams_deleted_at_previous_level);
		gaussians[l] = removeSeams(gaussians[l],mapped_seams);

		pair< Mat , vector< vector< coord > > > result = seam(gaussians[l] , seam_cols_to_delete[l]);

		gaussians[l] = result.first;
		vector< vector< coord > > seams_deleted_by_seam_carving = result.second;

		cout << "Num Mapped Seams : " << mapped_seams.size() << endl;
		cout << "Num Seams of Seam Carving : " << seams_deleted_by_seam_carving.size();
		cout << "\n\n";

		seams_deleted_at_previous_level.clear();
		seams_deleted_at_previous_level.insert( seams_deleted_at_previous_level.end(),
																						mapped_seams.begin(), mapped_seams.end() );

		seams_deleted_at_previous_level.insert( seams_deleted_at_previous_level.end(),
																						seams_deleted_by_seam_carving.begin(),
																						seams_deleted_by_seam_carving.end() );
	}
	return gaussians[0];

}

//------------------------------ Draw save/remove objects-----------------//

bool left_button_down = false;
bool right_button_down = false;
Mat obj_img;
set<Point,pointComp> save;
set<Point,pointComp> rem;
string draw_winname = "Draw to explicitly save/remove objects";

void obj_draw(int event, int x, int y, int flags, void* param){
//	qDebug() << "These are to save size : " << save.size();
//	qDebug() << "These are to remove size : " << rem.size();
	if (event == CV_EVENT_LBUTTONDOWN){
		left_button_down = true;
	}else if (event == CV_EVENT_RBUTTONDOWN){
		right_button_down = true;
	} else if (event == CV_EVENT_MOUSEMOVE && right_button_down){
		int rad = 4;
		circle(obj_img, Point(x,y), rad, RED, -1);
		for (int i=-rad;i<=rad;i++){
			for (int j=-rad;j<=rad;j++){
				if ((x+i)>=0 && (x+i)<obj_img.cols && (y+j)>=0 && (y+j)<obj_img.rows)
					rem.insert(Point(x+i,y+j));
			}
		}
		imshow(draw_winname,obj_img);
	} else if (event == CV_EVENT_MOUSEMOVE && left_button_down){
		int rad = 4;
		circle(obj_img, Point(x,y), rad, GREEN, -1);
		for (int i=-rad;i<=rad;i++){
			for (int j=-rad;j<=rad;j++){
				if ((x+i)>=0 && (x+i)<obj_img.cols && (y+j)>=0 && (y+j)<obj_img.rows)
					save.insert(Point(x+i,y+j));
			}
		}
		imshow(draw_winname,obj_img);
	} else if (event == CV_EVENT_LBUTTONUP){
		left_button_down = false;
	} else if (event == CV_EVENT_RBUTTONUP){
		right_button_down = false;
	}
}
