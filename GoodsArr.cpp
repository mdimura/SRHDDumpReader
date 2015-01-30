#include "GoodsArr.h"
#include <QVector>
GoodsArr::GoodsArr()
{
	memset (arr,0,sizeof(arr));
}

GoodsArr::GoodsArr(const QString &str)
{
	int i=0;
	for(const QStringRef& sref : str.splitRef(','))
	{
		arr[i++]=sref.toInt();
	}
}
