#include "Mandelbrout.h"

int DrawMandelbrot ();

int main ()
{
	DrawMandelbrot ();
	return 0;
}

int DrawMandelbrot ()
{
	txCreateWindow (size_x, size_y);
	const float offset_x = -1.325f, offset_y = 0.f; //�������� �������� ��� ��������� � �������� �������� ������
	float delta_x = 1 / 800.f, delta_y = 1 / 800.f, delta_s = 1/800.f; //��� ����� �� ����������� ���������
	const float R = 100.f; //������
	float x_center = 0.f, y_center = 0.f, scale = 1.f; // �������� ������������ ������

	txBegin ();

	while (!(txGetAsyncKeyState(VK_ESCAPE)))
	{
		if (txGetAsyncKeyState ('W')) y_center -= delta_y * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f); //�� ����������, � ���� �� ����������!
		if (txGetAsyncKeyState ('A')) x_center -= delta_x * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('S')) y_center += delta_y * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('D')) x_center += delta_x * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('P')) scale    += delta_s * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('O')) scale    -= delta_s * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);

		delta_x *= scale;
		delta_y *= scale;

		for (int y_coord = 0; y_coord < size_y; y_coord++)
		{
			float Re = (                - (float) size_x/2) * delta_x + offset_x + x_center; //��������� ���������� X �� ����������� ���������
			float Im = ((float) y_coord - (float) size_y/2) * delta_y + offset_y + y_center; //��������� ���������� Y �� ����������� ���������

			for (int x_coord = 0; x_coord < size_x; x_coord++, Re += delta_x)
			{
				int n_color = 0; // ������� ����� ����� � ������������������, �� �� ����� �����
				float Re_i = Re, Im_i = Im;

				for (; n_color < n_max; n_color++)
				{
					float Re_2 = Re_i * Re_i, Im_2 = Im_i * Im_i, ImRe = Im_i * Re_i;
					float R_2  = Re_2 + Im_2;

					if (R_2 >= R) break;

					Re_i = Re_2 - Im_2 + Re; //������� ���������� � ������� ������������ ����� � ���������� ���������� ��������
					Im_i = ImRe + ImRe + Im;
				}

				txSetPixel (x_coord, y_coord, RGB (n_color, n_color, n_color));
			}

		}

		txSleep ();
		printf ("fps %.1f\r", txGetFPS ());
	}

	return 0;
}