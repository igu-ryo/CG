#pragma warning(disable : 4996)
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define OUTPUT_FILENAME "parametric_surface.obj" // �o�̓t�@�C����
#define PI 3.1415926535

#define NUM_U 50  // U�����̕�����
#define NUM_V 50  // V�����̕�����

double x[NUM_U + 1][NUM_V + 1]; // x ���W
double y[NUM_U + 1][NUM_V + 1]; // y ���W
double z[NUM_U + 1][NUM_V + 1]; // z ���W

// �z��ɍ��W�l��ݒ肷��
void setCoordinates() {

	for (int i = 0; i < NUM_U + 1; i++) {
		for (int j = 0; j < NUM_V + 1; j++) {

			// u �� v �̒l�� 0.0 �` 1.0 �ɐ��K������
			double u = 1.0 / NUM_U * i;
			double v = 1.0 / NUM_V * j;

			// ���W�l�̐ݒ� �� �ȉ��̗�ł�z=0�̕��ʂɂȂ�
			x[i][j] = u;
			y[i][j] = v;
			z[i][j] = 0;
		}
	}
}

// OBJ �`���ł̃t�@�C���̏o��
void exportOBJ() {
	FILE* fout;

	// �o�͗p�t�@�C�����J��
	//---------------------------
	fout = fopen(OUTPUT_FILENAME, "w");
	if (fout == NULL) {
		printf("Error: fopen\n");
		exit(0);
	}


	// Visual Studio �ŏ�L�ł͂��܂������Ȃ��ꍇ���̂��̂ɒu��������
/*
	// �o�͗p�t�@�C�����J��
	//---------------------------
	if(0 != fopen_s(&fout, OUTPUT_FILENAME, "w") ) {
		printf("Error: fopen \n");
		exit(0);
	}
*/

// ���_���̏o��
//---------------------------
	for (int i = 0; i < NUM_U + 1; i++) {
		for (int j = 0; j < NUM_V + 1; j++) {
			// �e���_�̍��W�l���o�͂���
			fprintf(fout, "v %f %f %f\n", x[i][j], y[i][j], z[i][j]);
		}
	}

	// �ʏ��̏o��
	/*
		�� �ʏ����o�͂���ɂ́A�ȉ��̃R�����g�A�E�g���O����
	*/
	//---------------------------
	/*
	for(int i = 0; i < NUM_U; i++) {
		for(int j = 0; j < NUM_V; j++) {
			// ���ꂼ��̊i�q�ɑ΂���2�̎O�p�`���o�͂���

			// OBJ�`���ł͒��_�ԍ���1����n�܂�
			int lb_index = // �����̒��_�ԍ��� i,j,NUM_U,NUM_V�̒l����v�Z����
			int lt_index = // ����̒��_�ԍ��̌v�Z��
			int rb_index = // �E���̒��_�ԍ��̌v�Z��
			int rt_index = // �E��̒��_�ԍ��̌v�Z��

			// �O�p�`���\�����钸�_�ԍ����o��
			fprintf(fout, "f %d %d %d\n", lb_index, rt_index, lt_index);
			fprintf(fout, "f %d %d %d\n", lb_index, rb_index, rt_index);

		}
	}
	*/

	// �o�͗p�t�@�C�������
	//---------------------------
	fclose(fout);
}

int main(int argc, char** argv) {
	setCoordinates();
	exportOBJ();
	return 0;
}