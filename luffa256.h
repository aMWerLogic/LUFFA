#include <stdio.h>
#include <stdint.h>

//LUFFAv2 Sbox
uint32_t  Sbox[16] = {13,14,0,1,5,10,7,6,11,3,9,12,15,8,2,4};

//initial values of constants for w=3
uint32_t ivc[] = {0x181cca53,0x380cde06,0x5b6f0876,0xf16f8594,0x7e106ce9,0x38979cb0};

//constants used in AddConst block
uint32_t con[6] = {0x0};

//constants generator - not necessery but included in implementation
//v1,v2 - initial values for given Internal State
//c1 - constant for given Internal State
void FL(uint32_t *v1, uint32_t *v2, uint32_t *c1)
{
uint32_t tl = *v1;
uint32_t tr = *v2;
uint32_t c;
c = tl >> 31;
tl = (tl << 1) | (tr >> 31);
tr = tr << 1;
if (c == 1){ tl ^= 0xc4d6496c; tr ^= 0x55c61c8d; }
uint32_t pom=tl;
tl = tr;
tr = pom;
*c1 = tr; /* k=0,4 */
*v1 = tl;
*v2 = tr;
}


//multiplication by 0x02 (by x) according to specification
void mult_2(uint32_t *x)
{
    uint32_t m_temp = *(x+7);
    for(int i=7; i>0; i--)
    {
        *(x+i) = *(x+i-1);
    }
    *(x) = m_temp;
    *(x+4) = *(x+4) ^ m_temp;
    *(x+3) = *(x+3) ^ m_temp;
    *(x+1) = *(x+1) ^ m_temp;
}

//Message Injection 
//internal - internal state 24 * 32b, buffer - message block 8 * 32b
void MI(uint32_t *internal, uint32_t *buffer)
{
    uint32_t temp[8]={0};

    for(int i=0; i<8; i++)
    {
        temp[i] = (*(internal+i)) ^ (*(internal+(i+8))) ^ (*(internal+(i+16)));
    }
    mult_2(temp);
    for(int i=0; i<=16; i=i+8)
    {
        for(int j=0; j<8; j++)
        {
            *(internal+i+j) = temp[j] ^ *(internal+i+j) ^ *(buffer+j);
        }
        mult_2(buffer);
    }
}

//left rotation by j bits in 32b register 
uint32_t left_rot(uint32_t x, unsigned int j)
{
    j=j%32;
    return (x<<j)|(x>>(32-j));
}

//TWEAK block
//left rotation by j of 4 least significant words of internal state
void tweak(uint32_t *x, int j)
{
    for(int i=4; i<8; i++)
        *(x+i) = left_rot(*(x+i), j);
}


//SubCrumb block
//it's just 4x4 Sbox
void SubCrumb(uint32_t *x1, uint32_t *x2, uint32_t *x3, uint32_t *x4)
{
    int slice = 0x1;

    uint32_t index = 0;
    int shift=3;
    for(int i=0; i<32; i++)
    {
        
        index = left_rot((*x1 & slice),shift+32) | left_rot((*x2 & slice),shift-1+32)  | left_rot((*x3 & slice),shift-2+32)  | left_rot((*x4 & slice),shift-3+32);
        shift--;
        slice=slice*2;
        index=Sbox[index];
        uint32_t mask = 1 << i;
        *x1 = ((*x1) & ~mask) | (((index & 0x8) >> 3) << i);
        *x2 = ((*x2) & ~mask) | (((index & 0x4) >> 2) << i);
        *x3 = ((*x3) & ~mask) | (((index & 0x2) >> 1) << i);
        *x4 = ((*x4) & ~mask) | ((index & 0x1) << i);
    }

}

//MixWord block
void MixWord(uint32_t *x1, uint32_t *x2)
{
    *x2 = *x2 ^ *x1;
    *x1 = left_rot(*x1, 2) ^ (*x2);
    *x2 = left_rot(*x2, 14) ^ (*x1);
    *x1 = left_rot(*x1, 10) ^ (*x2);
    *x2 = left_rot(*x2, 1);
}

//AddConst block
//x - internal value, v - initial values of CONST generator FL, c - pointer to proper constants
void AddConst(uint32_t *x, uint32_t *v, uint32_t *c)
{
    FL(v, v+1, c);
    FL(v, v+1, c+1);
    *x = (*x) ^ *c;
    *(x+4) = (*(x+4)) ^ (*(c+1)); 
}


//Single Qj permutation block
void Q_perm(uint32_t *x, int j)
{
    tweak(x,j);
    for(int r=0; r<8; r++)
    {
        SubCrumb(x+3, x+2, x+1, x);
        SubCrumb(x+4, x+7, x+6, x+5);
        for(int k=0; k<4; k++)
        {
            MixWord((x+k), (x+k+4));
        }
        AddConst(x, &ivc[2*j], &con[2*j]);
    }
}


//Resets constants for another round
void reset_con()
{
    ivc[0] =0x181cca53;
    ivc[1] =0x380cde06;
    ivc[2] =0x5b6f0876;
    ivc[3] =0xf16f8594;
    ivc[4] =0x7e106ce9;
    ivc[5] =0x38979cb0;
    for(int i =0 ; i<6; i++)
        con[i] = 0x0;
}

//Round function
void Round(uint32_t *internal, uint32_t *buff)
{
    MI(internal, buff);
    for(int j=0; j<3; j++)
    {
        Q_perm((internal+j*8), j);
    }
    reset_con();
}

//Last round of algorithm 
//Z - 8 x 32b words array for storing a hash
void Finalization(uint32_t *internal, uint32_t *Z)
{
    uint32_t message[8] = {0x0};
    Round(internal,message);
    for(int i=0; i<8; i++)
    {
        *(Z+i) = *(internal+i) ^ *(internal+i+8) ^ *(internal+i+16);
    }
}













