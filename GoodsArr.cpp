#include "GoodsArr.h"
GoodsArr::GoodsArr()
{
    memset (arr,0,sizeof(arr));
}

GoodsArr::GoodsArr(QString &str)
{
    QTextStream a(&str);
    char c;
    a>>arr[0];
    for(int i=1; i<8; i++)
    {
        a>>c>>arr[i];
    }
}
