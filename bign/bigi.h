#ifndef __bigi__hul__
#define __bigi__hul__


#include <stdint.h>
#include <ostream>

#define UINT_BYTES (2)
#define UINT_BITS (UINT_BYTES*8)
#define ULONG_BYTES (4)
#define ULONG_BITS (ULONG_BYTES*8)
#define SIGN_MARK 0x8000
#define POSITIVE_HEAD 0x0000
#define NEGTIVE_HEAD 0xffff

typedef uint16_t uint;
typedef uint32_t ulong;

class bigi {
private:
	int l_size;
	int p_size;
	uint * data;
	void _init();
	void _init_new(uint *, int);
public:
	bigi(uint * bytes, int size);
	bigi(uint * bytes, int size, bool forcePositive);
	bigi(int value);
	bigi();
	bigi(const char *c);
	bigi(const bigi&);
	~bigi();
	uint value(int) const;
	bigi operator +(const bigi&) const;
	bigi operator *(const bigi&) const;
	bigi operator /(const bigi&) const;
	bigi operator %(const bigi&) const;
	bigi operator ~() const;
	bigi operator -() const;
	bigi operator <<(int) const;
	bigi operator >>(int) const;
	bigi operator -(const bigi&) const;
	bool operator ==(const bigi&) const;
	bool operator <(const bigi&) const;
	bool operator <=(const bigi&) const;
	bool operator >(const bigi&) const;
	bool operator >=(const bigi&) const;
	bool operator !=(const bigi&) const;
	void operator =(const bigi&);
	int sign() const;
	int intValue() const;
	bigi pow(const bigi &) const;
	static bigi gcd(const bigi&a, const bigi&b);
	static bigi lcm(const bigi&a, const bigi&b);
	bool isEven() const;
	static void divide(const bigi&, const bigi&, bigi*, bigi*);
	static bigi abs(const bigi&);
	friend std::ostream& operator <<(std::ostream &, const bigi&);
	friend std::istream& operator >>(std::istream &, bigi&);
};
#endif
