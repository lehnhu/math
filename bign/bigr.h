#ifndef __bigr__hul__
#define __bigr__hul__

#include "bigi.h"
#include <stdexcept>

class bigr {
private:
	bigi integer;
	bigi numerator;
	bigi denominator;
	void _init(const bigi& integer, const bigi& numerator,
			const bigi&denominator);
public:
	bigr(const bigi& integer, const bigi& numerator, const bigi&denominator);
	bigr(const bigi& integer);
	bigr(const bigi& numerator, const bigi&denominator);
	bigr(const char *);
	bigr(int intvalue);
	bigr();
	bigr operator -() const;
	bigr operator +(const bigr&) const;
	bigr operator -(const bigr&) const;
	bigr operator *(const bigr&) const;
	bigr operator /(const bigr&) const;

	bool operator ==(const bigr&) const;
	bool operator <(const bigr&) const;
	bool operator <=(const bigr&) const;
	bool operator >(const bigr&) const;
	bool operator >=(const bigr&) const;
	bool operator !=(const bigr&) const;

	int sign();
	std::string toString(int dec) const;
	bigr round(int dec) const;
	friend std::ostream& operator <<(std::ostream &, const bigr&);
	friend std::istream& operator >>(std::istream &, bigr&);
};
#endif
