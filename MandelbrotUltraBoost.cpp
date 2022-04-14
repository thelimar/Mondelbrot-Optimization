#include <intrin.h>
#include <immintrin.h>
#include "Mandelbrout.h"

int DrawMandelbroutUltraBoost ();

int main  ()
{
	return DrawMandelbroutUltraBoost ();
}

int DrawMandelbroutUltraBoost ()
{

	txCreateWindow(size_x, size_y);
	const float offset_x = -1.325f, offset_y = 0.f; //поправка значений для попадания в диапазон значений фигуры
	float delta_x = 1 / 800.f, delta_y = 1 / 800.f, delta_s = 1 / 800.f; //шаг точек на комплексной плоскости
	const float R = 100.f; //радиус
	float x_center = 0.f, y_center = 0.f, scale = 1.f; // смещение относительно центра

	__m256 Radius = _mm256_set1_ps (R);

	txBegin ();
	while  (! (txGetAsyncKeyState (VK_ESCAPE)))
	{
		if  (txGetAsyncKeyState ('W')) y_center -= delta_y *  (txGetAsyncKeyState (VK_SHIFT) ? 100.f : 10.f); //не скопировал, а взял на вооружение!
		if  (txGetAsyncKeyState ('A')) x_center -= delta_x *  (txGetAsyncKeyState (VK_SHIFT) ? 100.f : 10.f);
		if  (txGetAsyncKeyState ('S')) y_center += delta_y *  (txGetAsyncKeyState (VK_SHIFT) ? 100.f : 10.f);
		if  (txGetAsyncKeyState ('D')) x_center += delta_x *  (txGetAsyncKeyState (VK_SHIFT) ? 100.f : 10.f);
		if  (txGetAsyncKeyState ('P')) scale    += delta_s *  (txGetAsyncKeyState (VK_SHIFT) ? 100.f : 10.f);
		if  (txGetAsyncKeyState ('O')) scale    -= delta_s *  (txGetAsyncKeyState (VK_SHIFT) ? 100.f : 10.f);

		delta_x *= scale;
		delta_y *= scale;

		for  (int y_coord = 0; y_coord < size_y; y_coord++)
		{
			float Re = (                - (float) size_x/2) * delta_x + offset_x + x_center; //начальная координата X на комплексной плоскости
			float Im = ((float) y_coord - (float) size_y/2) * delta_y + offset_y + y_center; //начальная координата Y на комплексной плоскости

			__m256 Ims = _mm256_set1_ps (Im); //упакованные 8 начальных значений мнимой части

			//обработка восьми точек на одной вертикали с различием по горизонтали на delta_x
			for  (int x_coord = 0; x_coord < size_x; x_coord += 8, Re += 8 * delta_x) 
			{
				__m256 Res = _mm256_setr_ps (Re, Re + delta_x, Re + delta_x * 2, Re + delta_x * 3, Re + delta_x * 4,
											 Re + delta_x * 5, Re + delta_x * 6, Re + delta_x * 7);
				__m256i colors = _mm256_setzero_si256 (); //8 значений текущего номера числа в последовательности, он же номер цвета
				__m256 cmp_vector = _mm256_setzero_ps (); //маска, показывающая не вышла ли точка за радиус

				cmp_vector = _mm256_cmp_ps (cmp_vector, cmp_vector, 0);

				__m256 Res_i = Res;
				__m256 Ims_i = Ims;

				for  (int n_color = 0; n_color < n_max && _mm256_movemask_ps (cmp_vector); n_color++)
				{
					__m256 Res_2 = _mm256_mul_ps (Res_i, Res_i);
					__m256 Ims_2 = _mm256_mul_ps (Ims_i, Ims_i);
					__m256 Res_Ims = _mm256_mul_ps (Res_i, Ims_i);
					__m256 R_2 = _mm256_add_ps (Res_2, Ims_2);

					cmp_vector = _mm256_cmp_ps (R_2, Radius, 2);

					colors = _mm256_sub_epi32 (colors, _mm256_castps_si256 (cmp_vector));

					//эквивалентно псевдокоду: if (cmp_vector[k] = 0) Res_i[k] = Res_i[k]; else Res_i[k] = Res_2[k] - Ims_2[k] + Res
					Res_i = _mm256_add_ps (_mm256_andnot_ps (cmp_vector, Res_i), 
						                   _mm256_and_ps (_mm256_add_ps (_mm256_sub_ps (Res_2, Ims_2), Res), cmp_vector));

					Ims_i = _mm256_add_ps (_mm256_andnot_ps (cmp_vector, Ims_i), 
										   _mm256_and_ps (_mm256_add_ps (_mm256_add_ps (Res_Ims, Res_Ims), Ims), cmp_vector));
				}

				int32_t* colors_mas =  (int32_t*) (&colors);

				for  (int i = 0; i < 8; i++)
					txSetPixel (x_coord + i, y_coord, RGB (colors_mas[i], colors_mas[i], colors_mas[i]));
			}
		}

		printf("fps %.1f\r", txGetFPS());
		txSleep ();
	}

	return 0;
}
