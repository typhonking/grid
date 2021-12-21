#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

//ffmpeg -framerate 60 -i frame%03d.bmp -c:v libx264 -crf 0 shortmute.mkv
//ffmpeg -f concat -i list.txt -c copy longlaptop.mkv
//ffmpeg -i 72060.mkv -i grid.wav -c copy 72060audio.mkv
int number=0;//frame number for filenames

  //width, height, and bitcount are the key factors:
    int32_t widthin = 1280;
    int32_t heightin = 720;
    int32_t widthin2 = 1000;
    int32_t heightin2 = 667;
    int32_t widthout = 1280;
    int32_t heightout = 720;
    uint16_t bitcount = 24;//<- 24 or 32-bit bitmap


double screen [2000][2000][4];//width, height,bgra (a is brightness)

double screen2 [2000][2000][4];
int screenout[2000][2000][3];
double clipboard[2000][2000][5];




double alpha (int xcoor, int ycoor){
    double brightness;
    brightness = (screen[xcoor][ycoor][0] + screen[xcoor][ycoor][1] + screen[xcoor][ycoor][2])/3.0f;
    return brightness;
}
double alpha2 (int xcoor, int ycoor){
    double brightness;
    brightness = (screen2[xcoor][ycoor][0] + screen2[xcoor][ycoor][1] + screen2[xcoor][ycoor][2])/3.0f;
    return brightness;
}

void setAlpha (){
    for (int i=0;i<widthin;i++){
        for (int j=0;j<heightin;j++){
            screen [i][j][3]=alpha(i,j);
        }
    }
}
void setAlpha2 (){
    for (int i=0;i<widthin;i++){
        for (int j=0;j<heightin;j++){
            screen2 [i][j][3]=alpha2(i,j);
        }
    }
}


void contrast(int spread, int cutoff, int weakness){

for (int i=0; i<widthin; i++){
    for (int j=0; j<heightin; j++){

        if (screen [i][j][3]<cutoff){
            for (int k=0;k<spread;k++){
                for(int l=0;l<spread;l++){
                    for(int m=0;m<3;m++){
                        int ioffset;
                        int joffset;
                        if (i>=spread) {
                            ioffset =spread;
                            }
                         else ioffset=i;
                        if (j>=spread) {
                            joffset =spread;
                            }
                         else joffset=j;
                        screen [i+k-ioffset][j+l-joffset][m] += ((alpha(i+k-ioffset,j+l-joffset)-70)/weakness);
                        
                        screen [i+k-ioffset][j+l-joffset][3] = alpha(i+k-ioffset,j+l-joffset);//update alpha

                        //screen [i+k-ioffset][j+l-joffset][2] = 250 ;
                       if (screen[i+k-ioffset][j+l-joffset][m]<1) screen[i+k-ioffset][j+l-joffset][m]=1;
                        if (screen[i+k-ioffset][j+l-joffset][m]>250) screen[i+k-ioffset][j+l-joffset][m]=250;
                        
                    }
                }
            }
            
        }

    }
}
}

void anticontrast(int spread, int cutoff, int weakness){

for (int i=0; i<widthin; i++){
    for (int j=0; j<heightin; j++){

        if (screen [i][j][3]<cutoff){
            for (int k=0;k<spread;k++){
                for(int l=0;l<spread;l++){
                    for(int m=0;m<3;m++){
                        int ioffset;
                        int joffset;
                        if (i>=spread) {
                            ioffset =spread;
                            }
                         else ioffset=i;
                        if (j>=spread) {
                            joffset =spread;
                            }
                         else joffset=j;
                        screen [i+k-ioffset][j+l-joffset][m] -= ((alpha(i+k-ioffset,j+l-joffset)-70)/weakness);
                        
                        screen [i+k-ioffset][j+l-joffset][3] = alpha(i+k-ioffset,j+l-joffset);//update alpha

                        //screen [i+k-ioffset][j+l-joffset][2] = 250 ;
                       if (screen[i+k-ioffset][j+l-joffset][m]<1) screen[i+k-ioffset][j+l-joffset][m]=1;
                        if (screen[i+k-ioffset][j+l-joffset][m]>250) screen[i+k-ioffset][j+l-joffset][m]=250;
                        
                    }
                }
            }
            
        }

    }
}
}


