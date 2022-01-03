#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/sha.h>


unsigned short xorshift_rand(unsigned short x)
{

x ^= x << 13;
x ^= x << 17;
x ^= x << 5;

return x;
}

void read_top(char *arr)
{
  memset(arr, 0, 1000);
  FILE * fp;

  fp = fopen ("/tmp/.mytopoutput", "r");

  for(int i=0;i<1000; i++)
  {
    arr[i]=fgetc(fp);

    if( feof(fp) )
      break ;
  }
  fclose(fp);
}

// How similar are these two arrays; Very rudimentary percent similarity calculation
int percentsimilar(char *arr1, char *arr2)
{
int similar=0;

for(int i=0; i<1000; i++)
{

if (arr1[i] == arr2[i])
similar++;

}

return (similar/10); // since the total is 1000 character div by 10 would give us the percentage 

}

void main()
{

unsigned short src1 = 0;
unsigned short src2 = 0;
time_t t;
int key = 0;
int key_prev = 0;
char topoutput[1000];
char topoutput_prev[1000];

//Initialize the above char arrays
memset(topoutput, 0, 1000);
memset(topoutput_prev, 1, 1000);

//Note: This is just to prove the concept of random character genration (like /dev/random)
//I don't claim this to be a good random number generator 
//Further more I have not done exception or error checking
//I have not used secure c functions or kept security in mind while coding this

while(1)
{
  //Using the different sources from the envrionment for entropy. Increase this by adding more sources.
  //Source 1 : From time

  t = time(NULL);
  src1 = t%USHRT_MAX;

  //Use the xorshift function above to get a 'key' which you can use to xor data from another source(s)


  key=xorshift_rand(src1);

  //Get output of top command as another source. Alternatively you can read some common files 
  //from /proc filesystem by chosing the ones which changes often.

  system("top -b -n1 > /tmp/.mytopoutput");

  //Take the first 1000 bytes for processing
  read_top(topoutput);

  //If the previous data from soruce (top) more than 70% similar wait for few secconds and check again 
  // (and the key is not the prev key)
  if(percentsimilar(topoutput, topoutput_prev) > 70 || key == key_prev)
  {
    sleep(2);
  }
  else
  {
    //xor the output from top with the 'key' we formed above and send it to stdout
  
    for (int i=0; i < 1000; i++)
    {
      putchar(topoutput[i] ^ key%256);
    }

    memcpy(topoutput_prev, topoutput, 1000);
    key_prev = key;
  }

}// End while

}
