#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

void toBeOne(Mat& input, Mat& output, int index = 0)
{
    float max = 0, min = 0;
    output = Mat::zeros(input.rows, input.cols, CV_8UC3);
    for (int i = 0; i < input.rows; i++)
    {
        float* ptr = input.ptr<float>(i);
        for (int j = 0; j < input.cols * 3; j++)//图像为3通道
        {
            if (max < ptr[j])
                max = ptr[j];
            if (min > ptr[j])
                min = ptr[j];
        }

    }
    //取出图像中的上下限
    for (int i = 0; i < input.rows; i++)
    {
        float* ptr = input.ptr<float>(i);
        uchar* optr = output.ptr<uchar>(i);
        for (int j = 0; j < input.cols * 3; j++)
        {
            if (index) {
                if (ptr[j] > 1) {
                    optr[j] = 255;
                    continue;
                }
                else if (ptr[j] < 0) {
                    optr[j] = 0;
                    continue;
                }
                else
                    optr[j] = (uchar)(ptr[j] * 255);
            }
            else
                optr[j] = (uchar)((ptr[j] - min) / (max - min) * 255);
        }
    }
}

//γ变换
cv::Mat gammaTran(const cv::Mat src, double gamma, double comp)
{
    cv::Mat dst(src);
    int M = 0;
    int N = 0;
    if (src.empty()) {
        std::cout << "Src pic is empty" << std::endl;
        return src;
    }
    M = src.rows;
    N = src.cols * src.channels();
    for (int i = 0; i < M; i++) {
        const float* p1 = src.ptr<const float>(i);
        float* p2 = dst.ptr<float>(i);
        for (int j = 0; j < N; j++) {
            p2[j] = pow(p1[j], gamma) * comp;
        }
    }
    return dst;
}

int main()
{
    Mat iinput = imread("F:/学习/计算机/数字图像处理/skeleton.png"), input, Tlaplas;
    imshow("original", iinput);
    iinput.convertTo(input, CV_32F, 1.0 / 255, 0);//把图片转化为float类型，这样子可以直接进行加减而不会溢出

    //Laplacian变换
    Mat kern = (Mat_<char>(3, 3) << 1, 1, 1,       //滤波器
        1, -8, 1,
        1, 1, 1);
    Mat laplas;
    Mat output;
    filter2D(input, laplas, input.depth(), kern);//使用滤波器kern对input进行相关操作，结果存储在laplas中
    toBeOne(laplas, Tlaplas);
    imshow("Tlaplas", Tlaplas);
    output = input - laplas;//如果中间的值是正的则是加号，负值则是减号
    toBeOne(output, iinput, 1);
    Mat R0 = iinput;
    imshow("laplace", R0);

    //Sobel梯度
    Mat kern2 = (Mat_<char>(3, 3) << -1, -2, -1,
        0, 0, 0,
        1, 2, 1);
    Mat kern3 = (Mat_<char>(3, 3) << -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1);
    Mat gx, gy;
    filter2D(input, gx, input.depth(), kern2);
    filter2D(input, gy, input.depth(), kern3);
    Mat Soutput = abs(gx) + abs(gy);
    toBeOne(Soutput, iinput, 1);
    Mat R1 = iinput;
    imshow("Sobel1", R1);

    //盒滤波器平滑
    Mat smooth;
    blur(iinput, smooth, Size(5, 5));
    Mat R2 = smooth;
    imshow("Smooth", R2);

    //相乘
    Mat R3, r3;
    R0.convertTo(R0, CV_32F, 1.0 / 255, 0);
    R2.convertTo(R2, CV_32F, 1.0 / 255, 0);
    multiply(R0, R2, r3);
    toBeOne(r3, R3, 1);
    imshow("Result0", R3);

    //锐化增强
    cv::Mat r4, R4;
    r4 = input + r3;
    toBeOne(r4, R4, 1);
    imshow("Result1", R4);

    //γ变换
    cv::Mat r5, R5;
    r5 = gammaTran(r4, 0.5, 1);
    toBeOne(r5, R5, 1);
    imshow("Result", R5);

    waitKey();
    return 0;
}