void inputbmp(char filename[50]){
        // Input file


    //take padding in to account
    int width_in_bytes;
     width_in_bytes= ((widthin * bitcount + 31) / 32) * 4;

    //total image size in bytes, not including header
    uint32_t imagesize = width_in_bytes * heightin;

    //this value is always 40, it's the sizeof(BITMAPINFOHEADER)
    const uint32_t biSize = 40;

    //bitmap bits start after headerfile, 
    //this is sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    const uint32_t bfOffBits = 54; 

    //total file size:
    //uint32_t filesize = 54 + imagesize;

    //number of planes is usually 1
    const uint16_t biPlanes = 1;

    //testing...
    
    unsigned char* readwidth = malloc(1);



    //prepare pixel data for ARRAY:
    unsigned char* bitmapImage = malloc(imagesize);

    FILE *fin = fopen(filename,"rb");

    fseek(fin, 18, SEEK_SET); 
    fread(readwidth,1,1,fin);
    //printf("width is %u\n", readwidth);
    free (readwidth);

    //move file pointer to the beginning of bitmap data
    fseek(fin, bfOffBits, SEEK_SET);

        //read in the bitmap image data
    fread(bitmapImage,imagesize,1,fin);
    fclose(fin);

    for(int row = heightin - 1; row >= 0; row--)
        {
            for(int col = 0; col < widthin; col++)
            {
                screen [col][row][0] = bitmapImage[row * width_in_bytes + col * 3 + 0];//blue
                screen [col][row][1] = bitmapImage[row * width_in_bytes + col * 3 + 1];//green
                screen [col][row][2] = bitmapImage[row * width_in_bytes + col * 3 + 2];//red
            }
        }
    free (bitmapImage);
}

void inputbmp2(char filename[50]){
        // Input file


    //take padding in to account
    int width_in_bytes;
     width_in_bytes= ((widthin2 * bitcount + 31) / 32) * 4;

    //total image size in bytes, not including header
    uint32_t imagesize = width_in_bytes * heightin2;

    //this value is always 40, it's the sizeof(BITMAPINFOHEADER)
    const uint32_t biSize = 40;

    //bitmap bits start after headerfile, 
    //this is sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    const uint32_t bfOffBits = 54; 

    //total file size:
    //uint32_t filesize = 54 + imagesize;

    //number of planes is usually 1
    const uint16_t biPlanes = 1;

    //testing...
    
    unsigned char* readwidth = malloc(1);



    //prepare pixel data for ARRAY:
    unsigned char* bitmapImage = malloc(imagesize);

    FILE *fin = fopen(filename,"rb");

    fseek(fin, 18, SEEK_SET); 
    fread(readwidth,1,1,fin);
    printf("width is %u\n", readwidth);
    free (readwidth);

    //move file pointer to the beginning of bitmap data
    fseek(fin, bfOffBits, SEEK_SET);

        //read in the bitmap image data
    fread(bitmapImage,imagesize,1,fin);
    fclose(fin);

    for(int row = heightin2 - 1; row >= 0; row--)
        {
            for(int col = 0; col < widthin2; col++)
            {
                screen2 [col][row][0] = bitmapImage[row * width_in_bytes + col * 3 + 0];//blue
                screen2 [col][row][1] = bitmapImage[row * width_in_bytes + col * 3 + 1];//green
                screen2 [col][row][2] = bitmapImage[row * width_in_bytes + col * 3 + 2];//red
            }
        }
    free (bitmapImage);
}

void pushAlpha(){
    for (int i=0;i<widthin;i++){
        for (int j=0;j<heightin;j++){
            double newAlpha=0;
            for (int m=0;m<3;m++){
                newAlpha +=screen[i][j][m];
            }
            for (int m=0;m<3;m++){
                screen [i][j][m]+=(newAlpha-screen[i][j][3])/(3.0f);
            }
        }
    }
}
void pushClipboardAlpha(){
    for (int i=0;i<widthin;i++){
        for (int j=0;j<heightin;j++){
            for (int m=0;m<3;m++){
                screen [i][j][m]+=(clipboard[i][j][3]-screen[i][j][3])/3.0f;
            }
        }
    }
}

void lineBlurring(){
    int ioffset=0;
    int joffset=0;
    for (int i=0;i<widthin;i++){
        for (int j=0;j<heightin;j++){
            double alphabuffer=0;
            for (int k=0;k<4;k++){
                if (i>2 && (widthin-2)>i) {
                    ioffset =k;
                }
                else if (i<3){
                    ioffset = i;
                }
                else ioffset = 4-widthin+i;
                alphabuffer+=screen[i+ioffset-2][j][3];
            }

            for (int l=0;l<4;l++){
                if (j>2 && (heightin-2)>j) {
                    joffset =l;
                }
                else if (j<5){
                    joffset = j;
                }
                else joffset = 4-heightin+j;
                alphabuffer+=screen[i][j+joffset-2][3];
            }
            clipboard[i][j][3]=alphabuffer/16.0f;
        }
    }
}

