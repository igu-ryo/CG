#define FREEGLUT_STATIC
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <GL/glut.h>


// 2次元ベクトルを扱うためのクラス
class Vector2d {
public:
	double x, y;
	Vector2d() { x = y = 0; }
	Vector2d(double _x, double _y) { x = _x; y = _y; }
	void set(double _x, double _y) { x = _x; y = _y; }

	// 長さを1に正規化する
	void normalize() {
		double len = length();
		x /= len; y /= len;
	}

	// 長さを返す
	double length() { return sqrt(x * x + y * y); }

	// s倍する
	void scale(const double s) { x *= s; y *= s; }

	// 加算の定義
	Vector2d operator+(Vector2d v) { return Vector2d(x + v.x, y + v.y); }

	// 減算の定義
	Vector2d operator-(Vector2d v) { return Vector2d(x - v.x, y - v.y); }

	// 内積の定義
	double operator*(Vector2d v) { return x * v.x + y * v.y; }

	// 代入演算の定義
	Vector2d& operator=(const Vector2d& v) { x = v.x; y = v.y; return (*this); }

	// 加算代入の定義
	Vector2d& operator+=(const Vector2d& v) { x += v.x; y += v.y; return (*this); }

	// 減算代入の定義
	Vector2d& operator-=(const Vector2d& v) { x -= v.x; y -= v.y; return (*this); }

	// 値を出力する
	void print() { printf("Vector2d(%f %f)\n", x, y); }
};

// マイナスの符号の付いたベクトルを扱えるようにするための定義 例：b=(-a); のように記述できる
Vector2d operator-(const Vector2d& v) { return(Vector2d(-v.x, -v.y)); }

// ベクトルと実数の積を扱えるようにするための定義 例： c=5*a+2*b; c=b*3; のように記述できる
Vector2d operator*(const double& k, const Vector2d& v) { return(Vector2d(k * v.x, k * v.y)); }
Vector2d operator*(const Vector2d& v, const double& k) { return(Vector2d(v.x * k, v.y * k)); }

// ベクトルを実数で割る操作を扱えるようにするための定義 例： c=a/2.3; のように記述できる
Vector2d operator/(const Vector2d& v, const double& k) { return(Vector2d(v.x / k, v.y / k)); }

// ================================================================================================


std::vector<Vector2d> g_ControlPoints; // 制御点を格納する

// ノットベクトルの要素数
const int NUM_NOT = 17;

// ノットベクトル
// この配列の値を変更することで基底関数が変化する。その結果として形が変わる。
// 下の例では、一定間隔で値が変化するので、「一様Bスプライン曲線」となる
double g_NotVector[] = { 0, 0, 0, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1, 1, 1};

const int spline_d = 3; // スプラインの次数
int spline_seg;// スプラインのセグメント数

#define OUTPUT_FILENAME "parametric_surface.obj" // 出力ファイル名
#define PI 3.1415926535

#define NUM_U 50  // U方向の分割数
#define NUM_V 50  // V方向の分割数

double x[NUM_U + 1][NUM_V + 1]; // x 座標
double y[NUM_U + 1][NUM_V + 1]; // y 座標
double z[NUM_U + 1][NUM_V + 1]; // z 座標

double getBaseN(int i, int n, double t);

