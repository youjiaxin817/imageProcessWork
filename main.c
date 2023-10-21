#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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


typedef struct BGR
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
}BGR;
#pragma pack(1)
//函数声明
void readBMPInfo(char* fileName);
void histImg(char* fileName);
void histImgColor(char* fileName);
void bgrCopy(BGR* bgr,BGR* bgr1);

int main(void){
    char img_in1[] = "image1.bmp";
    char img_in2[] = "image_color.bmp";
    readBMPInfo(img_in1);
    // histImage(img_in1,"image12.bmp");
    histImageColor(img_in2,"image_color2.bmp");
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



void histImage(char* fileName,char * output){
    FILE* src = fopen(fileName,"rb");
    FILE* dst = fopen(output,"wb+"); 
    BMP srcImg;
    fread(&(srcImg.bitMapFileHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapFileHeaders),src);
    fread(&(srcImg.bitMapInfoHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapInfoHeaders),src);
    fread(srcImg.palettes,sizeof(palette),256,src);
    int width = srcImg.bitMapInfoHeaders.biWidth;
    int height = srcImg.bitMapInfoHeaders.biHeight;
    int pixel_bit = srcImg.bitMapInfoHeaders.biBitCount;
    int bit_size = height*width;
    int offbit = srcImg.bitMapFileHeaders.bfOffBits;
    int pixel_count[256] = {0};
    srcImg.imgData = (uint8_t*)malloc(pixel_bit/8*bit_size);
    fseek(src,offbit,SEEK_SET);

    fread(srcImg.imgData,pixel_bit/8,bit_size,src);
    clock_t start,finish;
    start = clock();
    //统计每个像素值的个数，用pixel_count记录
    uint8_t* p= &(srcImg.imgData[0]);
    for(int i = 0;i<bit_size;i++){
        pixel_count[srcImg.imgData[i]]++;
        p++;
    }
    //统计每个像素值的概率，用probability记录
    double probability[256] = {0};
    for(int i = 0;i<256;i++){
        probability[i] = (pixel_count[i] * 1.0) / bit_size;
    }
    //统计每个像素值的累计概率，用sum_probability记录
    double sum_probability[256]= {0};
    for(int i = 0;i<256;i++){
        for(int j = 0;j<=i;j++){
            sum_probability[i] += probability[j];
        }
    }
    //根据累计比例调整灰度值,新灰度值用new_pixel_count记录
    uint8_t new_pixel_count[256]={0};
    for(int i = 0;i<256;i++){
        new_pixel_count[i] = (int)(sum_probability[i] * 255 + 0.5);//四舍五入
    }

    //修改原图像中的像素值
    for(int i = 0;i < bit_size;i++){
        srcImg.imgData[i] = new_pixel_count[srcImg.imgData[i]];//通过数组下标实现像素值转换
    }
    finish = clock();
    printf("直方图均衡化花费%lf秒\n",(double)(finish-start)/CLOCKS_PER_SEC);

    //将修改后的像素值写到新图片中
    fwrite(&(srcImg.bitMapFileHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapFileHeaders),dst);
    fwrite(&(srcImg.bitMapInfoHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapInfoHeaders),dst);
    fwrite(srcImg.palettes,sizeof(palette),256,dst);
    fwrite(srcImg.imgData,sizeof(uint8_t),bit_size,dst);

    fclose(dst);
    fclose(src);
}

void bgrCopy(BGR* bgr,BGR* bgr1){

    bgr1->blue = bgr->blue;
    bgr1->green = bgr->green;
    bgr1->red = bgr->red;
}


void histImageColor(char* fileName,char * output){
    FILE* src = fopen(fileName,"rb");
    FILE* dst = fopen(output,"wb+"); 
    BMP1 srcImg;
    fread(&(srcImg.bitMapFileHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapFileHeaders),src);
    fread(&(srcImg.bitMapInfoHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapInfoHeaders),src);

    int width = srcImg.bitMapInfoHeaders.biWidth;
    int height = srcImg.bitMapInfoHeaders.biHeight;
    int pixel_bit = srcImg.bitMapInfoHeaders.biBitCount;
    int bit_size = height*width;
    int offbit = srcImg.bitMapFileHeaders.bfOffBits;
    printf("offbit=%d\n",pixel_bit);
    int pixel_count[3][256] = {0};
    srcImg.imgData = (uint8_t*)malloc(pixel_bit/8*bit_size);

    fseek(src,offbit,SEEK_SET);

    fread(srcImg.imgData,pixel_bit/8,bit_size,src);
    clock_t start,finish;
    start = clock();
    //统计每个像素值的个数，用pixel_count记录


    for(int i = 0;i<bit_size*3;i+=3){
        pixel_count[0][srcImg.imgData[i+0]]++;
        pixel_count[1][srcImg.imgData[i+1]]++;
        pixel_count[2][srcImg.imgData[i+2]]++;
 
    }
    //统计每个像素值的概率，用probability记录
    double probability[3][256] = {0};
    for(int i = 0;i<256;i++){
        probability[0][i] = (pixel_count[0][i] * 1.0) / bit_size;
        probability[1][i] = (pixel_count[1][i] * 1.0) / bit_size;
        probability[2][i] = (pixel_count[2][i] * 1.0) / bit_size;
    }
    //统计每个像素值的累计概率，用sum_probability记录
    double sum_probability[3][256]= {0};
    for(int i = 0;i<256;i++){


        if(i !=0){
            sum_probability[0][i] += sum_probability[0][i-1] + probability[0][i];
            sum_probability[1][i] += sum_probability[1][i-1] + probability[1][i];
            sum_probability[2][i] += sum_probability[2][i-1] + probability[2][i];
        }
        else{
            sum_probability[0][i] = probability[0][i];
            sum_probability[1][i] = probability[1][i];
            sum_probability[2][i] = probability[2][i];
        }
            
    }
    //根据累计比例调整灰度值,新灰度值用new_pixel_count记录
    uint8_t new_pixel_count[3][256]={0};
    for(int i = 0;i<256;i++){
        new_pixel_count[0][i] = (int)(sum_probability[0][i] * 255 + 0.5);//四舍五入
        new_pixel_count[1][i] = (int)(sum_probability[1][i] * 255 + 0.5);//四舍五入
        new_pixel_count[2][i] = (int)(sum_probability[2][i] * 255 + 0.5);//四舍五入
    }

    //修改原图像中的像素值
    for(int i = 0;i < bit_size*3;i+=3){
        srcImg.imgData[i+0] = new_pixel_count[0][srcImg.imgData[i+0]];//通过数组下标实现像素值转换
        srcImg.imgData[i+1] = new_pixel_count[1][srcImg.imgData[i+1]];
        srcImg.imgData[i+2] = new_pixel_count[2][srcImg.imgData[i+2]];
    }
    finish = clock();
    printf("直方图均衡化花费%lf秒\n",(double)(finish-start)/CLOCKS_PER_SEC);

    //将修改后的像素值写到新图片中
    fwrite(&(srcImg.bitMapFileHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapFileHeaders),dst);
    fwrite(&(srcImg.bitMapInfoHeaders),sizeof(uint8_t),sizeof(srcImg.bitMapInfoHeaders),dst);

    fwrite(srcImg.imgData,sizeof(uint8_t),bit_size*3,dst);

    fclose(dst);
    fclose(src);
}