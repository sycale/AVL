#include <iostream>
#include <mmintrin.h>
#include <type_traits>
#include <experimental/random>


using namespace std;

typedef union Converter
{
    int8_t int8_data[8];
    int64_t int64_data;
};

void unpack_mmx8(__m64 data, __m64 res[2]) {
    __m64 MMX_mask = _mm_cmpgt_pi8(_m_from_int(0), data); // Creating mask
    res[0] = _mm_unpacklo_pi8(data, MMX_mask);
    res[1] = _mm_unpackhi_pi8(data, MMX_mask);
}

void MMx_auto_convert(Converter converter, __m64 res[2]) {
    __m64 MMX_data = _m_from_int64(converter.int64_data); // pack A into MMX
     unpack_mmx8(MMX_data, res); // Unpack function
}

int main()
{
    srand(time(NULL));
    // for (int i = 0; i < arr_size; i++)
    // {
    //     A8[i] = rand() % limit - (limit / 2);
    //     B8[i] = rand() % limit - (limit / 2);
    //     C8[i] = rand() % limit - (limit / 2);
    //     D16[i] = rand() % (limit * limit) - (limit * limit / 2);
    // }
    union Converter converterA; // = {0, 1, -50, 3, 4, 5, 6, 7};
    union Converter converterB; //= {0, 1, 2, 3, 4, 5, 6, 7};
    union Converter converterC; //= {0, 1, 2, 3, 4, 5, 6, 7};
    int16_t D[8]; //{0, 1, 2, 3, 4, 5, 6, 7}; // random

    for(int i = 0; i < 8; i++) {
        converterA.int8_data[i] = std::experimental::randint(-125, 125);
        converterB.int8_data[i] = std::experimental::randint(-125, 125);
        converterC.int8_data[i] = std::experimental::randint(-125, 125);
        D[i] = std::experimental::randint(-32000, 32000);
    }


    __m64 MMX_B[2] = {};
    __m64 MMX_C[2] = {};
    __m64 MMX_A[2] = {};
    __m64 MMX_D[2] = {};


    MMx_auto_convert(converterB, MMX_B);
    MMx_auto_convert(converterC, MMX_C);
    MMx_auto_convert(converterA, MMX_A);
    MMX_D[0] = _mm_setr_pi16(D[0], D[1], D[2], D[3]); // pack D[1..3] into MMX
    MMX_D[1] = _mm_setr_pi16(D[4], D[5], D[6], D[7]); // pack D[4..7] into MMX

    __m64 MMX_buf[2] = {};
    __m64 MMX_buffAdd[2] = {};
    __m64 MMX_rez[2] = {};

    MMX_buf[0] = _mm_mullo_pi16(MMX_B[0], MMX_C[0]);
    MMX_buf[1] = _mm_mullo_pi16(MMX_B[1], MMX_C[1]);
    MMX_buffAdd[0] = _mm_add_pi16(MMX_buf[0], MMX_A[0]);
    MMX_buffAdd[1] = _mm_add_pi16(MMX_buf[1], MMX_A[1]);

    MMX_rez[0] = _mm_subs_pi16(MMX_buffAdd[0], MMX_D[0]);
    MMX_rez[1] = _mm_subs_pi16(MMX_buffAdd[1], MMX_D[1]);

    short F[8] = {MMX_rez[0][0], MMX_rez[0][0] >> 16, MMX_rez[0][1], MMX_rez[0][1] >> 16, MMX_rez[1][0], MMX_rez[1][0] >> 16, MMX_rez[1][1], MMX_rez[1][1] >> 16};

    for (int i = 0; i < 8; i++)
    {
        cout << "F[" << i << "] = " << F[i] << endl;
    }

    for(int i = 0; i < 8; i++) {
        cout << "[" << i << "] = " << converterA.int8_data[i] + (static_cast<int16_t>(converterB.int8_data[i]) * static_cast<int16_t>(converterC.int8_data[i])) - D[i] << endl;
    }
}
