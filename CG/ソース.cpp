#define FREEGLUT_STATIC
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <GL/glut.h>


// 2�����x�N�g�����������߂̃N���X
class Vector2d {
public:
	double x, y;
	Vector2d() { x = y = 0; }
	Vector2d(double _x, double _y) { x = _x; y = _y; }
	void set(double _x, double _y) { x = _x; y = _y; }

	// ������1�ɐ��K������
	void normalize() {
		double len = length();
		x /= len; y /= len;
	}

	// ������Ԃ�
	double length() { return sqrt(x * x + y * y); }

	// s�{����
	void scale(const double s) { x *= s; y *= s; }

	// ���Z�̒�`
	Vector2d operator+(Vector2d v) { return Vector2d(x + v.x, y + v.y); }

	// ���Z�̒�`
	Vector2d operator-(Vector2d v) { return Vector2d(x - v.x, y - v.y); }

	// ���ς̒�`
	double operator*(Vector2d v) { return x * v.x + y * v.y; }

	// ������Z�̒�`
	Vector2d& operator=(const Vector2d& v) { x = v.x; y = v.y; return (*this); }

	// ���Z����̒�`
	Vector2d& operator+=(const Vector2d& v) { x += v.x; y += v.y; return (*this); }

	// ���Z����̒�`
	Vector2d& operator-=(const Vector2d& v) { x -= v.x; y -= v.y; return (*this); }

	// �l���o�͂���
	void print() { printf("Vector2d(%f %f)\n", x, y); }
};

// �}�C�i�X�̕����̕t�����x�N�g����������悤�ɂ��邽�߂̒�` ��Fb=(-a); �̂悤�ɋL�q�ł���
Vector2d operator-(const Vector2d& v) { return(Vector2d(-v.x, -v.y)); }

// �x�N�g���Ǝ����̐ς�������悤�ɂ��邽�߂̒�` ��F c=5*a+2*b; c=b*3; �̂悤�ɋL�q�ł���
Vector2d operator*(const double& k, const Vector2d& v) { return(Vector2d(k * v.x, k * v.y)); }
Vector2d operator*(const Vector2d& v, const double& k) { return(Vector2d(v.x * k, v.y * k)); }

// �x�N�g���������Ŋ��鑀���������悤�ɂ��邽�߂̒�` ��F c=a/2.3; �̂悤�ɋL�q�ł���
Vector2d operator/(const Vector2d& v, const double& k) { return(Vector2d(v.x / k, v.y / k)); }

// ================================================================================================


std::vector<Vector2d> g_ControlPoints; // ����_���i�[����

// �m�b�g�x�N�g���̗v�f��
const int NUM_NOT = 17;

// �m�b�g�x�N�g��
// ���̔z��̒l��ύX���邱�ƂŊ��֐����ω�����B���̌��ʂƂ��Č`���ς��B
// ���̗�ł́A���Ԋu�Œl���ω�����̂ŁA�u��lB�X�v���C���Ȑ��v�ƂȂ�
double g_NotVector[] = { 0, 0, 0, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1, 1, 1};

const int spline_d = 3; // �X�v���C���̎���
int spline_seg;// �X�v���C���̃Z�O�����g��

#define OUTPUT_FILENAME "parametric_surface.obj" // �o�̓t�@�C����
#define PI 3.1415926535

#define NUM_U 50  // U�����̕�����
#define NUM_V 50  // V�����̕�����

double x[NUM_U + 1][NUM_V + 1]; // x ���W
double y[NUM_U + 1][NUM_V + 1]; // y ���W
double z[NUM_U + 1][NUM_V + 1]; // z ���W

double getBaseN(int i, int n, double t);

// �z��ɍ��W�l��ݒ肷��
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
			

			// ���W�l�̐ݒ�
			x[i][j] = p.x * cos(-u);
			y[i][j] = p.y;
			z[i][j] = p.x * sin(-u);

		}
	}
}

