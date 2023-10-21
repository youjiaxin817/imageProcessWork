#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef struct fileHeader
{
    uint16_t bfType;//位图文件类型
    uint32_t BfSize;//位图文件大小
    uint32_t bfReserverd1;//位图保留字
    uint32_t bfOffBits;//位图数据起始位置，
    /* 图像文件头0-13字节*/
}bitMapFileHeader;

typedef struct infoHeader
{
    uint32_t biSize;//本结构所使用字节数
    uint32_t biWidth;//位图宽度
    uint32_t biHeight;//位图高度
    uint16_t biPlanes;//目标设备级别
    uint16_t biBitCount;//每个像素所需像素位
    uint32_t biCompression;//位图压缩类型
    uint32_t biSizeImage;//位图大小
    uint32_t biXpelsPerMeter;//位图水平分辨率
    uint32_t biYpelsPerMeter;//位图垂直分辨率
    uint32_t biCirUsed;//位图实际使用颜色表中的颜色数量
    uint32_t biClrImportant;//对图像显示有重要影响的颜色索引数量

    /* 位图信息头14-53字节*/
}bitMapInfoHeader;

typedef struct palettes
{
    uint8_t rgbBlue;//蓝色亮度
    uint8_t rgbGreen;//绿色亮度
    uint8_t rgbRed;//红色亮度
    uint8_t rgbReserved;//保留  
    /* 调色板*/
}palette;


typedef struct
{
    bitMapFileHeader bitMapFileHeaders;
    bitMapInfoHeader bitMapInfoHeaders;
    palette palettes[256];
    uint8_t * imgData;
    /* data */
}BMP;

typedef struct BMP1
{
    bitMapFileHeader bitMapFileHeaders;
    bitMapInfoHeader bitMapInfoHeaders;
    uint8_t * imgData;
    /* data */
}BMP1;
#pragma pack(1)
//函数声明
void readBMPInfo(char* fileName);
void bmp2raw(char * bmpName,char *rawName);
void colorBmp2raw(char * bmpName,char *rawName);
int main(void){

    char img_in1[] = "lena.bmp";
    char img_in2[] = "lena_C.bmp";
    char img_out1[] = "lena";
    char img_out2[] = "lena_C";
    readBMPInfo(img_in1);
    readBMPInfo(img_in2);
    bmp2raw(img_in1,img_out1);
    colorBmp2raw(img_in2,img_out2);


    return 0;
}



void readBMPInfo(char* fileName){
    FILE* fp = fopen(fileName,"rb");
    printf(">>>>>>>>>>>>>>>>>>>\n%s 图像信息如下：\n",fileName);
    BMP * bmp = (BMP*)malloc(sizeof(BMP));
    if(fp == NULL)
    {
        printf("读取%s失败\n",fileName);
        return ;
    }
    fread(&(bmp->bitMapFileHeaders),1,sizeof(bmp->bitMapFileHeaders),fp);
    if(bmp->bitMapFileHeaders.bfType== 0x4d42)
    {
        printf("该文件为BMP类型文件，可以继续\n");
       
    }
    else{
         printf("该文件不为BMP类型文件，不可以继续\n");
         printf("%x",bmp->bitMapFileHeaders.bfType);
    }


    printf("该文件大小为 %dkB\n",bmp->bitMapFileHeaders.BfSize/1024);
    printf("保留1和保留2为 is %x\n",bmp->bitMapFileHeaders.bfReserverd1);
    printf("位图数据起始位置为 %dByte\n",bmp->bitMapFileHeaders.bfOffBits);
    printf("***************************** \n");
    
    
    fread(&(bmp->bitMapInfoHeaders),1,sizeof(bmp->bitMapInfoHeaders),fp);

    printf("信息头大小为 %dB\n",bmp->bitMapInfoHeaders.biSize);
    printf("图像宽为 %d像素\n",bmp->bitMapInfoHeaders.biWidth);
    printf("图像高为 %d像素\n",bmp->bitMapInfoHeaders.biHeight);
    printf("目标设备等级为 %d\n",bmp->bitMapInfoHeaders.biPlanes);
    printf("每个像素位数为 %dbit\n",bmp->bitMapInfoHeaders.biBitCount);
    printf("图像压缩等级为 %d\n",bmp->bitMapInfoHeaders.biCompression);
    printf("水平分辨率为 %d\n",bmp->bitMapInfoHeaders.biXpelsPerMeter);
    printf("垂直分辨率为 %d\n",bmp->bitMapInfoHeaders.biYpelsPerMeter);
    printf("使用的颜色数为 %d\n",bmp->bitMapInfoHeaders.biCirUsed);
    printf("图像中重要颜色数为 %d\n",bmp->bitMapInfoHeaders.biClrImportant);
    printf("***************************** \n");
    fclose(fp);
    free(bmp);
}



