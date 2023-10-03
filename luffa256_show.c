//ALEKSANDER WIĄCEK
//LUFFAv2 256b - REFERAL IMPLEMENTATION
//with internal values printing
//argv[1] - file to read
//argv[2] - file to save data to
#include <time.h>
#include "luffa256_show.h"
#include "WordsConv.h"

clock_t start, end;
double time_taken;
//fpin - read file, fpout - write file
FILE *fpin,*fpout;

uint32_t internal[] = {   //w=3
0x6d251e69, 0x44b051e0, 0x4eaa6fb4, 0xdbf78465,
0x6e292011, 0x90152df4, 0xee058139, 0xdef610bb,
0xc3b44b95, 0xd9d2f256, 0x70eee9a0, 0xde099fa3,
0x5d9b0557, 0x8fc944b3, 0xcf1ccf0e, 0x746cd581,
0xf7efc89d, 0x5dba5781, 0x04016ce5, 0xad659c05,
0x0306194f, 0x666d1836, 0x24aa230a, 0x8b264ae7
};


//MAIN//////////////////////////
int main(int argc, char **argv)
{

if (argc < 3)
    {
        fprintf (stderr, "%s\n", argv[0]);
        return -1;
    }

int h_len; 
printf("Length of hash in bytes: ");
scanf("%d",&h_len);

start = clock();
int N=0; //round counter

fpin = fopen (argv[1], "rb");
if (!fpin)
    {
        fprintf (stderr, "I cannot open a file %s\n", argv[1]);
        return -1;
    }

long long int dl;
fseek(fpin, 0L, SEEK_END);
dl = ftell(fpin);
rewind(fpin);
printf("\nFile name: %s\n",argv[1]);
printf("%s size: %d B\n",argv[1],dl);

long long int c;
c=dl;

uint8_t buffer[32] = {0}; //buffer for one 256b message block
uint32_t message[8] = {0x0}; //used in MI block

while(c>=0) //reading from argv[1] file
{
    for(int i=0; i<32; i++)
    {
        buffer[i]=0x00;
    }
    int j=0;
    if(c<32)
    {
        buffer[c]=0b10000000; //padding
    }
    if(c>0)
    fread(buffer,1,sizeof(buffer),fpin);
    c=c-32;

    w8_to_w32(buffer, message);
    printf("\nMessage block:\n");
    print_block(message);
    printf("\n");
    printf("\nRound %d\n",N);
    Round(internal,message);
    N++;
}
fclose(fpin);

uint32_t Z[8] = {0x0}; //array for storing a hash

Finalization(internal, Z);

printf("\n");

w32_to_w8(buffer, Z);
h_len=h_len%33;
for(int i=0; i<h_len; i++)
{
    printf("%02x",buffer[i]);
}
printf("\n");
printf("\nHash length: %d B",h_len);
fpout = fopen(argv[2], "wb");
if (!fpout)
    {
        fprintf (stderr, "I cannot open a file %s\n", argv[2]);
        return -1;
    }
fwrite(buffer , 1 , h_len , fpout);

fclose(fpout);

end = clock();
time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
printf("\nTime: %f [s]",time_taken);
return 0;
}