void outputbmp(char filename[50]){
                  //Output File
 //take padding in to account
     int32_t bitcountout=24;

    int width_in_bytes;
     width_in_bytes= ((widthout * bitcountout + 31) / 32) * 4;

    //total image size in bytes, not including header
    uint32_t imagesize = width_in_bytes * heightout;

    //this value is always 40, it's the sizeof(BITMAPINFOHEADER)
    const uint32_t biSize = 40;

    //bitmap bits start after headerfile, 
    //this is sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    const uint32_t bfOffBits = 54; 

    //total file size:
    //uint32_t filesize = 54 + imagesize;

    //number of planes is usually 1
    const uint16_t biPlanes = 1;

    //total file size:
    uint32_t filesize = 54 + imagesize;


   //create header:
    //copy to buffer instead of BITMAPFILEHEADER and BITMAPINFOHEADER
    //to avoid problems with structure packing
    unsigned char header[54] = { 0 };
    memcpy(header, "BM", 2);
    memcpy(header + 2 , &filesize, 4);
    memcpy(header + 10, &bfOffBits, 4);
    memcpy(header + 14, &biSize, 4);
    memcpy(header + 18, &widthout, 4);
    memcpy(header + 22, &heightout, 4);
    memcpy(header + 26, &biPlanes, 2);
    memcpy(header + 28, &bitcountout, 2);
    memcpy(header + 34, &imagesize, 4);

  //prepare pixel data:
    unsigned char* buf = malloc(imagesize);
  
    for(int row = heightout - 1; row >= 0; row--)
    {
        for(int col = 0; col < widthout; col++)
        {
            buf[row * width_in_bytes + col * 3 + 0] = screen [col][row][0];//blue
            buf[row * width_in_bytes + col * 3 + 1] = screen [col][row][1];//green
            buf[row * width_in_bytes + col * 3 + 2] = screen [col][row][2];//red
        }
    }
    
    FILE *fout = fopen(filename, "wb");
    fwrite(header, 1, 54, fout);
    fwrite((char*)buf, 1, imagesize, fout);
    fclose(fout);
    free(buf);

}

void rotate(double degrees){
    double  radians=3.14159*180*degrees;
    for (int i=0;i<widthin;i++){
        for(int j=0;j<heightin;j++){

            double p=0;
            double q=0;
            p=cos(radians)*i-sin(radians)*j;
            q=sin(radians)*i+cos(radians)*j;
            int pint =p;
            int qint =q;
            for(int m=0;m<3;m++){
                clipboard[pint][qint][m]=screen[i][j][m];            
            }
        }
    }
        
}
void silkScreen (char filename[50],double weight){
    inputbmp2(filename);
    setAlpha2();
    double transparency;
    for (int i=0;i<widthin2;i++){
        for (int j=0;j<heightin2;j++){
            transparency=screen2[i][j][3]*weight;
            for (int m=0;m<3;m++){
                screen[i][j][m]=(1.0f-transparency)*screen[i][j][m]+transparency*clipboard[i][j][m];
            }
        }
    }
}
void collage (char filename[50],double transparency){
    inputbmp2(filename);
    setAlpha2();
    for (int i=0;i<widthin2;i++){
        for (int j=0;j<heightin2;j++){
            for (int m=0;m<3;m++){
                screen[i][j][m]=(1.0f-transparency)*screen[i][j][m]+transparency*screen2[i][j][m];
            }
        }
    }
}
void collageCentre (char filename[50],double transparency){
    inputbmp2(filename);
    setAlpha2();
    for (int i=100;i<widthin2+100;i++){
        for (int j=100;j<600;j++){
            for (int m=0;m<3;m++){
                screen[i][j][m]=(1.0f-transparency)*screen[i][j][m]+transparency*screen2[i-100][j-100][m]-256/10;
            }
        }
    }
}
void overlay(double transparency){
    for (int i=0;i<widthin;i++){
        for (int j=0;j<heightin;j++){
            for (int m=0;m<3;m++){
                screen[i][j][m]=(1.0f-transparency)*screen[i][j][m]+transparency*clipboard[i][j][m];
            }
        }
    }
}

