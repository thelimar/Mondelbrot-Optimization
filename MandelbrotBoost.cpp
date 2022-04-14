#include <intrin.h>
#include "Mandelbrout.h"

int DrawMandelbroutBoost ();

int main ()
{
	return DrawMandelbroutBoost ();
}

int DrawMandelbroutBoost()
{
	
	txCreateWindow(size_x, size_y);
	const float offset_x = -1.325f, offset_y = 0.f; //поправка значений для попадания в диапазон значений фигуры
	float delta_x = 1 / 800.f, delta_y = 1 / 800.f, delta_s = 1 / 800.f; //шаг точек на комплексной плоскости
	const float R = 100.f; //радиус
	float x_center = 0.f, y_center = 0.f, scale = 1.f; // смещение относительно центра

	__m128 Radius = _mm_set_ps1 (R);

	txBegin ();
	
	while (!(txGetAsyncKeyState(VK_ESCAPE)))
	{
		if (txGetAsyncKeyState ('W')) y_center -= delta_y * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f); //не скопировал, а взял на вооружение!
		if (txGetAsyncKeyState ('A')) x_center -= delta_x * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('S')) y_center += delta_y * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('D')) x_center += delta_x * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('P')) scale    += delta_s * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
		if (txGetAsyncKeyState ('O')) scale    -= delta_s * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);

		delta_x *= scale;
		delta_y *= scale;

		for (int y_coord = 0; y_coord < size_y; y_coord++)
		{	
			float Re = (                - (float) size_x/2) * delta_x + offset_x + x_center; //начальная координата X на комплексной плоскости
			float Im = ((float) y_coord - (float) size_y/2) * delta_y + offset_y + y_center; //начальная координата Y на комплексной плоскости

			__m128 Ims = _mm_set_ps1 (Im); //упакованные 4 начальных значения мнимой части

			//обработка четырех точек на одной вертикали с различием по горизонтали на delta_x
			for (int x_coord = 0; x_coord < size_x; x_coord += 4, Re += 4 * delta_x) 
			{
				__m128 Res = _mm_setr_ps (Re, Re + delta_x, Re + delta_x * 2, Re + delta_x * 3);
				__m128i colors = _mm_setzero_si128 (); //4 значения текущего номера числа в последовательности, он же номер цвета
				
				__m128 cmp_vector = _mm_cmpeq_ps (cmp_vector, cmp_vector); //маска, показывающая не вышла ли точка за радиус

				__m128 Res_i = Res;
				__m128 Ims_i = Ims;

				for (int n_color = 0; n_color < n_max && _mm_movemask_ps (cmp_vector); n_color++)
				{
					__m128 Res_2   = _mm_mul_ps (Res_i, Res_i);
					__m128 Ims_2   = _mm_mul_ps (Ims_i, Ims_i);
					__m128 Res_Ims = _mm_mul_ps (Res_i, Ims_i);
					__m128 R_2     = _mm_add_ps (Res_2, Ims_2);

					cmp_vector = _mm_cmple_ps (R_2, Radius);

					colors = _mm_sub_epi32 (colors, _mm_castps_si128 (cmp_vector));
					
					//эквивалентно псевдокоду: if (cmp_vector[k] = 0) Res_i[k] = Res_i[k]; else Res_i[k] = Res_2[k] - Ims_2[k] + Res
					Res_i = _mm_add_ps (_mm_andnot_ps (cmp_vector, Res_i), _mm_and_ps (_mm_add_ps (_mm_sub_ps (Res_2, Ims_2), Res), cmp_vector));
					Ims_i = _mm_add_ps (_mm_andnot_ps (cmp_vector, Ims_i), _mm_and_ps (_mm_add_ps (_mm_add_ps (Res_Ims, Res_Ims), Ims), cmp_vector));

				}

				int32_t* colors_mas = (int32_t*) (&colors);

				for (int i = 0; i < 4; i++)
					txSetPixel (x_coord + i, y_coord, RGB (colors_mas[i], colors_mas[i], colors_mas[i]));
			}
		}

		txSleep ();
		printf ("fps %.1f\r", txGetFPS ());
	}

	return 0;
}
