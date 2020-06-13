#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define OUTPUT_FILENAME "parametric_surface.obj" // 出力ファイル名
#define PI 3.1415926535

#define NUM_U 50  // U方向の分割数
#define NUM_V 50  // V方向の分割数

double x[NUM_U + 1][NUM_V + 1]; // x 座標
double y[NUM_U + 1][NUM_V + 1]; // y 座標
double z[NUM_U + 1][NUM_V + 1]; // z 座標

// 配列に座標値を設定する
void setCoordinates() {

	for (int i = 0; i < NUM_U + 1; i++) {
		for (int j = 0; j < NUM_V + 1; j++) {

			// u と v の値を 0.0 〜 1.0 に正規化する
			double u = 2.0 * PI / NUM_U * i;
			double v = 2.0 * PI / NUM_U * j;

			// 座標値の設定 ★ 以下の例ではz=0の平面になる
			x[i][j] = 2 * cos(u) + cos(v) * cos(u);
			y[i][j] = 2 * sin(u) + cos(v) * sin(u);
			z[i][j] = sin(v);

		}
	}
}

// OBJ 形式でのファイルの出力
void exportOBJ() {
	FILE* fout;

	// 出力用ファイルを開く
	//---------------------------
	if(0 != fopen_s(&fout, OUTPUT_FILENAME, "w") ) {
		printf("Error: fopen \n");
		exit(0);
	}

	// 頂点情報の出力
	//---------------------------
	for (int i = 0; i < NUM_U + 1; i++) {
		for (int j = 0; j < NUM_V + 1; j++) {
			// 各頂点の座標値を出力する
			fprintf(fout, "v %f %f %f\n", x[i][j], y[i][j], z[i][j]);
		}
	}

	// 面情報の出力
	//---------------------------
	for(int i = 0; i < NUM_U; i++) {
		for(int j = 0; j < NUM_V; j++) {
			// それぞれの格子に対して2つの三角形を出力する

			// OBJ形式では頂点番号は1から始まる
			int lb_index = i * (NUM_V + 1) + (j + 1); // 左下の頂点番号を i,j,NUM_U,NUM_Vの値から計算する
			int lt_index = i * (NUM_V + 1) + (j + 2);// 左上の頂点番号の計算式
			int rb_index = (i + 1) * (NUM_V + 1) + (j + 1);// 右下の頂点番号の計算式
			int rt_index = (i + 1) * (NUM_V + 1) + (j + 2);// 右上の頂点番号の計算式

			// 三角形を構成する頂点番号を出力
			fprintf(fout, "f %d %d %d\n", lb_index, rt_index, lt_index);
			fprintf(fout, "f %d %d %d\n", lb_index, rb_index, rt_index);

		}
	}
	

	// 出力用ファイルを閉じる
	//---------------------------
	fclose(fout);
}

int main(int argc, char** argv) {
	setCoordinates();
	exportOBJ();
	return 0;
}