// OBJ �`���ł̃t�@�C���̏o��
void exportOBJ() {
	FILE* fout;

	// �o�͗p�t�@�C�����J��
	//---------------------------
	if(0 != fopen_s(&fout, OUTPUT_FILENAME, "w") ) {
		printf("Error: fopen \n");
		exit(0);
	}

	// ���_���̏o��
	//---------------------------
	for (int i = 0; i < NUM_U + 1; i++) {
		for (int j = 0; j < NUM_V + 1; j++) {
			// �e���_�̍��W�l���o�͂���
			fprintf(fout, "v %f %f %f\n", x[i][j], y[i][j], z[i][j]);
		}
	}

	// �ʏ��̏o��
	//---------------------------
	for(int i = 0; i < NUM_U; i++) {
		for(int j = 0; j < NUM_V; j++) {
			// ���ꂼ��̊i�q�ɑ΂���2�̎O�p�`���o�͂���

			// OBJ�`���ł͒��_�ԍ���1����n�܂�
			int lb_index = i * (NUM_V + 1) + (j + 1); // �����̒��_�ԍ��� i,j,NUM_U,NUM_V�̒l����v�Z����
			int lt_index = i * (NUM_V + 1) + (j + 2);// ����̒��_�ԍ��̌v�Z��
			int rb_index = (i + 1) * (NUM_V + 1) + (j + 1);// �E���̒��_�ԍ��̌v�Z��
			int rt_index = (i + 1) * (NUM_V + 1) + (j + 2);// �E��̒��_�ԍ��̌v�Z��

			// �O�p�`���\�����钸�_�ԍ����o��
			fprintf(fout, "f %d %d %d\n", lb_index, rt_index, lt_index);
			fprintf(fout, "f %d %d %d\n", lb_index, rb_index, rt_index);

		}
	}
	

	// �o�͗p�t�@�C�������
	//---------------------------
	fclose(fout);
}


// ���֐� N{i,n}(t)�̒l���v�Z����
double getBaseN(int i, int n, double t) {
	if (n == 0) {
		// n �� 0 �̎����� t �̒l�ɉ����� 0 �܂��� 1 ��Ԃ�
		if (t >= g_NotVector[i] && t < g_NotVector[i + 1]) {
			return 1.0;
		}
		return 0;
	}
	else {
		// �������ɕK�v�ȃv���O�����R�[�h���L�q����
		// ���ċA�i�������g�̊֐� getBaseN ���Ăԏ������K�v�j
		// ���W�����v�Z����Ƃ��ɁA�m�b�g���d�Ȃ�i���ꂪ�[���ƂȂ�j�Ƃ��ɂ́A���̍��𖳎�����B
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


// �\�����������̊֐��ŋL��
void display(void) {
	glClearColor(1.0, 1.0, 1.0, 1.0);  // �����F�w��
	glClear(GL_COLOR_BUFFER_BIT);     // ��ʏ���

	// ����_�̕`��
	glPointSize(5);
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < g_ControlPoints.size(); i++) {
		glVertex2d(g_ControlPoints[i].x, g_ControlPoints[i].y);
	}
	glEnd();

	// ����_�����Ԑ����̕`��
	glColor3d(1.0, 0.0, 0.0);
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < g_ControlPoints.size(); i++) {
		glVertex2d(g_ControlPoints[i].x, g_ControlPoints[i].y);
	}
	glEnd();

	spline_seg = (int)g_ControlPoints.size() - spline_d; // �X�v���C���̃Z�O�����g��

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

	// �E�B���h�E���̍��W�n�ݒ�
	// �}�E�X�N���b�N�̍��W�ƕ`����W����v����悤�Ȑ����e
	glOrtho(0, w, h, 0, -10, 10);

	glMatrixMode(GL_MODELVIEW);
}

// �L�[�{�[�h�C�x���g����
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':
		exit(0);  /* '\033' �� ESC �� ASCII �R�[�h */
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

// �}�E�X�C�x���g����
void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
			// �N���b�N�����ʒu�ɐ���_��ǉ�
			// �m�b�g���𑝂₹�΂�����ł�����_��ǉ��ł��邪�A�����NUM_NOT�̒l�ŌŒ肳��Ă���̂�
			// ������ł��ǉ��ł���킯�ł͂Ȃ�
			if (g_ControlPoints.size() < NUM_NOT - 4) {
				g_ControlPoints.push_back(Vector2d(x, y));
			}
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			// �����̐���_�̍폜
			if (!g_ControlPoints.empty()) {
				g_ControlPoints.pop_back();
			}
			break;
		default:
			break;
		}
		glutPostRedisplay(); // �ĕ`��
	}
}

// ���C���v���O����
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);          // ���C�u�����̏�����
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // �`�惂�[�h�̎w��
	glutInitWindowSize(800, 800);  // �E�B���h�E�T�C�Y���w��
	glutCreateWindow(argv[0]);      // �E�B���h�E���쐬
	glutDisplayFunc(display);       // �\���֐����w��
	glutReshapeFunc(resizeWindow);  // �E�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��̊֐����w��
	glutKeyboardFunc(keyboard);     // �L�[�{�[�h�C�x���g�����֐����w��
	glutMouseFunc(mouse);           // �}�E�X�C�x���g�����֐����w��
	glutMainLoop();                 // �C�x���g�҂�
	return 0;
}