void bmp2raw(char * bmpName,char *rawName){

    FILE *src,*dst,*dst_bmp;
    char name_raw[20] = {};
    char name_bmp[20] = {};

    strcat(name_raw,rawName);
    strcat(name_bmp,rawName);
    char raw_suffix[] = ".raw";
    char bmp_suffix[] = "s.bmp";
    strcat(name_raw,raw_suffix);
    strcat(name_bmp,bmp_suffix);

    src = fopen(bmpName,"rb+");
    dst = fopen(name_raw,"wb+");
    dst_bmp = fopen(name_bmp,"wb+");

    BMP srcImg;
    
    fread(&(srcImg.bitMapFileHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapFileHeaders),src);
    fread(&(srcImg.bitMapInfoHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapInfoHeaders),src);
    fread(srcImg.palettes,sizeof(palette),256,src);
    int width = srcImg.bitMapInfoHeaders.biWidth;
    int height = srcImg.bitMapInfoHeaders.biHeight;
    int pixel_bit = srcImg.bitMapInfoHeaders.biBitCount;
    int offbit = srcImg.bitMapFileHeaders.bfOffBits;
    int bit_size = height*width;
    srcImg.imgData = (uint8_t*)malloc(pixel_bit/8*bit_size);

    if(pixel_bit == 8){
        printf("8位图像\n");
    }
    else{
        printf("24位图像\n");
    }

    fseek(src,offbit,SEEK_SET);

    fread(srcImg.imgData,pixel_bit/8,bit_size,src);

    uint8_t*data = (uint8_t*)malloc(pixel_bit/8*bit_size);



    
    for(int i = 0;i<height;i++){
        for(int j = 0;j<width;j++){
            int pos = i * width + j;
            data[pos] = srcImg.imgData[bit_size - pos -1];
        }
    }


//上面操作左右翻转
    uint8_t*temp_data = (uint8_t*)malloc(pixel_bit/8*bit_size);

    for(int i = 0;i<height;i++){
        for(int j = 0;j<width;j++){
            int pos = i * width + j;
            int srcpos = i*width +(width - j - 1);
            temp_data[pos] = data[srcpos];
        }
    }
    //左右翻转
    fwrite(temp_data,1,bit_size,dst);
    //将数据转成二维数组
    uint8_t **ptr_arr = (uint8_t **)malloc(sizeof(uint8_t*)*height);
    for(int i = 0;i<height;i++){
        
        ptr_arr[i] = &temp_data[i*width];
    }
    uint8_t *quar_data = (uint8_t *)malloc(sizeof(uint8_t)*bit_size*pixel_bit/8/4);
    for(int i = 0;i<height/2;i++){
        for(int j = 0;j<width/2;j++){
            quar_data[i * width/2 + j] = ptr_arr[i][j];
        }
    }
    srcImg.bitMapFileHeaders.BfSize = 54 + bit_size*pixel_bit/8/4;
    srcImg.bitMapInfoHeaders.biHeight = height/2;
    srcImg.bitMapInfoHeaders.biWidth = width/2;
    fwrite(&(srcImg),1,offbit,dst_bmp); 
    //按bmp顺序写入图像数据
    for(int i =height/2-1;i>0;i--){
        fwrite(&quar_data[i*width/2],1,width/2,dst_bmp);
        
    }
}


void colorBmp2raw(char * bmpName,char *rawName){

    FILE *src,*dst,*dst_bmp;
    char name_raw[20] = {};
    char name_bmp[20] = {};

    strcat(name_raw,rawName);
    strcat(name_bmp,rawName);
    char raw_suffix[] = ".raw";
    char bmp_suffix[] = "s.bmp";
    strcat(name_raw,raw_suffix);
    strcat(name_bmp,bmp_suffix);

    src = fopen(bmpName,"rb+");
    

    //24位没有调色板
    BMP1 srcImg;
    
    fread(&(srcImg.bitMapFileHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapFileHeaders),src);
    fread(&(srcImg.bitMapInfoHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapInfoHeaders),src);
    int width = srcImg.bitMapInfoHeaders.biWidth;
    int height = srcImg.bitMapInfoHeaders.biHeight;
    int pixel_bit = srcImg.bitMapInfoHeaders.biBitCount;
    int pixel_byte = pixel_bit/8;
    int offbit = srcImg.bitMapFileHeaders.bfOffBits;
    
    


    //考虑对齐
    int real_width =pixel_byte* (pixel_byte*width%4)?(pixel_byte*width+1):(pixel_byte*width);
    printf("real_width is %d\n",real_width);
    long bit_size = height*real_width;

    srcImg.imgData = (uint8_t*)malloc(bit_size);
    if(pixel_bit == 8){
        printf("8位图像\n");
    }
    else{
        printf("24位图像\n");
    }


    dst = fopen(name_raw,"wb+");
    fread(srcImg.imgData,1,bit_size,src);
    
    //此时图片上下颠倒，三通道不对
    uint8_t * temp_data_raw = (uint8_t*)malloc(bit_size);
    uint8_t * temp_data_bmp = (uint8_t*)malloc(bit_size);
    for(int i = 0;i < height;i++){
        for(int j = 0;j<width;j++){
            int pos = i * real_width + 3 * j;
            int B = pos;
            int G = pos +1;
            int R = pos +2;
            int srcpos = (height-i-1)*real_width +3*j;
            int srcR = srcpos;
            int srcG = srcpos+1;
            int srcB = srcpos+2;
            temp_data_raw[R] =  srcImg.imgData[srcR];
            temp_data_raw[G] =  srcImg.imgData[srcG];
            temp_data_raw[B] =  srcImg.imgData[srcB];

            temp_data_bmp[B] =  srcImg.imgData[srcR];
            temp_data_bmp[G] =  srcImg.imgData[srcG];
            temp_data_bmp[R] =  srcImg.imgData[srcB];
 
        }
    }//经过修整

    fwrite(temp_data_raw,1,bit_size,dst);
    //以上获得原始图片正常
    dst_bmp = fopen(name_bmp,"wb+");
    

    srcImg.bitMapFileHeaders.BfSize = offbit + bit_size/4;
    srcImg.bitMapInfoHeaders.biHeight = height/2;
    srcImg.bitMapInfoHeaders.biWidth = width/2;
    fwrite(&(srcImg.bitMapFileHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapFileHeaders),dst_bmp);
    fwrite(&(srcImg.bitMapInfoHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapInfoHeaders),dst_bmp);
    
    for(int i=height/2;i<height;i++){
        fwrite(&srcImg.imgData[i*real_width],1,real_width/2,dst_bmp);
    }




}