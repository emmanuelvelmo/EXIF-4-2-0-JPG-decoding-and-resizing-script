#include<iostream>
#include<filesystem>
#include<fstream>
#include<string>
#include<vector>
#include<functional>
#include<optional>
#include <bitset>
#include<cmath>
#include<cstdlib>
#include<Windows.h>

int main()
{
    //CAPTURA NOMBRE DE USUARIO
    wchar_t w_usr[256];
    DWORD tam_w_usr = 256;

    GetUserName(w_usr, &tam_w_usr);
    std::string n_usr(w_usr, w_usr + tam_w_usr);
    n_usr.pop_back();

    //CREA DIRECTORIO PARA PROCESAR IMÁGENES
    if (!std::filesystem::is_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/"))
    {
        std::filesystem::create_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/");
    }

    std::cout << "Please store all JPG files in the 'gallerydir' folder on the Desktop and press any key to continue\n";
    system("pause");

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

        //SI HAY POR LO MENOS UNA IMAGEN JPG
        if (n == 1)
        {
            if (!std::filesystem::is_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/"))
            {
                std::filesystem::create_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/");
            }

            n = 0;

            //ITERA CADA UNA DE LAS IMÁGENES EN EL DIRECTORIO CREADO
            for (std::filesystem::directory_entry dcim_dir : std::filesystem::directory_iterator("C:/Users/" + n_usr + "/Desktop/gallerydir/"))
            {
                if (dcim_dir.path().extension() == ".jpg")
                {
                    unsigned int i;
                    unsigned short m = 0;

                    //CAPTURA IMAGEN EN MEMORIA
                    std::ifstream jpg_ent(dcim_dir.path().string(), std::ios::binary);

                    //SE OBTIENE TAMAÑO DE IMAGEN
                    jpg_ent.seekg(0, jpg_ent.end);
                    unsigned int jpg_size = jpg_ent.tellg();
                    jpg_ent.seekg(0, jpg_ent.beg);

                    //PUNTERO HACIA POSICIONES DEL ARREGLO DE LA IMAGEN
                    char* jpg_arr = new char[jpg_size];
                    jpg_ent.read(jpg_arr, jpg_size);

                    jpg_ent.close();

                    //REINTERPRETA PUNTERO PARA PODER LEER BYTES
                    unsigned char* jpg_reint = reinterpret_cast<unsigned char*>(jpg_arr);

                    //SI LA IMAGEN POSEE THUMBNAIL SALTA AL SEGUNDO MARCADOR
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
                        //BYTE QUE INDICA ORIENTACIÓN EXIF
                        unsigned char byte_ornt = jpg_reint[78];

                        float alto_in;
                        float ancho_in;

                        //CAPTURA DIMENSIONES
                        alto_in = jpg_reint[i] * 256 + jpg_reint[i + 1];
                        ancho_in = jpg_reint[i + 2] * 256 + jpg_reint[i + 3];

                        //SI LAS DIMENSIONES SON DIFERENTES A LAS DE SALIDA SE PROCESA LA IMAGEN
                        if (!(ancho_in == 1920 && alto_in == 1080 || ancho_in == 1080 && alto_in == 1920))
                        {
                            //CALCULAR NUEVAS DIMENSIONES PARA RECORTE EN RATIO 16:9 O 9:16
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
                                    ancho_aj = alto_in * (16.0f / 9);
                                    alto_aj = alto_in;
                                }
                                else
                                {
                                    ancho_aj = ancho_in;
                                    alto_aj = ancho_in * (9.0f / 16);
                                }
                            }
                            else
                            {
                                if (rel_lado > ancho_in)
                                {
                                    ancho_aj = ancho_in;
                                    alto_aj = ancho_in * (16.0f / 9);
                                }
                                else
                                {
                                    ancho_aj = alto_in * (9.0f / 16);
                                    alto_aj = alto_in;
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

                            //OBTENER POSICIONES DE MARCADOR FFC4 (INICIO Y FIN)
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

                            //TABLAS HUFFVAL DE LUMINANCIA Y CROMINANCIA PARA COEFICIENTES DC Y AC  (INICIO Y FIN)
                            unsigned char dht_00[16];
                            std::string lum_dc_nod;

                            unsigned char dht_01[16];
                            std::string chr_dc_nod;

                            unsigned char dht_10[16];
                            std::string lum_ac_nod;

                            unsigned char dht_11[16];
                            std::string chr_ac_nod;

                            //GUARDAR SÍMBOLOS EN ARREGLOS
                            unsigned int i2 = 0;
                            unsigned short m2;
                            unsigned char ch_ch;

                            for (m = 0; m < 16; m++)
                            {
                                i2 = i2 + jpg_reint[c4 + 5 + m];
                            }

                            if (18 + i2 < c42 - c4)
                            {
                                unsigned short m3 = 0, m4 = 0;

                                //TABLA DC DE LUMINANCIA
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

                                //TABLA DC DE CROMINANCIA
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

                                //TABLA AC DE LUMINANCIA
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

                                //TABLA AC DE CROMINANCIA
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

                            //OBTENER POSICIONES DE MARCADOR FFDA (INICIO Y FIN)
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

                            //FUNCIÓN PARA GENERAR CÓDIGOS CANÓNICOS
                            std::function<std::string* (unsigned char(&)[16], unsigned short&)> generar_canonicos = [&i](unsigned char(&dht2)[16], unsigned short& tam_arr2) -> std::string*
                            {
                                unsigned short m = 0;

                                for (i = 0; i < 16; i++)
                                {
                                    tam_arr2 += dht2[i];
                                }

                                std::string* inst_codigos = new std::string[tam_arr2];
                                std::string gen_codigo = "0";

                                m = 0;
                                for (i = 0; i < 16; i++)
                                {
                                    for (unsigned short m2 = 0; m2 < dht2[i]; m2++)
                                    {
                                        inst_codigos[m++] = gen_codigo;

                                        unsigned char ite_str = gen_codigo.size() - 1;
                                        while (ite_str >= 0 && gen_codigo[ite_str] == '1')
                                        {
                                            gen_codigo[ite_str] = '0';
                                            --ite_str;
                                        }
                                        if (ite_str >= 0)
                                        {
                                            gen_codigo[ite_str] = '1';
                                        }
                                    }

                                    gen_codigo += "0";
                                }

                                return inst_codigos;
                            };

                            //ARREGLOS PARA ALMACENAR CÓDIGOS CANÓNICOS DE TABLAS DE LUMINANCIA Y CROMINANCIA Y VARIABLES PARA LA CANTIDAD DE ELEMENTOS DE CADA TABLA
                            unsigned short tam_arr_00 = 0;
                            std::string* codigos_canonicos_00 = generar_canonicos(dht_00, tam_arr_00);

                            unsigned short tam_arr_01 = 0;
                            std::string* codigos_canonicos_01 = generar_canonicos(dht_01, tam_arr_01);

                            unsigned short tam_arr_10 = 0;
                            std::string* codigos_canonicos_10 = generar_canonicos(dht_10, tam_arr_10);

                            unsigned short tam_arr_11 = 0;
                            std::string* codigos_canonicos_11 = generar_canonicos(dht_11, tam_arr_11);

                            std::vector<float> ffda_buff;

                            //FUNCIÓN PARA DECODIFICAR SEGMENTO FFDA-FFD9 / DECODIFICACIÓN HUFFMAN
                            std::function<void()> decod_ffda = [&i, &da, &da2, &jpg_reint, &ffda_buff, &codigos_canonicos_00, &lum_dc_nod, &tam_arr_00, &codigos_canonicos_01, &chr_dc_nod, &tam_arr_01, &codigos_canonicos_10, &lum_ac_nod, &tam_arr_10, &codigos_canonicos_11, &chr_ac_nod, &tam_arr_11]()
                            {
                                //CONTADORES PARA RUN-LENGTH Y COMPLEMENTO A DOS
                                unsigned short num_ceros = 0;
                                unsigned short nbits_cdos = 0;

                                //FUNCIÓN DE INVERSA DE TRANSFORMADA DISCRETA DE COSENO
                                std::function<void()> f_idct = [&ffda_buff]()
                                {
                                    float mtz_tmp[64];

                                    for (unsigned short iter_idct = 0; iter_idct < 64; iter_idct++)
                                    {
                                        mtz_tmp[iter_idct] = ffda_buff[ffda_buff.size() - 64 + iter_idct];
                                    }

                                    for (unsigned short cord_y = 0; cord_y < 8; cord_y++)
                                    {
                                        for (unsigned short cord_x = 0; cord_x < 8; ++cord_x)
                                        {
                                            float p_sum = 0;

                                            for (unsigned short cord_v = 0; cord_v < 8; cord_v++)
                                            {
                                                for (unsigned short cord_u = 0; cord_u < 8; cord_u++)
                                                {
                                                    float cu = (cord_v == 0) ? 1 / std::sqrt(2) : 1;
                                                    float cv = (cord_u == 0) ? 1 / std::sqrt(2) : 1;

                                                    p_sum += cu * cv * mtz_tmp[(cord_v * 8) + cord_u] * std::cos((2 * cord_y + 1) * cord_v * 3.141592 / 16) * std::cos((2 * cord_x + 1) * cord_u * 3.141592 / 16);
                                                }
                                            }

                                            ffda_buff[(ffda_buff.size() - 64) + ((cord_y * 8) + cord_x)] = p_sum / 4;
                                        }
                                    }
                                };

                                //FUNCIÓN DE ZIG ZAG INVERSO
                                std::function<void()> f_zig_zag = [&ffda_buff]()
                                {
                                    unsigned short i_z_z[64] = { 0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42, 3, 8, 12, 17, 25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63 };
                                    short tb_dc_ac[64];

                                    for (unsigned short iter_tb = 0; iter_tb < 64; iter_tb++)
                                    {
                                        tb_dc_ac[iter_tb] = ffda_buff[ffda_buff.size() - 64 + i_z_z[iter_tb]];
                                    }

                                    for (unsigned short iter_tb = 0; iter_tb < 64; iter_tb++)
                                    {
                                        ffda_buff[ffda_buff.size() - 64 + iter_tb] = tb_dc_ac[iter_tb];
                                    }
                                };

                                //FUNCIÓN DE COMPLEMENTO A DOS
                                std::function<short(std::string)> complemento_dos = [](std::string buff_canonico2) -> short
                                {
                                    bool eval_neg = false;

                                    if (buff_canonico2[0] == '0')
                                    {
                                        eval_neg = true;

                                        for (unsigned short iter_buff = 0; iter_buff < buff_canonico2.size(); iter_buff++)
                                        {
                                            if (buff_canonico2[iter_buff] == '0')
                                            {
                                                buff_canonico2[iter_buff] = '1';
                                            }
                                            else
                                            {
                                                buff_canonico2[iter_buff] = '0';
                                            }
                                        }
                                    }

                                    std::bitset<16> conv_decimal(buff_canonico2);

                                    if (eval_neg == true)
                                    {
                                        return conv_decimal.to_ulong() * -1;
                                    }
                                    else
                                    {
                                        return conv_decimal.to_ulong();
                                    }
                                };

                                //FUNCIÓN PARA SEPARAR DÍGITOS DE BYTE (SÍMBOLO) Y CONVERTIR A DECIMAL PARA RUN-LENGTH Y COMPLEMENTO A DOS
                                std::function<void(unsigned char)> sep_digs = [&num_ceros, &nbits_cdos](unsigned char byte_char)
                                {
                                    bool bit_eval;
                                    std::string buff_bit_4;

                                    for (short iter_bit = 7; iter_bit >= 4; iter_bit--)
                                    {
                                        bit_eval = byte_char & (1 << iter_bit);
                                        buff_bit_4.push_back(bit_eval ? '1' : '0');

                                        if (iter_bit == 4)
                                        {
                                            std::bitset<4> conv_bit_4(buff_bit_4);
                                            std::string().swap(buff_bit_4);
                                            num_ceros = conv_bit_4.to_ulong();
                                        }
                                    }

                                    for (short iter_bit = 3; iter_bit >= 0; iter_bit--)
                                    {
                                        bit_eval = byte_char & (1 << iter_bit);
                                        buff_bit_4.push_back(bit_eval ? '1' : '0');

                                        if (iter_bit == 0)
                                        {
                                            std::bitset<4> conv_bit_4(buff_bit_4);
                                            std::string().swap(buff_bit_4);
                                            nbits_cdos = conv_bit_4.to_ulong();
                                        }
                                    }

                                    //CONSIDERA SÍMBOLO F0 COMO 16 Y NO 15 CEROS
                                    if (num_ceros == 15 && nbits_cdos == 0)
                                    {
                                        num_ceros = 16;
                                    }
                                };

                                //POSICIÓN INICIAL DE BITSTREAM. CONTADORES PARA BITS, MCUS, COEFICIENTES AC, COMPLEMETO A DOS. VARIABLES PARA DECODIFICACIÓN DELTA. BUFFER PARA CÓDIGOS CANÓNICOS
                                unsigned int da_2 = da;

                                unsigned short cont_bit = 7;
                                unsigned short cont_dcac = 0;
                                unsigned short cont_nbits = 0;
                                unsigned short cont_63 = 0;

                                short delta_y = 0;
                                short delta_cb = 0;
                                short delta_cr = 0;

                                bool bit_val = false;
                                bool ev_comp_dos = false;

                                std::string buff_canonico;

                                //ITERA SEGMENTO FFDA-FFD9
                                for (i = 0; i < 8 * (da2 - da - 13); i++)
                                {
                                    //SI SE APLICA COMPLEMENTO A DOS PARA UNA CIFRA
                                    if (nbits_cdos > 0)
                                    {
                                        //SE ALMACENA EN BUFFER EL CÓDIGO CORRESPONDIENTE AL COMPLEMENTO A DOS EN LARGO DE BITS
                                        bool bit_val = jpg_reint[da_2 + 14] & (1 << cont_bit);
                                        buff_canonico.push_back(bit_val ? '1' : '0');
                                        cont_nbits++;

                                        //SI SE ALCANZA EL LARGO EN BITS DEL COMPLEMENTO A DOS
                                        if (cont_nbits == nbits_cdos)
                                        {
                                            short val_decimal = complemento_dos(buff_canonico);

                                            //SE GUARDA EL VALOR DECIMAL COMO COEFICIENTE DC / SE APLICA DECODIFICACIÓN DELTA PARA CADA CAPA
                                            if (cont_dcac == 0 || cont_dcac == 64 || cont_dcac == 128 || cont_dcac == 192)
                                            {
                                                delta_y += val_decimal;
                                                ffda_buff.push_back(delta_y);
                                            }

                                            if (cont_dcac == 256)
                                            {
                                                delta_cb += val_decimal;
                                                ffda_buff.push_back(delta_cb);
                                            }

                                            if (cont_dcac == 320)
                                            {
                                                delta_cr += val_decimal;
                                                ffda_buff.push_back(delta_cr);
                                            }

                                            //SE GUARDA EL VALOR DECIMAL COMO COEFICIENTE AC
                                            if (cont_dcac > 0 && cont_dcac < 64 || cont_dcac > 64 && cont_dcac < 128 || cont_dcac > 128 && cont_dcac < 192 || cont_dcac > 192 && cont_dcac < 256 || cont_dcac > 256 && cont_dcac < 320 || cont_dcac > 320 && cont_dcac < 384)
                                            {
                                                ffda_buff.push_back(val_decimal);

                                                cont_63++;

                                                //SE RETORNA CONTADOR DE CEROS A VALOR INICIAL
                                                if (cont_63 == 63)
                                                {
                                                    cont_63 = 0;

                                                    //SE REALIZA ZIG ZAG INVERSO, DECUANTIFICACIÓN E INVERSA DE TRANSFORMADA DISCRETA DE COSENO SOBRE LA MATRIZ DE 8X8 PROCESADA
                                                    f_zig_zag();
                                                    f_idct();
                                                }
                                            }

                                            std::string().swap(buff_canonico);
                                            cont_nbits = 0;
                                            nbits_cdos = 0;

                                            cont_dcac++;
                                        }

                                        //RETORNA CONTADOR DE BITS A VALOR INICIAL
                                        if (cont_bit == 0)
                                        {
                                            cont_bit = 7;
                                            da_2++;
                                        }
                                        else
                                        {
                                            cont_bit--;
                                        }

                                        //CONSIDERA FF0000 COMO SALTO DE TERCER BYTE (BYTE 00)
                                        if (cont_bit == 7 && jpg_reint[da_2 + 13] == 0xFF && jpg_reint[da_2 + 14] == 0x00)
                                        {
                                            da_2++;
                                            i += 8;
                                        }

                                        if (cont_dcac == 384)
                                        {
                                            cont_dcac = 0;
                                        }
                                    }
                                    //SI NO SE APLICA COMPLEMENTO A DOS SE ALMACENAN BITS EN BUFFER, SE AGREGAN CEROS A MATRICES
                                    else
                                    {
                                        //SE ALMACENA EN UN BUFFER EL CÓDIGO CANÓNICO DECODIFICADO HASTA ENCONTRAR COINCIDENCIA EN LA TABLA QUE CORRESPONDA
                                        bool bit_val = jpg_reint[da_2 + 14] & (1 << cont_bit);
                                        buff_canonico.push_back(bit_val ? '1' : '0');

                                        bool comprobar_buff = false;

                                        if (buff_canonico.size() > 1)
                                        {
                                            //SI EL CONTADOR CAE EN UN COEFICIENTE DC SE USA EL SÍMBOLO ASOCIADO DE LA TABLA DC DE LUMINANCIA
                                            if (cont_dcac == 0 || cont_dcac == 64 || cont_dcac == 128 || cont_dcac == 192)
                                            {
                                                //SE RECORREN LAS TABLAS CORRESPONDIENTES
                                                for (unsigned int iter_dcac = 0; iter_dcac < tam_arr_00; iter_dcac++)
                                                {
                                                    //SI SE ENCUENTRA LA TABLA CORRESPONDIENTE
                                                    if (buff_canonico == codigos_canonicos_00[iter_dcac])
                                                    {
                                                        //SE CONVIERTE EL SÍMBOLO A VALOR DECIMAL PARA EL NÚMERO DE BITS PARA COMPLEMENTO A DOS
                                                        nbits_cdos = static_cast<unsigned short>(lum_dc_nod[iter_dcac]);

                                                        //EL BUFFER HA SIDO UTILIZADO
                                                        comprobar_buff = true;

                                                        //SI EL SÍMBOLO ES 00 EL COEFICIENTE DC CONSERVARÁ EL MISMO VALOR QUE EL COEFICIENTE DC DE LA MATRIZ ANTERIOR
                                                        if (nbits_cdos == 0)
                                                        {
                                                            ev_comp_dos = true;
                                                        }
                                                    }
                                                }

                                                //SI SE HA UTILIZADO EL BUFFER SE LIMPIA PARA SU REUTILIZACIÓN
                                                if (comprobar_buff == true)
                                                {
                                                    std::string().swap(buff_canonico);
                                                }
                                            }

                                            //SI EL CONTADOR CAE EN UN COEFICIENTE AC SE USA EL SÍMBOLO ASOCIADO DE LA TABLA AC DE LUMINANCIA
                                            if (cont_dcac > 0 && cont_dcac < 64 || cont_dcac > 64 && cont_dcac < 128 || cont_dcac > 128 && cont_dcac < 192 || cont_dcac > 192 && cont_dcac < 256)
                                            {
                                                for (unsigned int iter_dcac = 0; iter_dcac < tam_arr_10; iter_dcac++)
                                                {
                                                    if (buff_canonico == codigos_canonicos_10[iter_dcac])
                                                    {
                                                        //SE SEPARA EL DÍGITO EN DOS NÚMEROS Y SE ASIGNAN SUS VALORES A LOS CONTADORES DE RUN-LENGTH Y COMPLEMENTO A DOS
                                                        sep_digs(lum_ac_nod[iter_dcac]);

                                                        //SI EL SÍMBOLO ES 00 SE LLENA EL RESTO DE LA MATRIZ (END OF BLOCK))
                                                        if (num_ceros == 0 && nbits_cdos == 0)
                                                        {
                                                            for (cont_63; cont_63 < 63; cont_63++)
                                                            {
                                                                ffda_buff.push_back(0);

                                                                cont_dcac++;
                                                            }
                                                        }

                                                        //SE RELLENA CON LA CANTIDAD DE CEROS ESPECIFICADA
                                                        if (num_ceros > 0)
                                                        {
                                                            for (unsigned short bit_cont = 0; bit_cont < num_ceros; bit_cont++)
                                                            {
                                                                ffda_buff.push_back(0);

                                                                cont_dcac++;
                                                                cont_63++;
                                                            }

                                                            num_ceros = 0;
                                                        }

                                                        comprobar_buff = true;

                                                        if (cont_63 == 63)
                                                        {
                                                            cont_63 = 0;

                                                            f_zig_zag();
                                                            f_idct();
                                                        }
                                                    }
                                                }

                                                if (comprobar_buff == true)
                                                {
                                                    std::string().swap(buff_canonico);
                                                }
                                            }

                                            //SI EL CONTADOR CAE EN UN COEFICIENTE DC SE USA EL SÍMBOLO ASOCIADO DE LA TABLA DC DE CROMINANCIA
                                            if (cont_dcac == 256 || cont_dcac == 320)
                                            {
                                                for (unsigned int iter_dcac = 0; iter_dcac < tam_arr_01; iter_dcac++)
                                                {
                                                    if (buff_canonico == codigos_canonicos_01[iter_dcac])
                                                    {
                                                        nbits_cdos = static_cast<unsigned short>(lum_dc_nod[iter_dcac]);

                                                        comprobar_buff = true;

                                                        if (nbits_cdos == 0)
                                                        {
                                                            ev_comp_dos = true;
                                                        }
                                                    }
                                                }

                                                if (comprobar_buff == true)
                                                {
                                                    std::string().swap(buff_canonico);
                                                }
                                            }

                                            //SI EL CONTADOR CAE EN UN COEFICIENTE AC SE USA EL SÍMBOLO ASOCIADO DE LA TABLA AC DE CROMINANCIA
                                            if (cont_dcac > 256 && cont_dcac < 320 || cont_dcac > 320 && cont_dcac < 384)
                                            {
                                                for (unsigned int iter_dcac = 0; iter_dcac < tam_arr_11; iter_dcac++)
                                                {
                                                    if (buff_canonico == codigos_canonicos_11[iter_dcac])
                                                    {
                                                        sep_digs(chr_ac_nod[iter_dcac]);

                                                        if (num_ceros == 0 && nbits_cdos == 0)
                                                        {
                                                            for (cont_63; cont_63 < 63; cont_63++)
                                                            {
                                                                ffda_buff.push_back(0);

                                                                cont_dcac++;
                                                            }
                                                        }

                                                        if (num_ceros > 0)
                                                        {
                                                            for (unsigned short bit_cont = 0; bit_cont < num_ceros; bit_cont++)
                                                            {
                                                                ffda_buff.push_back(0);

                                                                cont_dcac++;
                                                                cont_63++;
                                                            }

                                                            num_ceros = 0;
                                                        }

                                                        comprobar_buff = true;

                                                        if (cont_63 == 63)
                                                        {
                                                            cont_63 = 0;

                                                            f_zig_zag();
                                                            f_idct();
                                                        }
                                                    }
                                                }

                                                if (comprobar_buff == true)
                                                {
                                                    std::string().swap(buff_canonico);
                                                }
                                            }
                                        }

                                        //SE CONSERVA EL MISMO VALOR DEL COEFICIENTE DC ANTERIOR SEGÚN SU CAPA
                                        if (ev_comp_dos == true)
                                        {
                                            if (cont_dcac == 0 || cont_dcac == 64 || cont_dcac == 128 || cont_dcac == 192)
                                            {
                                                ffda_buff.push_back(delta_y);
                                            }

                                            if (cont_dcac == 256)
                                            {
                                                ffda_buff.push_back(delta_cb);
                                            }

                                            if (cont_dcac == 320)
                                            {
                                                ffda_buff.push_back(delta_cr);
                                            }

                                            ev_comp_dos = false;

                                            cont_dcac++;
                                        }

                                        if (cont_bit == 0)
                                        {
                                            cont_bit = 7;
                                            da_2++;
                                        }
                                        else
                                        {
                                            cont_bit--;
                                        }

                                        if (cont_bit == 7 && jpg_reint[da_2 + 13] == 0xFF && jpg_reint[da_2 + 14] == 0x00)
                                        {
                                            da_2++;
                                            i += 8;
                                        }

                                        if (cont_dcac == 384)
                                        {
                                            cont_dcac = 0;
                                        }
                                    }
                                }
                            };

                            decod_ffda();

                            delete[] codigos_canonicos_00;
                            delete[] codigos_canonicos_01;
                            delete[] codigos_canonicos_10;
                            delete[] codigos_canonicos_11;
                            delete[] jpg_arr;

                            unsigned short ancho_8;
                            unsigned short alto_8;

                            //OBTENER DIMENSIONES DE IMAGEN DECODIFICADA
                            if ((ancho_in / 8) - short(ancho_in / 8) > 0)
                            {
                                ancho_8 = (short(ancho_in / 8) + 1) * 8;
                            }
                            else
                            {
                                ancho_8 = ancho_in;
                            }

                            if ((alto_in / 8) - short(alto_in / 8) > 0)
                            {
                                alto_8 = (short(alto_in / 8) + 1) * 8;
                            }
                            else
                            {
                                alto_8 = alto_in;
                            }

                            //SUBSAMPLING POR CAPAS
                            std::function<void(std::vector<float>&, unsigned short&, unsigned short&, unsigned short&, unsigned short&, bool&)> f_ycbcr = [&ffda_buff, &ancho_8, &alto_8](std::vector<float>& rgb_entrada2, unsigned short& n_coe2, unsigned short& pos_in2, unsigned short& cu_coe2, unsigned short& mcu_coe2, bool& bl_ssp2)
                            {
                                //ALMACENA CAPA EN ARREGLO
                                for (unsigned int iter0 = 0; iter0 < ffda_buff.size() / 384; iter0++)
                                {
                                    for (unsigned short iter1 = 0; iter1 < n_coe2; iter1++)
                                    {
                                        rgb_entrada2[(iter0 * n_coe2) + iter1] = ffda_buff[(iter0 * 384) + pos_in2 + iter1];
                                    }
                                }

                                //REORDENAR ARREGLO EN MATRIZ
                                if (true)
                                {
                                    std::vector<float> rgb_tmp = rgb_entrada2;
                                    unsigned int iter_lin = 0;

                                    for (unsigned short cuad_y = 0; cuad_y < alto_8 / 16; cuad_y++)
                                    {
                                        for (unsigned short cuad_x = 0; cuad_x < ancho_8 / 16; cuad_x++)
                                        {
                                            for (unsigned short mcu_y = 0; mcu_y < mcu_coe2; mcu_y++)
                                            {
                                                for (unsigned short mcu_x = 0; mcu_x < mcu_coe2; mcu_x++)
                                                {
                                                    for (unsigned short filas_y = 0; filas_y < 8; filas_y++)
                                                    {
                                                        for (unsigned short columnas_x = 0; columnas_x < 8; columnas_x++)
                                                        {
                                                            rgb_entrada2[(cuad_y * (ancho_8 / cu_coe2) * (16 / cu_coe2)) + (cuad_x * (16 / cu_coe2)) + ((2 - cu_coe2) * mcu_y * ancho_8 * 8) + ((2 - cu_coe2) * mcu_x * 8) + (filas_y * (ancho_8 / cu_coe2)) + columnas_x] = rgb_tmp[iter_lin];

                                                            iter_lin++;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    std::vector<float>().swap(rgb_tmp);
                                }

                                //SUBSAMPLING SI APLICA
                                if (bl_ssp2 == true)
                                {
                                    std::vector<float> rgb_tmp((ffda_buff.size() / 6) * 4);

                                    for (unsigned short filas_y = 0; filas_y < alto_8; filas_y++)
                                    {
                                        for (unsigned short columnas_x = 0; columnas_x < ancho_8; columnas_x++)
                                        {
                                            rgb_tmp[(filas_y * ancho_8) + columnas_x] = rgb_entrada2[((filas_y / 2) * (ancho_8 / 2)) + (columnas_x / 2)];
                                        }
                                    }

                                    rgb_entrada2 = rgb_tmp;
                                    std::vector<float>().swap(rgb_tmp);
                                }
                            };

                            //CAPA Y
                            std::vector<float> rgb_entrada_y((ffda_buff.size() * 2) / 3);

                            unsigned short n_coe = 256;
                            unsigned short pos_in = 0;
                            unsigned short cu_coe = 1;
                            unsigned short mcu_coe = 2;
                            bool bl_ssp = false;

                            f_ycbcr(rgb_entrada_y, n_coe, pos_in, cu_coe, mcu_coe, bl_ssp);

                            //CAPA CB
                            std::vector<float> rgb_entrada_cb(ffda_buff.size() / 6);

                            n_coe = 64;
                            pos_in = 256;
                            cu_coe = 2;
                            mcu_coe = 1;
                            bl_ssp = true;

                            f_ycbcr(rgb_entrada_cb, n_coe, pos_in, cu_coe, mcu_coe, bl_ssp);

                            //CAPA CR
                            std::vector<float> rgb_entrada_cr(ffda_buff.size() / 6);

                            n_coe = 64;
                            pos_in = 320;
                            cu_coe = 2;
                            mcu_coe = 1;
                            bl_ssp = true;

                            f_ycbcr(rgb_entrada_cr, n_coe, pos_in, cu_coe, mcu_coe, bl_ssp);

                            std::vector<float> rgb_entrada(ffda_buff.size() * 2);

                            //REORDENA YCBCR
                            if (true)
                            {
                                for (unsigned int iter0 = 0; iter0 < rgb_entrada.size() / 3; iter0++)
                                {
                                    rgb_entrada[iter0 * 3] = rgb_entrada_y[iter0];
                                    rgb_entrada[(iter0 * 3) + 1] = rgb_entrada_cb[iter0];
                                    rgb_entrada[(iter0 * 3) + 2] = rgb_entrada_cr[iter0];
                                }
                            }

                            std::vector<float>().swap(ffda_buff);
                            std::vector<float>().swap(rgb_entrada_y);
                            std::vector<float>().swap(rgb_entrada_cb);
                            std::vector<float>().swap(rgb_entrada_cr);

                            //REORDENA EN ESPEJO
                            if (byte_ornt == 0x01 || byte_ornt == 0x03)
                            {
                                std::vector<float> rgb_tmp = rgb_entrada;

                                for (unsigned short filas_y = 0; filas_y < alto_8; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_8; columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_entrada[(filas_y * ancho_8 * 3) + (columnas_x * 3) + cont_rgb] = rgb_tmp[(ancho_8 * 3) - 3 + (filas_y * ancho_8 * 3) - (columnas_x * 3) + cont_rgb];
                                        }
                                    }
                                }

                                std::vector<float>().swap(rgb_tmp);
                            }
                            else
                            {
                                std::vector<float> rgb_tmp = rgb_entrada;

                                for (unsigned short filas_y = 0; filas_y < alto_8; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_8; columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_entrada[(filas_y * ancho_8 * 3) + (columnas_x * 3) + cont_rgb] = rgb_tmp[(ancho_8 * alto_8 * 3) - (ancho_8 * 3) - (filas_y * ancho_8 * 3) + (columnas_x * 3) + cont_rgb];
                                        }
                                    }
                                }

                                std::vector<float>().swap(rgb_tmp);
                            }

                            //ROTAR MATRIZ 180 GRADOS
                            if (byte_ornt == 0x01)
                            {
                                std::vector<float> rgb_tmp = rgb_entrada;

                                for (unsigned short filas_y = 0; filas_y < alto_8; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_8; columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_entrada[(filas_y * ancho_8 * 3) + (columnas_x * 3) + cont_rgb] = rgb_tmp[(ancho_8 * alto_8 * 3) - 3 - (columnas_x * 3) - (filas_y * ancho_8 * 3) + cont_rgb];
                                        }
                                    }
                                }

                                std::vector<float>().swap(rgb_tmp);
                            }

                            //ROTAR MATRIZ 90 GRADOS IZQUIERDA
                            if (byte_ornt == 0x06)
                            {
                                std::vector<float> rgb_tmp = rgb_entrada;
                                unsigned int iter_lin = 0;

                                for (unsigned short columnas_x = 0; columnas_x < ancho_8; columnas_x++)
                                {
                                    for (unsigned short filas_y = 0; filas_y < alto_8; filas_y++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_entrada[iter_lin] = rgb_tmp[(ancho_8 * 3) - 3 + (filas_y * ancho_8 * 3) - (columnas_x * 3) + cont_rgb];

                                            iter_lin++;
                                        }
                                    }
                                }

                                std::vector<float>().swap(rgb_tmp);
                            }

                            //ROTAR MATRIZ 90 GRADOS DERECHA
                            if (byte_ornt == 0x08)
                            {
                                std::vector<float> rgb_tmp = rgb_entrada;
                                unsigned int iter_lin = 0;

                                for (unsigned short columnas_x = 0; columnas_x < ancho_8; columnas_x++)
                                {
                                    for (unsigned short filas_y = 0; filas_y < alto_8; filas_y++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_entrada[iter_lin] = rgb_tmp[(ancho_8 * alto_8 * 3) - (ancho_8 * 3) - (filas_y * ancho_8 * 3) + (columnas_x * 3) + cont_rgb];

                                            iter_lin++;
                                        }
                                    }
                                }

                                std::vector<float>().swap(rgb_tmp);
                            }

                            //CONVERSIÓN RGB
                            for (unsigned int iter_conv = 0; iter_conv < rgb_entrada.size() / 3; iter_conv++)
                            {
                                float conv_R = rgb_entrada[iter_conv * 3] + 1.402f * rgb_entrada[(iter_conv * 3) + 2] + 128;
                                float conv_G = rgb_entrada[iter_conv * 3] - 0.344f * rgb_entrada[(iter_conv * 3) + 1] - 0.714f * rgb_entrada[(iter_conv * 3) + 2] + 128;
                                float conv_B = rgb_entrada[iter_conv * 3] + 1.772f * rgb_entrada[(iter_conv * 3) + 1] + 128;

                                if (conv_R < 0) conv_R = 0;
                                if (conv_R > 255) conv_R = 255;
                                if (conv_G < 0) conv_G = 0;
                                if (conv_G > 255) conv_G = 255;
                                if (conv_B < 0) conv_B = 0;
                                if (conv_B > 255) conv_B = 255;

                                rgb_entrada[iter_conv * 3] = conv_B;
                                rgb_entrada[(iter_conv * 3) + 1] = conv_G;
                                rgb_entrada[(iter_conv * 3) + 2] = conv_R;
                            }

                            //REORDENAR DIMENSIONES
                            if (byte_ornt == 0x06 || byte_ornt == 0x08)
                            {
                                unsigned int ancho_x = alto_in;
                                unsigned int alto_x = ancho_in;

                                ancho_in = ancho_x;
                                alto_in = alto_x;

                                unsigned int ancho_x1 = alto_aj;
                                unsigned int alto_x1 = ancho_aj;

                                ancho_aj = ancho_x1;
                                alto_aj = alto_x1;

                                unsigned int ancho_x2 = alto_8;
                                unsigned int alto_x2 = ancho_8;

                                ancho_8 = ancho_x2;
                                alto_8 = alto_x2;
                            }

                            //RECORTE DE BUFFER
                            if (!(ancho_in == ancho_8 && alto_in == alto_8))
                            {
                                std::vector<float> rgb_tmp(ancho_in * alto_in * 3);

                                for (unsigned short filas_y = 0; filas_y < int(alto_in); filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < int(ancho_in); columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_tmp[(filas_y * int(ancho_in) * 3) + (columnas_x * 3) + cont_rgb] = rgb_entrada[(ancho_8 * (alto_8 - int(alto_in)) * 3) + ((ancho_8 - int(ancho_in)) * 3) + (filas_y * ancho_8 * 3) + (columnas_x * 3) + cont_rgb];
                                        }
                                    }
                                }

                                rgb_entrada = rgb_tmp;
                                std::vector<float>().swap(rgb_tmp);
                            }

                            //RECORTE CENTRADO
                            if (true)
                            {
                                std::vector<float> rgb_tmp(ancho_aj * alto_aj * 3);

                                for (unsigned short filas_y = 0; filas_y < int(alto_aj); filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < int(ancho_aj); columnas_x++)
                                    {
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            rgb_tmp[(filas_y * int(ancho_aj) * 3) + (columnas_x * 3) + cont_rgb] = rgb_entrada[(((int(alto_in) - int(alto_aj)) / 2) * int(ancho_in) * 3) + (((int(ancho_in) - int(ancho_aj)) / 2) * 3) + (filas_y * int(ancho_in) * 3) + (columnas_x * 3) + cont_rgb];
                                        }
                                    }
                                }

                                rgb_entrada = rgb_tmp;
                                std::vector<float>().swap(rgb_tmp);
                            }

                            unsigned short ancho_fin;
                            unsigned short alto_fin;

                            //DETERMINAR ORIENTACIÓN DE IMAGEN DE SALIDA
                            if (ancho_aj > alto_aj)
                            {
                                ancho_fin = 1920, alto_fin = 1080;
                            }
                            else
                            {
                                ancho_fin = 1080, alto_fin = 1920;
                            }

                            std::vector<std::optional<float>> rgb_salida(ancho_fin * alto_fin * 3);

                            //SI LA IMAGEN PROCESADA NO TIENE LAS DIMENSIONES DE SALIDA
                            if (!(ancho_aj == 1920 && alto_aj == 1080 || ancho_aj == 1080 && alto_aj == 1920))
                            {
                                unsigned short ancho_min;
                                unsigned short alto_min;
                                unsigned int iter1 = 0, iter2 = 0;
                                unsigned short norm_x = 0;
                                unsigned int norm_y = 0;

                                //OBTENER DIMENSIONES MÁS PEQUEÑAS
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

                                //COPIAR PÍXELES DE IMAGEN DE ENTRADA HACIA IMAGEN DE SALIDA SEGÚN PROPORCIONES ENTRE AMBAS
                                for (unsigned short filas_y = 0; filas_y < alto_min; filas_y++)
                                {
                                    for (unsigned short columnas_x = 0; columnas_x < ancho_min; columnas_x++)
                                    {
                                        //NORMALIZAR COORDENADA X
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

                                        //NORMALIZAR COORDENADA Y
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

                                        //REASIGNAR POSICIONES DE PÍXELES EN MATRIZ DE SALIDA / INTERPOLACIÓN VECINO MÁS CERCANO (IMAGEN DE SALIDA CON AMBAS DIMENSIONES MENORES A IMAGEN DE ENTRADA)
                                        for (unsigned short cont_rgb = 0; cont_rgb < 3; cont_rgb++)
                                        {
                                            if (ancho_fin >= ancho_aj && alto_fin >= alto_aj)
                                            {
                                                rgb_salida[norm_y + norm_x + cont_rgb] = rgb_entrada[(filas_y * ancho_aj * 3) + (columnas_x * 3) + cont_rgb];
                                            }

                                            if (ancho_fin <= ancho_aj && alto_fin < alto_aj || ancho_fin < ancho_aj && alto_fin <= alto_aj)
                                            {
                                                rgb_salida[(filas_y * ancho_fin * 3) + (columnas_x * 3) + cont_rgb] = rgb_entrada[norm_y + norm_x + cont_rgb];
                                            }

                                            if (ancho_fin > ancho_aj && alto_fin < alto_aj)
                                            {
                                                rgb_salida[(filas_y * ancho_fin * 3) + norm_x + cont_rgb] = rgb_entrada[norm_y + (columnas_x * 3) + cont_rgb];
                                            }

                                            if (ancho_fin < ancho_aj && alto_fin > alto_aj)
                                            {
                                                rgb_salida[norm_y + (columnas_x * 3) + cont_rgb] = rgb_entrada[(filas_y * ancho_aj * 3) + norm_x + cont_rgb];
                                            }
                                        }
                                    }
                                }

                                //INTERPOLACIÓN BICÚBICA HORIZONTAL
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

                                //INTERPOLACIÓN BICÚBICA VERTICAL
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

                                //REDONDEO
                                for (unsigned int iter_mtz = 0; iter_mtz < rgb_salida.size(); iter_mtz++)
                                {
                                    rgb_salida[iter_mtz] = std::round(rgb_salida[iter_mtz].value());
                                }
                            }

                            std::vector<float>().swap(rgb_entrada);

                            //ENCABEZADO DE BMP
                            unsigned char enc_bmp[54] = { 0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

                            //DIMENSIONES DE IMAGEN DE SALIDA EN ARREGLO FINAL
                            enc_bmp[18] = static_cast<unsigned char>(ancho_fin - (unsigned char(ancho_fin / 256) * 256));
                            enc_bmp[19] = static_cast<unsigned char>(ancho_fin / 256);
                            enc_bmp[22] = static_cast<unsigned char>(alto_fin - (unsigned char(alto_fin / 256) * 256));
                            enc_bmp[23] = static_cast<unsigned char>(alto_fin / 256);

                            std::string bmp_salida;

                            //PASAR ENCABEZADO A ARREGLO FINAL
                            for (unsigned char ch_ch : enc_bmp)
                            {
                                bmp_salida.push_back(ch_ch);
                            }

                            //PASAR MATRIZ DE SALIDA A ARREGLO FINAL
                            for (std::optional<unsigned char> ch_ch : rgb_salida)
                            {
                                bmp_salida.push_back(ch_ch.value());
                            }

                            std::vector<std::optional<float>>().swap(rgb_salida);

                            if (std::filesystem::exists("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.bmp"))
                            {
                                std::filesystem::remove("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.bmp");
                            }

                            //ESCRIBIR ARREGLO FINAL EN ARCHIVO BMP Y CERRAR ARCHIVO 
                            std::ofstream bmp_arch("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.bmp", std::ios::binary);
                            bmp_arch.write(bmp_salida.data(), bmp_salida.size());
                            bmp_arch.close();

                            if (std::filesystem::exists(dcim_dir.path().string()))
                            {
                                std::filesystem::remove(dcim_dir.path().string());
                            }

                            //MOSTRAR IMAGEN PROCESADA EN CONSOLA
                            std::filesystem::rename("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/tmpimg.bmp", "C:/Users/" + n_usr + "/Desktop/gallerydir/" + dcim_dir.path().stem().string() + ".bmp");

                            std::cout << dcim_dir.path().string() + "\n";

                            n++;
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
                std::cout << "No JPG files with dimensions greater or smaller than 1920x1080 were found\n";
            }
        }
        else
        {
            if (std::filesystem::is_directory("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/"))
            {
                std::filesystem::remove_all("C:/Users/" + n_usr + "/Desktop/gallerydir/tmpfdr/");
            }

            std::cout << "No JPG files were found\n";
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
