#ifndef GOODSARR_H
#define GOODSARR_H
#include <QTextStream>
#include <cassert>
class GoodsArr
{
public:
    explicit GoodsArr();
    GoodsArr(const QString &str);

    unsigned& operator[](int i)
    {
	assert (i<8);
	return arr[i];
    }
    const unsigned& operator[](int i) const
    {
	assert (i<8);
	return arr[i];
    }
    bool empty() const
    {
	unsigned sum=0;
	for(int i=0; i<8; i++){
	    sum+=arr[i];
	}
	return !sum;
    }
    void set(unsigned d)
    {
	for(int i=0; i<8; i++)
	{
	    arr[i]=d;
	}
    }

    GoodsArr min(const GoodsArr& other) const
    {
	GoodsArr result;
	for(int i=0; i<8; i++){
	    result[i]=std::min(arr[i],other.arr[i]);
	}
	return result;
    }
    GoodsArr min(const GoodsArr& other, const GoodsArr& sieve) const
    {
	GoodsArr result;
	for(int i=0; i<8; i++){
	    if(sieve.arr[i])
	    {
		result[i]=std::min(arr[i],other.arr[i]);
	    }
	    else {
		result[i]=arr[i];
	    }
	}
	return result;
    }
    GoodsArr max(const GoodsArr& other) const
    {
	GoodsArr result;
	for(int i=0; i<8; i++){
	    result[i]=std::max(arr[i],other.arr[i]);
	}
	return result;
    }

private:
    unsigned arr[8];
};

#endif // GOODSARR_H