// 配列に座標値を設定する
void setCoordinates() {

	for (int i = 0; i < NUM_U + 1; i++) {
		for (int j = 0; j < NUM_V + 1; j++) {

			// (0 <= u <= 2*PI), (g_NotVector[spline_d] <= v <= g_NotVector[spline_d + spline_seg])
			double u = 2.0 * PI / NUM_U * i;
			double v = g_NotVector[spline_d] + (g_NotVector[spline_d + spline_seg] - g_NotVector[spline_d] - 0.0001) / NUM_U * j;

			Vector2d p;
			for (int k = 0; k <= spline_d + spline_seg - 1; k++)
			{
				p += getBaseN(k, spline_d, v) * g_ControlPoints[k];
			}
			

			// 座標値の設定
			x[i][j] = p.x * cos(-u);
			y[i][j] = p.y;
			z[i][j] = p.x * sin(-u);

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


// 基底関数 N{i,n}(t)の値を計算する
double getBaseN(int i, int n, double t) {
	if (n == 0) {
		// n が 0 の時だけ t の値に応じて 0 または 1 を返す
		if (t >= g_NotVector[i] && t < g_NotVector[i + 1]) {
			return 1.0;
		}
		return 0;
	}
	else {
		// ★ここに必要なプログラムコードを記述する
		// ★再帰（自分自身の関数 getBaseN を呼ぶ処理が必要）
		// ★係数を計算するときに、ノットが重なる（分母がゼロとなる）ときには、その項を無視する。
		double f, f1, f2;
		f1 = (g_NotVector[i + n] - g_NotVector[i]) != 0
			? (t - g_NotVector[i]) / (g_NotVector[i + n] - g_NotVector[i]) * getBaseN(i, n - 1, t)
			: 0;
		f2 = (g_NotVector[i + n + 1] - g_NotVector[i + 1]) != 0
			? (g_NotVector[i + n + 1] - t) / (g_NotVector[i + n + 1] - g_NotVector[i + 1]) * getBaseN(i + 1, n - 1, t)
			: 0;
		f = f1 + f2;
		return f;
	}
}


// 表示部分をこの関数で記入
void display(void) {
	glClearColor(1.0, 1.0, 1.0, 1.0);  // 消去色指定
	glClear(GL_COLOR_BUFFER_BIT);     // 画面消去

	// 制御点の描画
	glPointSize(5);
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < g_ControlPoints.size(); i++) {
		glVertex2d(g_ControlPoints[i].x, g_ControlPoints[i].y);
	}
	glEnd();

	// 制御点を結ぶ線分の描画
	glColor3d(1.0, 0.0, 0.0);
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < g_ControlPoints.size(); i++) {
		glVertex2d(g_ControlPoints[i].x, g_ControlPoints[i].y);
	}
	glEnd();

	spline_seg = (int)g_ControlPoints.size() - spline_d; // スプラインのセグメント数

	if (spline_seg > 0)
	{
		glColor3d(0.0, 0.0, 0.0);
		glLineWidth(1);
		glBegin(GL_LINE_STRIP);


		for (double t = g_NotVector[spline_d]; t < g_NotVector[spline_d + spline_seg]; t += 0.01)
		{
			Vector2d p;
			for (int i = 0; i <= spline_d + spline_seg - 1; i++)
			{
				p += getBaseN(i, spline_d, t) * g_ControlPoints[i];
			}
			glVertex2d(p.x, p.y);
		}


		glEnd();
	}


	glutSwapBuffers();
}


void resizeWindow(int w, int h) {
	h = (h == 0) ? 1 : h;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// ウィンドウ内の座標系設定
	// マウスクリックの座標と描画座標が一致するような正投影
	glOrtho(0, w, h, 0, -10, 10);

	glMatrixMode(GL_MODELVIEW);
}

// キーボードイベント処理
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':
		exit(0);  /* '\033' は ESC の ASCII コード */
		break;
	case 's':
		setCoordinates();
		exportOBJ();
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

// マウスイベント処理
void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
			// クリックした位置に制御点を追加
			// ノット数を増やせばいくらでも制御点を追加できるが、今回はNUM_NOTの値で固定されているので
			// いくらでも追加できるわけではない
			if (g_ControlPoints.size() < NUM_NOT - 4) {
				g_ControlPoints.push_back(Vector2d(x, y));
			}
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			// 末尾の制御点の削除
			if (!g_ControlPoints.empty()) {
				g_ControlPoints.pop_back();
			}
			break;
		default:
			break;
		}
		glutPostRedisplay(); // 再描画
	}
}

// メインプログラム
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);          // ライブラリの初期化
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // 描画モードの指定
	glutInitWindowSize(800, 800);  // ウィンドウサイズを指定
	glutCreateWindow(argv[0]);      // ウィンドウを作成
	glutDisplayFunc(display);       // 表示関数を指定
	glutReshapeFunc(resizeWindow);  // ウィンドウサイズが変更されたときの関数を指定
	glutKeyboardFunc(keyboard);     // キーボードイベント処理関数を指定
	glutMouseFunc(mouse);           // マウスイベント処理関数を指定
	glutMainLoop();                 // イベント待ち
	return 0;
}