void drawGrid(int border,int number){
    int size=(heightin -border*2)/number;
    for (int j=100;j<=heightin-100;j+=size){
        for (int i=100;i<widthin-100;i++){
            for (int m=0;m<3;m++){

                screen [i][j-2][m]=screen [i][j][m];
                screen [i][j-1][m]=screen [i][j][m];
                screen [i][j+1][m]=screen [i][j][m];
                screen [i][j+2][m]=screen [i][j][m];                     
            }
        }
    }
    for (int i=100;i<=widthin-100;i+=size){
        for (int j=100;j<heightin-100;j++){
            for (int m=0;m<3;m++){
                screen [i-2][j][m]=screen [i][j][m];
                screen [i-1][j][m]=screen [i][j][m];
                screen [i+1][j][m]=screen [i][j][m];
                screen [i+2][j][m]=screen [i][j][m];

            }
        }
    }
}
void drawGridTransparent(int size, int borderx, int bordery,int number,double opacity){//opacity in thousandths 52,163,100,10,number*3
    for (int j=borderx;j<=heightin-borderx;j+=size){
        for (int i=bordery;i<=widthin-bordery;i++){
            for (int m=0;m<3;m++){
                for (int k=0;k<4;k++){
                screen [i][j+k-2][m]=screen [i][j][m]*opacity/1000.0f+screen [i][j+k-2][m]*(1-opacity/1000.0f);
                }
                  
            }
        }
    }
    for (int i=bordery;i<=widthin-bordery;i+=size){
        for (int j=borderx;j<=heightin-borderx;j++){
            for (int m=0;m<3;m++){
                for (int k=0;k<4;k++)
                screen [i+k-2][j][m]=screen [i][j][m]*opacity/1000.0f+screen [i+k-2][j][m]*(1-opacity/1000.0f);

            }
        }
    }
}

void drawGridTransparentLingering(int border,int number,double opacity,int frame){//opacity in thousandths
    int size=(heightin -border*2)/number;
    for (int j=100;j<=heightin-100;j+=size){
        for (int i=100;i<widthin-100;i++){
            for (int m=0;m<3;m++){
               
                for (int k=0;k<4;k++){
                    screen [i][j+k-2][m]=(screen [i][j][m]*(1-frame/333.0f)+ clipboard [i][j][m]*frame/333.0f)*opacity/1000.0f+screen [i][j+k-2][m]*(1-opacity/1000.0f);
                }
                  
            }
        }
    }
    for (int i=100;i<=widthin-100;i+=size){
        for (int j=100;j<heightin-100;j++){
            for (int m=0;m<3;m++){
                

                for (int k=0;k<4;k++)
                    screen [i+k-2][j][m]=(screen [i][j][m]*(1-frame/333.0f)+ clipboard [i][j][m]*frame/333.0f)*opacity/1000.0f+screen [i+k-2][j][m]*(1-opacity/1000.0f);

            }
        }
    }
}


