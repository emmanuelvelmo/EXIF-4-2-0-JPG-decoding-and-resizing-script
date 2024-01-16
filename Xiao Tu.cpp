#include<iostream>
#include<filesystem>
#include<fstream>
#include<string>
#include<functional>
#include<cmath>
#include<vector>
#include<optional>
#include<cstdlib>
#include<Windows.h>

int main()
{
    std::cout << "First store all images in a folder in Desktop named 'gallerydir' and press any key to continue\n";
    system("pause");

    wchar_t w_usr[256];
    DWORD tam_w_usr = 256;

    GetUserName(w_usr, &tam_w_usr);
    std::string n_usr(w_usr, w_usr + tam_w_usr);
    n_usr.pop_back();

    if (std::filesystem::is_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/"))
    {
        unsigned short n = 0;

        for (std::filesystem::directory_entry dcim_dir : std::filesystem::directory_iterator("C:/Users/" + n_usr + "/Desktop/gallerydir/"))
        {
            if (dcim_dir.path().extension() == ".jpg")
            {
                n++;
            }
            if (n == 1)
            {
                break;
            }
        }

        if (n == 1)
        {
            if (!std::filesystem::is_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/"))
            {
                std::filesystem::create_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/");
            }

            n = 0;

            for (std::filesystem::directory_entry dcim_dir : std::filesystem::directory_iterator("C:/Users/" + n_usr + "/Desktop/gallerydir/"))
            {
                if (dcim_dir.path().extension() == ".jpg")
                {
                    unsigned int i;
                    unsigned short m = 0;

                    std::ifstream jpg_ent(dcim_dir.path().string(), std::ios::binary);

                    jpg_ent.seekg(0, jpg_ent.end);
                    unsigned int jpg_size = jpg_ent.tellg();
                    jpg_ent.seekg(0, jpg_ent.beg);

                    char* jpg_arr = new char[jpg_size];
                    jpg_ent.read(jpg_arr, jpg_size);

                    unsigned char* jpg_reint = reinterpret_cast<unsigned char*>(jpg_arr);

                    for (i = 0; i < jpg_size; i++)
                    {
                        if (jpg_reint[i] == 0xFF && jpg_reint[i + 1] == 0xC0 && jpg_reint[i + 2] == 0x00 && jpg_reint[i + 3] == 0x11)
                        {
                            m++;

                            if (m == 2)
                            {
                                i = i + 5;
                                break;
                            }
                        }
                    }

                    if (m == 2)
                    {
                        float alto_in = jpg_reint[i] * 256.0f + jpg_reint[i + 1];
                        float ancho_in = jpg_reint[i + 2] * 256.0f + jpg_reint[i + 3];

                        if (ancho_in >= 1920 && alto_in > 1080 || ancho_in > 1920 && alto_in >= 1080 || ancho_in >= 1080 && alto_in > 1920 || ancho_in > 1080 && alto_in >= 1920)
                        {
                            float ancho_aj, alto_aj, rel_lado;

                            if (ancho_in > alto_in)
                            {
                                rel_lado = ancho_in * (9.0f / 16);
                            }
                            else
                            {
                                rel_lado = alto_in * (9.0f / 16);
                            }

                            if (ancho_in > alto_in)
                            {
                                if (rel_lado > alto_in)
                                {
                                    ancho_aj = alto_in * (16.0f / 9), alto_aj = alto_in;
                                }
                                else
                                {
                                    ancho_aj = ancho_in, alto_aj = ancho_in * (9.0f / 16);
                                }
                            }
                            else
                            {
                                if (rel_lado > ancho_in)
                                {
                                    ancho_aj = ancho_in, alto_aj = ancho_in * (16.0f / 9);
                                }
                                else
                                {
                                    ancho_aj = alto_in * (9.0f / 16), alto_aj = alto_in;
                                }
                            }

                            if (ancho_aj - int(ancho_aj) > 0)
                            {
                                ancho_aj = int(ancho_aj) + 1;
                            }
                            if (alto_aj - int(alto_aj) > 0)
                            {
                                alto_aj = int(alto_aj) + 1;
                            }

                            if (int(ancho_in - ancho_aj) & 1)
                            {
                                ancho_aj -= 1;
                            }
                            if (int(alto_in - alto_aj) & 1)
                            {
                                alto_aj -= 1;
                            }

                            m = 0;
                            unsigned int db;
                            unsigned int db2;
                            for (db = 0; db < jpg_size; db++)
                            {
                                if (jpg_reint[db] == 0xFF && jpg_reint[db + 1] == 0xDB && jpg_reint[db + 2] == 0x00 && jpg_reint[db + 3] == 0x84)
                                {
                                    m++;

                                    if (m == 2)
                                    {
                                        db2 = db + 2;

                                        while (true)
                                        {
                                            if (jpg_reint[db2] == 0xFF && jpg_reint[db2 + 1] == 0xC0 && jpg_reint[db2 + 2] == 0x00 && jpg_reint[db2 + 3] == 0x11)
                                            {
                                                db2--;
                                                break;
                                            }

                                            db2++;
                                        }

                                        break;
                                    }
                                }
                            }

                            if (m != 2)
                            {
                                break;
                            }

                            unsigned char lum_tb[64];
                            unsigned char chr_tb[64];
                            if (jpg_reint[db + 2] * 256 + jpg_reint[db + 3] > 67)
                            {
                                unsigned short z_z[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

                                for (m = 0; m < 64; m++)
                                {
                                    lum_tb[z_z[m]] = jpg_reint[db + 5 + m];
                                }

                                for (m = 0; m < 64; m++)
                                {
                                    chr_tb[z_z[m]] = jpg_reint[db + 70 + m];
                                }
                            }

                            m = 0;
                            unsigned int c0;
                            unsigned int c02;
                            for (c0 = 0; c0 < jpg_size; c0++)
                            {
                                if (jpg_reint[c0] == 0xFF && jpg_reint[c0 + 1] == 0xC0 && jpg_reint[c0 + 2] == 0x00 && jpg_reint[c0 + 3] == 0x11)
                                {
                                    m++;

                                    if (m == 2)
                                    {
                                        c02 = c0 + 2;

                                        while (true)
                                        {
                                            if (jpg_reint[c02] == 0xFF && jpg_reint[c02 + 1] == 0xDA && jpg_reint[c02 + 2] == 0x00 && jpg_reint[c02 + 3] == 0x0C)
                                            {
                                                c02--;
                                                break;
                                            }

                                            c02++;
                                        }

                                        break;
                                    }
                                }
                            }

                            unsigned char pcsn_bit = jpg_reint[c0 + 2];
                            unsigned char comp_img = jpg_reint[c0 + 7];
                            unsigned char cuant_y_01[2] = { jpg_reint[c0 + 9], jpg_reint[c0 + 10] };
                            unsigned char hor_cbcr_02[2] = { jpg_reint[c0 + 12], jpg_reint[c0 + 13] };
                            unsigned char ver_cbcr_03[2] = { jpg_reint[c0 + 15], jpg_reint[c0 + 16] };

                            m = 0;
                            unsigned int c4;
                            unsigned int c42;
                            for (c4 = 0; c4 < jpg_size; c4++)
                            {
                                if (jpg_reint[c4] == 0xFF && jpg_reint[c4 + 1] == 0xC4 && jpg_reint[c4 + 2] == 0x01 && jpg_reint[c4 + 3] == 0xA2)
                                {
                                    m++;

                                    if (m == 2)
                                    {
                                        c42 = c4 + 2;

                                        while (true)
                                        {
                                            if (jpg_reint[c42] == 0xFF && jpg_reint[c42 + 1] == 0xDB && jpg_reint[c42 + 2] == 0x00 && jpg_reint[c42 + 3] == 0x84)
                                            {
                                                c42--;
                                                break;
                                            }

                                            c42++;
                                        }

                                        break;
                                    }
                                }
                            }

                            if (m != 2)
                            {
                                break;
                            }

                            unsigned char dht_00[16];
                            std::string lum_dc_nod;

                            unsigned char dht_01[16];
                            std::string chr_dc_nod;

                            unsigned char dht_10[16];
                            std::string lum_ac_nod;

                            unsigned char dht_11[16];
                            std::string chr_ac_nod;

                            unsigned int i2 = 0;
                            unsigned short m2;
                            unsigned char ch_ch;

                            for (m = 0; m < 16; m++)
                            {
                                i2 = i2 + jpg_reint[c4 + 5 + m];
                            }

                            if (18 + i2 < c42 - c4)
                            {
                                unsigned short m3, m4 = 0;

                                i2 = m4;
                                for (m = 0; m < 16; m++)
                                {
                                    dht_00[m] = jpg_reint[c4 + 5 + i2 + m];

                                    if (jpg_reint[c4 + 5 + i2 + m] >= 1)
                                    {
                                        i = jpg_reint[c4 + 5 + i2 + m];
                                        for (m2 = 0; m2 < i; m2++, m3++, m4++)
                                        {
                                            ch_ch = jpg_reint[c4 + 21 + i2 + m3];
                                            lum_dc_nod.push_back(ch_ch);
                                        }
                                    }
                                }

                                m3 = 0;
                                i2 = m4;
                                for (m = 0; m < 16; m++)
                                {
                                    dht_01[m] = jpg_reint[c4 + 22 + i2 + m];

                                    if (jpg_reint[c4 + 22 + i2 + m] >= 1)
                                    {
                                        i = jpg_reint[c4 + 22 + i2 + m];
                                        for (m2 = 0; m2 < i; m2++, m3++, m4++)
                                        {
                                            ch_ch = jpg_reint[c4 + 38 + i2 + m3];
                                            chr_dc_nod.push_back(ch_ch);
                                        }
                                    }
                                }

                                m3 = 0;
                                i2 = m4;
                                for (m = 0; m < 16; m++)
                                {
                                    dht_10[m] = jpg_reint[c4 + 39 + i2 + m];

                                    if (jpg_reint[c4 + 39 + i2 + m] >= 1)
                                    {
                                        i = jpg_reint[c4 + 39 + i2 + m];
                                        for (m2 = 0; m2 < i; m2++, m3++, m4++)
                                        {
                                            ch_ch = jpg_reint[c4 + 55 + i2 + m3];
                                            lum_ac_nod.push_back(ch_ch);
                                        }
                                    }
                                }

                                m3 = 0;
                                i2 = m4;
                                for (m = 0; m < 16; m++)
                                {
                                    dht_11[m] = jpg_reint[c4 + 56 + i2 + m];

                                    if (jpg_reint[c4 + 56 + i2 + m] >= 1)
                                    {
                                        i = jpg_reint[c4 + 56 + i2 + m];
                                        for (m2 = 0; m2 < i; m2++, m3++, m4++)
                                        {
                                            ch_ch = jpg_reint[c4 + 72 + i2 + m3];
                                            chr_ac_nod.push_back(ch_ch);
                                        }
                                    }
                                }
                            }

                            m = 0;
                            unsigned int da;
                            unsigned int da2;
                            for (da = 0; da < jpg_size; da++)
                            {
                                if (jpg_reint[da] == 0xFF && jpg_reint[da + 1] == 0xDA && jpg_reint[da + 2] == 0x00 && jpg_reint[da + 3] == 0x0C)
                                {
                                    m++;

                                    if (m == 2)
                                    {
                                        da2 = da + 2;

                                        m2 = 0;
                                        i2 = da2;
                                        for (i = 0; i < jpg_size - da2; i++, i2++)
                                        {
                                            if (jpg_reint[i2] == 0xFF && jpg_reint[i2 + 1] == 0xD9)
                                            {
                                                m2++;
                                            }
                                        }

                                        m = 0;
                                        while (true)
                                        {
                                            if (jpg_reint[da2] == 0xFF && jpg_reint[da2 + 1] == 0xD9)
                                            {
                                                m++;

                                                if (m == m2)
                                                {
                                                    da2--;
                                                    break;
                                                }
                                            }

                                            da2++;
                                        }

                                        break;
                                    }
                                }
                            }

                            if (m != 2)
                            {
                                break;
                            }

                            unsigned char comp_y[2] = { jpg_reint[da + 5], jpg_reint[da + 6] };
                            unsigned char comp_cb[2] = { jpg_reint[da + 7], jpg_reint[da + 8] };
                            unsigned char comp_cr[2] = { jpg_reint[da + 9], jpg_reint[da + 10] };
                            unsigned char ss = jpg_reint[da + 11];
                            unsigned char se = jpg_reint[da + 12];
                            unsigned char ah_ai = jpg_reint[da + 13];
                            
                            //DECODIFICAR (Comenzar por Huffman - FFDA)



                            std::string rgb_no_aj;
                            
                            /*CONVERSIÓN DE COLOR*/

                            /* YCbCr to RGB
                            unsigned short conv_R = ((298.082 * [Y]) / 256) + ((408.583 * [Cr]) / 256) - (222.921)
                            unsigned short conv_G = ((298.082 * [Y]) / 256) - ((100.29 * [Cb]) / 256) - ((208.120 * [Cr]) / 256) + (135.576)
                            unsigned short conv_B = ((298.082 * [Y]) / 256) + ((516.412 * [Cb]) / 256) - (276.836)
                            
                            RGB to YCbCr
                            unsigned short conv_Y = (16) + ((65.738 * [R]) / 256) + ((129.057 * [G]) / 256) + ((25.064 * [B]) / 256)
                            unsigned short conv_Cb = (128) - ((37.945 * [R]) / 256) - ((74.494 * [G]) / 256) + ((112.439 * [B]) / 256)
                            unsigned short conv_Cr = (128) + (112.439 * [R]) - ((94.154 * [G]) / 256) - ((18.285 * [B]) / 256) */



                            if (jpg_reint[78] == 0x06)
                            {
                                std::string rgb_no_aj_2 = rgb_no_aj;

                                for (unsigned short filas_y = 0; filas_y < alto_in; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_in; columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_no_aj[(filas_y * ancho_in * 3) + (columnas_x * 3) + cont_rgb] = rgb_no_aj_2[(ancho_in * alto_in * 3) - (ancho_in * 3) - (columnas_x * ancho_in * 3) + (filas_y * 3) + cont_rgb];
                                        }
                                    }
                                }
                            }

                            if (jpg_reint[78] == 0x03)
                            {
                                std::string rgb_no_aj_2 = rgb_no_aj;

                                for (unsigned short filas_y = 0; filas_y < alto_in; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_in; columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_no_aj[(filas_y * ancho_in * 3) + (columnas_x * 3) + cont_rgb] = rgb_no_aj_2[(ancho_in * alto_in * 3) - 3 - (columnas_x * 3) - (filas_y * ancho_in * 3) + cont_rgb];
                                        }
                                    }
                                }
                            }

                            if (jpg_reint[78] == 0x08)
                            {
                                std::string rgb_no_aj_2 = rgb_no_aj;

                                for (unsigned short filas_y = 0; filas_y < alto_in; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_in; columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_no_aj[(filas_y * ancho_in * 3) + (columnas_x * 3) + cont_rgb] = rgb_no_aj_2[(columnas_x * alto_in * 3) + (alto_in * 3) - 3 - (filas_y * 3) + cont_rgb];
                                        }
                                    }
                                }
                            }
                            
                            std::string rgb_entrada(ancho_aj * alto_aj * 3, '\0');

                            for (unsigned short filas_y = 0; filas_y < alto_aj; filas_y++)
                            {
                                for (unsigned short columnas_x = 0; columnas_x < ancho_aj * 3; columnas_x++)
                                {
                                    rgb_entrada[(filas_y * ancho_aj * 3) + columnas_x] = rgb_no_aj[(filas_y * ancho_in * 3) + columnas_x + (((alto_in - alto_aj) / 2) * ancho_in * 3) + (((ancho_in - ancho_aj) / 2) * 3)];
                                }
                            }

                            rgb_no_aj.clear();
                            
                            unsigned short ancho_fin;
                            unsigned short alto_fin;

                            if (ancho_aj > alto_aj)
                            {
                                ancho_fin = 1920, ancho_fin = 1080;
                            }
                            else
                            {
                                ancho_fin = 1080, ancho_fin = 1920;
                            }

                            std::vector<std::optional<float>> rgb_salida(ancho_fin * alto_fin * 3);

                            if (!(ancho_aj == 1920.0f && alto_aj == 1080.0f || ancho_aj == 1080.0f && alto_aj == 1920.0f))
                            {
                                unsigned short ancho_min;
                                unsigned short alto_min;
                                unsigned int iter1 = 0, iter2 = 0;
                                unsigned short norm_x = 0;
                                unsigned int norm_y = 0;

                                if (ancho_aj <= ancho_fin)
                                {
                                    ancho_min = ancho_aj;
                                }
                                else
                                {
                                    ancho_min = ancho_fin;
                                }

                                if (alto_aj <= alto_fin)
                                {
                                    alto_min = alto_aj;
                                }
                                else
                                {
                                    alto_min = alto_fin;
                                }

                                for (unsigned short filas_y = 0; filas_y < alto_min; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_min; columnas_x++)
                                    {
                                        if (ancho_fin >= ancho_aj)
                                        {
                                            norm_x = columnas_x * (float(ancho_fin) / (ancho_aj - 1));

                                            if (norm_x == ancho_fin)
                                            {
                                                norm_x = ancho_fin - 1;
                                            }
                                        }
                                        else
                                        {
                                            norm_x = columnas_x * (float(ancho_aj) / (ancho_fin - 1));

                                            if (norm_x == ancho_aj)
                                            {
                                                norm_x = ancho_aj - 1;
                                            }
                                        }

                                        norm_x *= 3;

                                        if (alto_fin >= alto_aj)
                                        {
                                            norm_y = filas_y * (float(alto_fin) / (alto_aj - 1));

                                            if (norm_y == alto_fin)
                                            {
                                                norm_y = alto_fin - 1;
                                            }

                                            norm_y *= ancho_fin * 3;
                                        }
                                        else
                                        {
                                            norm_y = filas_y * (float(alto_aj) / (alto_fin - 1));

                                            if (norm_y == alto_aj)
                                            {
                                                norm_y = alto_aj - 1;
                                            }

                                            norm_y *= ancho_aj * 3;
                                        }

                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            if (ancho_fin >= ancho_aj && alto_fin >= alto_aj)
                                            {
                                                rgb_salida[norm_y + norm_x + cont_rgb] = static_cast<unsigned char>(rgb_entrada[(filas_y * ancho_aj * 3) + (columnas_x * 3) + cont_rgb]);
                                            }

                                            if (ancho_fin <= ancho_aj && alto_fin < alto_aj || ancho_fin < ancho_aj && alto_fin <= alto_aj)
                                            {
                                                rgb_salida[(filas_y * ancho_fin * 3) + (columnas_x * 3) + cont_rgb] = static_cast<unsigned char>(rgb_entrada[norm_y + norm_x + cont_rgb]);
                                            }

                                            if (ancho_fin > ancho_aj && alto_fin < alto_aj)
                                            {
                                                rgb_salida[(filas_y * ancho_fin * 3) + norm_x + cont_rgb] = static_cast<unsigned char>(rgb_entrada[norm_y + (columnas_x * 3) + cont_rgb]);
                                            }

                                            if (ancho_fin < ancho_aj && alto_fin > alto_aj)
                                            {
                                                rgb_salida[norm_y + (columnas_x * 3) + cont_rgb] = static_cast<unsigned char>(rgb_entrada[(filas_y * ancho_aj * 3) + norm_x + cont_rgb]);
                                            }
                                        }
                                    }
                                }

                                for (unsigned short filas_y = 0; filas_y < alto_fin; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_fin; columnas_x++)
                                    {
                                        if (rgb_salida[filas_y * ancho_fin * 3].has_value())
                                        {
                                            if (columnas_x == 0)
                                            {
                                                iter1 = iter2;
                                                iter2 += 3;
                                            }
                                            else
                                            {
                                                if (rgb_salida[iter1].has_value() && rgb_salida[iter2].has_value())
                                                {
                                                    for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                                    {
                                                        for (unsigned short iter_esp = 0; iter_esp < (iter2 / 3) - (iter1 / 3) - 1; iter_esp++)
                                                        {
                                                            float frac = (1 + iter_esp) * (1.0f / ((iter2 / 3) - (iter1 / 3)));

                                                            rgb_salida[iter1 + 3 + (iter_esp * 3) + cont_rgb] = (rgb_salida[iter1 + cont_rgb].value() * (2 * std::pow(frac, 3) - 3 * std::pow(frac, 2) + 1)) + (rgb_salida[iter2 + cont_rgb].value() * (-2 * std::pow(frac, 3) + 3 * std::pow(frac, 2)));
                                                        }
                                                    }

                                                    iter1 = iter2;
                                                    iter2 += 3;
                                                }
                                                else
                                                {
                                                    iter2 += 3;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            iter1 += 3;
                                            iter2 += 3;
                                        }
                                    }
                                }

                                for (unsigned short columnas_x = 0; columnas_x < ancho_fin; columnas_x++)
                                {
                                    iter1 = columnas_x * 3, iter2 = columnas_x * 3;

                                    for (unsigned short filas_y = 0; filas_y < alto_fin; filas_y++)
                                    {
                                        if (rgb_salida[filas_y * ancho_fin * 3].has_value())
                                        {
                                            if (filas_y == 0)
                                            {
                                                iter2 += ancho_fin * 3;
                                            }
                                            else
                                            {
                                                if (rgb_salida[iter1].has_value() && rgb_salida[iter2].has_value())
                                                {
                                                    for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                                    {
                                                        for (unsigned short iter_esp = 0; iter_esp < (iter2 / ancho_fin / 3) - (iter1 / ancho_fin / 3) - 1; iter_esp++)
                                                        {
                                                            float frac = (1 + iter_esp) * (1.0f / ((iter2 / ancho_fin / 3) - (iter1 / ancho_fin / 3)));

                                                            rgb_salida[iter1 + (ancho_fin * 3) + (iter_esp * (ancho_fin * 3)) + cont_rgb] = (rgb_salida[iter1 + cont_rgb].value() * (2 * std::pow(frac, 3) - 3 * std::pow(frac, 2) + 1)) + (rgb_salida[iter2 + cont_rgb].value() * (-2 * std::pow(frac, 3) + 3 * std::pow(frac, 2)));
                                                        }
                                                    }

                                                    iter1 = iter2;
                                                    iter2 += ancho_fin * 3;
                                                }
                                                else
                                                {
                                                    iter2 += ancho_fin * 3;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            iter2 += ancho_fin * 3;
                                        }
                                    }
                                }

                                for (unsigned int iter = 0; iter < (ancho_fin * alto_fin * 3); iter++)
                                {
                                    rgb_salida[iter] = std::round(rgb_salida[iter].value());
                                }
                            }

                            rgb_entrada.clear();
                            
                            //CODIFICAR



                            //GUARDAR

                            char* ffd8ffd9 = new char[26 + db2 - db + c02 - c0 + c42 - c4 + da2 - da];
                            i = 0;

                            char d8e0[20] = { 0xFF,0xD8,0xFF,0xE0,0x00,0x10,0x4A,0x46,0x49,0x46,0x00,0x01,0x01,0x01,0x00,0xB4,0x00,0xB4,0x00,0x00 };
                            for (i; i < 20; i++)
                            {
                                ffd8ffd9[i] = d8e0[i];
                            }

                            ffd8ffd9[24 + db2 - db + c02 - c0 + c42 - c4 + da2 - da] = 0xFF;
                            ffd8ffd9[25 + db2 - db + c02 - c0 + c42 - c4 + da2 - da] = 0xD9;

                            unsigned int db3 = db;
                            for (db3; db3 <= db2; db3++, i++)
                            {
                                ffd8ffd9[i] = jpg_arr[db3];
                            }

                            i2 = i;

                            unsigned int c03 = c0;
                            for (c03; c03 <= c02; c03++, i++)
                            {
                                ffd8ffd9[i] = jpg_arr[c03];
                            }

                            char da00 = unsigned short(ancho_aj) / 256;
                            char da01 = unsigned short(ancho_aj) % 256;
                            char eb00 = unsigned short(alto_aj) / 256;
                            char eb01 = unsigned short(alto_aj) % 256;

                            ffd8ffd9[i2 + 7] = da00;
                            ffd8ffd9[i2 + 8] = da01;
                            ffd8ffd9[i2 + 5] = eb00;
                            ffd8ffd9[i2 + 6] = eb01;

                            unsigned int c43 = c4;
                            for (c43; c43 <= c42; c43++, i++)
                            {
                                ffd8ffd9[i] = jpg_arr[c43];
                            }

                            unsigned int da3 = da;
                            for (da3; da3 <= da2; da3++, i++)
                            {
                                ffd8ffd9[i] = jpg_arr[da3];
                            }

                            delete[] jpg_arr;
                            jpg_ent.close();

                            if (std::filesystem::exists("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.jpg"))
                            {
                                std::filesystem::remove("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.jpg");
                            }
                            std::ofstream jpg_sal("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.jpg", std::ios::binary);

                            if (std::filesystem::exists(dcim_dir.path().string()))
                            {
                                std::filesystem::remove(dcim_dir.path().string());
                            }

                            jpg_sal.write(ffd8ffd9, 26 + db2 - db + c02 - c0 + c42 - c4 + da2 - da);

                            delete[] ffd8ffd9;
                            jpg_sal.close();

                            std::filesystem::rename("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.jpg", "C:/Users/" + n_usr + "/Desktop/gallerydir/" + dcim_dir.path().stem().string() + ".jpg");

                            n++;
                            std::cout << dcim_dir.path().string() + "\n";
                        }
                    }
                }
            }

            if (std::filesystem::is_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/"))
            {
                std::filesystem::remove_all("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/");
            }

            if (n == 0)
            {
                std::cout << "No jpg files with dimensions greater than 1920×1080 were found\n";
            }
        }
        else
        {
            if (std::filesystem::is_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/"))
            {
                std::filesystem::remove_all("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/");
            }

            std::cout << "No jpg files found\n";
        }

        if (n == 1)
        {
            std::cout << "One modified file\n";
        }
        if (n > 1)
        {
            std::cout << n << " modified files\n";
        }
    }
    else
    {
        std::cout << "Directory doesn't exist\n";
    }

    system("pause");
    return 0;
}
