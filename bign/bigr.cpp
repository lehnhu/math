#include "bigr.h"
#include <sstream>
#include <iostream>

void bigr::_init(const bigi& integer, const bigi& numerator,
		const bigi&denominator) {
	if (denominator <= 0) {
		throw std::logic_error(
				"bigr init error:denominator must greater than 0");
	} else {
		int dec_sign = numerator.sign();
		bigi _integer = integer, _numerator = bigi::abs(numerator),
				_denominator = denominator;
		if (_numerator == 0) {
			_denominator = 1;
		} else {
			//_numerator > 0
			bigi _gcd = bigi::gcd(_numerator, _denominator);
			if (_gcd > 1) {
				_numerator = _numerator / _gcd;
				_denominator = _denominator / _gcd;
			}
			bigi q(0), r(_numerator);
			if (_numerator >= _denominator) {
				bigi::divide(_numerator, _denominator, &q, &r);
			}
			if (dec_sign < 0) {
				q = -q;
				r = -r;
			}
			if (r < 0) {
				r = r + _denominator;
				q = q - 1;
			}

			_numerator = r;
			_integer = _integer + q;

		}
		this->denominator = _denominator;
		this->integer = _integer;
		this->numerator = _numerator;
	}
}
int bigr::sign() {
	if (integer == 0)
		return numerator.sign();
	return integer.sign();
}
bigr::bigr(const bigi& integer, const bigi& numerator,
		const bigi&denominatorn) {
	_init(integer, numerator, denominatorn);
}
bigr::bigr(const bigi& integer) {
	_init(integer, 0, 1);
}
bigr::bigr(const bigi& numerator, const bigi&denominator) {
	_init(0, numerator, denominator);
}
bigr::bigr(int intvalue) {
	_init(intvalue, 0, 1);
}
bigr::bigr() {
	_init(0, 0, 1);
}
bigr::bigr(const char * c) {
	bigr r = 0;
	bool is_negative = *c == '-' ? true : false;
	if (*c == '-' || *c == '+') {
		c++;
	}
	for (;;) {
		if (*c > '9' || *c < '0')
			break;
		r = r * 10 + int(*c - '0');
		c++;
	}
	bigr f(1);
	if (*c == '.') {
		c++;
		for (;;) {
			if (*c > '9' || *c < '0')
				break;
			f = f / 10;
			r = r + f * int(*c - '0');
			c++;
		}
	}
	if (is_negative)
		r = -r;
	_init(r.integer, r.numerator, r.denominator);
}
std::ostream& operator <<(std::ostream & out, const bigr& r) {
	std::stringstream o;
	if (r.integer != 0) {
		o << r.integer;
		if (r.numerator != 0) {
			o << "+";
		}
	}
	if (r.numerator != 0)
		o << r.numerator << "/" << r.denominator;
	else if (r.integer == 0)
		o << "0";
	std::string s;
	o >> s;
	out << s;
	return out;
}
bigr bigr::operator +(const bigr&b) const {
	bigi _integer = integer + b.integer;
	bigi _denominator = bigi::lcm(denominator, b.denominator);
	bigi _numerator = (_denominator / denominator) * numerator
			+ (_denominator / b.denominator) * b.numerator;
	return bigr(_integer, _numerator, _denominator);
}
bigr bigr::operator -() const {
	return bigr(-integer, -numerator, denominator);
}
bigr bigr::operator -(const bigr&b) const {
	return *this + (-b);
}
bigr bigr::operator *(const bigr&b) const {
	bigi _denominator = denominator * b.denominator;
	bigi _numerator = (numerator + denominator * integer)
			* (b.numerator + b.denominator * b.integer);
	return bigr(0, _numerator, _denominator);
}
bigr bigr::operator /(const bigr&b) const {
	bigi _numerator = (numerator + denominator * integer) * b.denominator;
	bigi _denominator = denominator * (b.numerator + b.denominator * b.integer);
	return bigr(0, _numerator, _denominator);
}
std::string bigr::toDecString(int scale) const {
	std::stringstream out;
	bigr tmp = *this;
	int i;
	for (i = 0; i < scale; i++) {
		tmp = tmp * 10;
	}
	bigi inte = tmp.integer;
	if (tmp.denominator <= tmp.numerator * 2)
		inte = inte + 1;
	if (inte.sign() < 0)
		out << "-";
	inte = bigi::abs(inte);
	bigi q, r;
	char *c = new char[scale];
	for (i = 0; i < scale; i++) {
		bigi::divide(inte, 10, &q, &r);
		c[i] = r.intValue() + '0';
		inte = q;
	}
	for (i = 0; i < scale && c[i] == '0'; i++)
		;
	out << inte;
	if (scale > i)
		out << '.';
	for (int j = scale - 1; j >= i; j--)
		out << c[j];
	delete[] c;
	std::string s;
	out >> s;
	return s;
}

bool bigr::operator ==(const bigr& a) const {
	return integer == a.integer && denominator == a.denominator
			&& numerator == a.numerator;
}
bool bigr::operator <(const bigr& a) const {
	bigr c = *this - a;
	return c.sign() < 0;
}
bool bigr::operator <=(const bigr& a) const {
	return !(*this > a);
}
bool bigr::operator >(const bigr& a) const {
	bigr c = *this - a;
	return c.sign() > 0;
}
bool bigr::operator >=(const bigr& a) const {
	return !(*this < a);
}
bool bigr::operator !=(const bigr& a) const {
	return !(*this == a);
}
bigr bigr::round(int scale) const {
	if (scale < 0)
		return *this;
	bigi new_den = bigi(10).pow(scale);
	bigi p = new_den * numerator;
	bigi q, r;
	bigi::divide(p, denominator, &q, &r);
	if (denominator <= r * 2) {
		q = q + 1;
	}
	return bigr(integer, q, new_den);
}
bigr bigr::abs(const bigr& a) {
	return a >= 0 ? a : -a;
}

bigr bigr::pow(const bigi& n, int scale) const {
	if (*this == 0 && n <= 0)
		throw std::logic_error("0^n n must greater than 0");
	else if (n < 0)
		return 0;
	else if (n == 0)
		return 1;
	else if (n == 1)
		return *this;
	else {
		bigr t = bigr::abs(*this);
		if (n.isEven()) {
			t = t.pow(n >> 1, scale);
			return t * t;
		} else {
			t = t.pow(n >> 1, scale);
			return t * t * (*this);
		}
	}
}
std::istream& operator >>(std::istream& in, bigr& b) {
	bigr r(0);
	std::_Ios_Fmtflags _old = in.setf(std::ios::skipws, std::ios::skipws);
	char ch;
	in >> ch;
	in.setf(std::ios::skipws, _old);
	if (!in.eof()) {
		bool is_negative = ch == '-' ? true : false;
		if (ch == '-' || ch == '+') {
		} else {
			in.putback(ch);
		}
		for (;;) {
			ch = in.get();
			if (in.eof() || ch > '9' || ch < '0')
				break;

			r = r * 10 + int(ch - '0');
		}
		bigr f(1);
		if (ch == '.') {
			ch = in.get();
			for (;;) {
				if (ch > '9' || ch < '0')
					break;

				f = f / 10;
				r = r + f * int(ch - '0');
				ch = in.get();
			}
		}
		if (is_negative)
			r = -r;

		b = r;
		if (!in.eof()) {
			in.putback(ch);
		}
	}
	return in;
}