void drawGridBlur(int border,int number){
    int size=(heightin -border*2)/number;
    for (int j=100;j<=heightin-100;j+=size){
        for (int i=100;i<widthin-100;i++){
            for (int m=0;m<3;m++){

                clipboard [i][j-2][m]=screen [i][j-2][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i][j-1][m]=screen [i][j-1][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i][j+1][m]=screen [i][j+1][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i][j+2][m]=screen [i][j+2][m]*0.5f+screen [i][j][m]*0.5f;                     
                clipboard [i-2][j][m]=screen [i-2][j][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i-1][j][m]=screen [i-1][j][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i+1][j][m]=screen [i+1][j][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i+2][j][m]=screen [i+2][j][m]*0.5f+screen [i][j][m]*0.5f;
                for (int k=0;k<4;k++){
                    clipboard [i-2][j+k-2][m]=screen [i-2][j+k-2][m]*0.333f+clipboard [i-2][j+k-2][m]*0.666f;
                    clipboard [i-1][j+k-2][m]=screen [i-1][j+k-2][m]*0.333f+clipboard [i-1][j+k-2][m]*0.666f;
                    clipboard [i+1][j+k-2][m]=screen [i+1][j+k-2][m]*0.333f+clipboard [i+1][j+k-2][m]*0.666f;
                    clipboard [i+2][j+k-2][m]=screen [i+2][j+k-2][m]*0.333f+clipboard [i+2][j+k-2][m]*0.666f;
                }
            }
        }
    }
    for (int i=100;i<=widthin-100;i+=size){
        for (int j=100;j<heightin-100;j++){
            for (int m=0;m<3;m++){
                clipboard [i-2][j][m]=screen [i-2][j][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i-1][j][m]=screen [i-1][j][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i+1][j][m]=screen [i+1][j][m]*0.5f+screen [i][j][m]*0.5f;
                clipboard [i+2][j][m]=screen [i+2][j][m]*0.5f+screen [i][j][m]*0.5f;
                for (int k=0;k<4;k++){
                    clipboard [i+k-2][j-2][m]=screen [i+k-2][j-2][m]*0.333f+clipboard [i+k-2][j-2][m]*0.666f;
                    clipboard [i+k-2][j-1][m]=screen [i+k-2][j-1][m]*0.333f+clipboard [i+k-2][j-1][m]*0.666f;
                    clipboard [i+k-2][j+1][m]=screen [i+k-2][j+1][m]*0.333f+clipboard [i+k-2][j+1][m]*0.666f;
                    clipboard [i+k-2][j+2][m]=screen [i+k-2][j+2][m]*0.333f+clipboard [i+k-2][j+2][m]*0.666f;
                }
            }
        }
    }
}
void noise(int contrast, int brightness){
    int range=contrast*2.56f;
    for (int i=0;i<widthin;i++){
    for (int j=0;j<heightin;j++){
        for (int m=0;m<3;m++){
            screen[i][j][m]=256-range-(100-brightness)*256 + rand() %range;
            }
        }
    }
}
void noiseContrast(){
    for (int i=0;i<widthout;i++){
    for (int j=0;j<heightout;j++){
        for (int m=0;m<3;m++){
            screen[i][j][m]=rand() %256;
            }
        }
    }
}
void copyScreen(){
    for (int i=0;i<widthin;i++){
        for (int j=0;j<heightin;j++){
            for (int m=0;m<3;m++){
                clipboard[i][j][m]=screen[i][j][m];
            }
        }
    }
}
void build(){
    char filename[50];
char *filestring=&filename[0];
    for (int i=0;i<333;i++){
    //noise(60,30);
    noiseContrast();
number++;
//drawGridTransparentLingering(100,10,(number*3),number);
drawGridTransparent(52,100,120,10,number*3);
if (number<10) {sprintf(filestring,"frame00%d.bmp",number);}
else if (number<100) {sprintf(filestring,"frame0%d.bmp",number);}
else {sprintf(filestring,"frame%d.bmp",number);}
puts(filename);
outputbmp(filestring);
}
for (int i=0;i<333;i++){
    //noise(60,30);
    noiseContrast();
    number++;
    double opacity = (1000 - (number-333)*3);
    //drawGridTransparentLingering(100,10,opacity,666-number);
    drawGridTransparent(52,100,120,10,opacity);
    sprintf(filestring,"frame%d.bmp",number);
    puts(filename);
    outputbmp(filestring);

}
/*
noiseContrast();
drawGridTransparent(52,100,120,10,1000);
outputbmp("test.bmp");
*/

system("ffmpeg -framerate 60 -i frame%03d.bmp -c:v libx264 -crf 0 72060.mkv");
number =0;
for (int i=0;i<666;i++){
    number++;
    if (number<10) {sprintf(filestring,"frame00%d.bmp",number);}
        else if (number<100) {sprintf(filestring,"frame0%d.bmp",number);}
            else {sprintf(filestring,"frame%d.bmp",number);}
    remove(filestring);
}

system("ffmpeg -i 72060.mkv -i grid.wav -c copy audio.mkv");
remove("72060.mkv");

system("ffmpeg -f concat -i list.txt -c copy 72060concat.mkv");
remove("audio.mkv");

}

int main(){
srand(time(NULL));   // Initialization, should only be called once.
//copyScreen();
//inputbmp("audio.mp3");    
//setAlpha ();
//anticontrast(8,90,10);
//lineBlurring();
//rotate(60);
//overlay(0.5);
//overlay(1.0);
//collageCentre("archcontrast5.bmp",0.4);

//pushClipboardAlpha();
/*
for (int i=0;i<widthin;i++){
    for (int j=0;j<heightin;j++){
        for (int m=0;m<3;m++){
            int buffer;
            for (int k=0;k<9;k++){
                buffer +=screen[i+k-4][j][m]
            }  
            if(screen[i][j][0]+screen[i][j][1]+screen[i][j][2]<500)screen[i][j][m]=0;
        }
    }
}*/
//contrast(4,130,10);

 //   noiseContrast();
//copyScreen();
    if( access( "72060concat.mkv", F_OK ) == -1 ) {
    build();
}

system("ffplay 72060concat.mkv");


return 0;
}


