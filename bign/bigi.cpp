#include "bigi.h"
#include <string.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))
void bigi::_init() {
	uint head;
	if (data[p_size - 1] & SIGN_MARK) {
		head = NEGTIVE_HEAD;
	} else {
		head = POSITIVE_HEAD;
	}
	int validpos = p_size - 1;
	while (validpos > 0 && data[validpos] == head)
		validpos--;
	if ((SIGN_MARK & data[validpos]) == (SIGN_MARK & head)) {
		l_size = validpos + 1;
	} else {
		l_size = validpos + 2;
	}
}
void bigi::_init_new(uint * _data, int n) {
	uint head;
	int act_size;
	if (_data[n - 1] & SIGN_MARK) {
		head = NEGTIVE_HEAD;
	} else {
		head = POSITIVE_HEAD;
	}
	int validpos = n - 1;
	while (validpos > 0 && _data[validpos] == head)
		validpos--;
	if ((SIGN_MARK & _data[validpos]) == (SIGN_MARK & head)) {
		act_size = validpos + 1;
	} else {
		act_size = validpos + 2;
	}
	data = new uint[act_size];
	l_size = p_size = act_size;
	memcpy(data, _data, act_size * UINT_BYTES);
}
bigi::bigi(uint *_data, int n) {
	_init_new(_data, n);
}
bigi::bigi(const char * c) {
	bigi r = 0;
	bool is_negative = *c == '-' ? true : false;
	if (*c == '-' || *c == '+') {
		c++;
	}
	for (;;) {
		if (*c > '9' || *c < '0')
			break;
		r = r * 10 + int (*c - '0');
		c++;
	}
	if (is_negative)
		r = -r;
	_init_new(r.data, r.l_size);
}
bigi::bigi(uint *_data, int n, bool forcePositive) {
	if (forcePositive) {
		data = new uint[n + 1];
		memcpy(data, _data, n * UINT_BYTES);
		data[n] = 0;
		p_size = n + 1;
		_init();
	} else {
		_init_new(_data, n);
	}
}
bigi::bigi(int _d) {
	int i_bytes = sizeof(int);
	int bigi_units = i_bytes / UINT_BYTES;
	uint * tmp = new uint[bigi_units];
	memcpy(tmp, &_d, i_bytes);
	data = tmp;
	p_size = bigi_units;
	_init();
}

bigi::bigi() {
	data = new uint[1];
	data[0] = 0;
	p_size = 1;
	_init();
}

bigi::bigi(const bigi & _v) {
	_init_new(_v.data, _v.l_size);
}
void bigi::operator =(const bigi & _v) {
	delete[] data;
	_init_new(_v.data, _v.l_size);
}
bigi::~bigi() {
	delete[] data;
}
uint bigi::value(int i) const {
	if (i < 0)
		return 0;
	else if (i < l_size)
		return data[i];
	else if (data[l_size - 1] & SIGN_MARK)
		return NEGTIVE_HEAD;
	else
		return POSITIVE_HEAD;
}
bigi bigi::operator +(const bigi& b) const {
	int rs_size = max(l_size,b.l_size) + 1;
	uint * tmp = new uint[rs_size];
	memset(tmp, 0, rs_size * UINT_BYTES);
	uint cur_o = 0;
	for (int i = 0; i < rs_size; i++) {
		ulong tlong = (ulong) value(i) + b.value(i) + cur_o;
		tmp[i] = (uint) tlong;
		cur_o = (uint) (tlong >> UINT_BITS);
	}
	bigi rs = bigi(tmp, rs_size);
	delete[] tmp;
	return rs;
}
bigi bigi::operator ~() const {
	uint * tmp = new uint[l_size];
	for (int i = 0; i < l_size; i++) {
		tmp[i] = ~data[i];
	}
	bigi rs(tmp, l_size);
	delete[] tmp;
	return rs;
}
bigi bigi::operator -() const {
	return (~(*this)) + bigi(1);
}
bigi bigi::operator -(const bigi& b) const {
	return (*this) + (-b);
}
bigi bigi::operator *(const bigi& b) const {
	bigi lf, rt;
	int _sign = this->sign() * b.sign();
	if (_sign == 0)
		return bigi(0);
	lf = this->sign() > 0 ? *this : -(*this);
	rt = b.sign() > 0 ? b : -b;
	int rs_size = lf.l_size + rt.l_size;
	uint * rs_tmp = new uint[rs_size];
	memset(rs_tmp, 0, rs_size * UINT_BYTES);
	for (int i = 0; i < lf.l_size; i++) {
		for (int j = 0; j < rt.l_size; j++) {
			ulong reg = lf.data[i] * (ulong) rt.data[j];
			uint multi_low = (uint) (reg & NEGTIVE_HEAD);
			uint multi_high = (uint) (reg >> UINT_BITS & NEGTIVE_HEAD);

			int pos = i + j;
			reg = rs_tmp[pos] + (ulong) multi_low;
			rs_tmp[pos] = (uint) (reg & NEGTIVE_HEAD);
			pos++;
			reg = rs_tmp[pos] + (reg >> UINT_BITS & NEGTIVE_HEAD) + multi_high;
			rs_tmp[pos] = (uint) (reg & NEGTIVE_HEAD);
			while (reg >> UINT_BITS & NEGTIVE_HEAD) {
				pos++;
				reg = rs_tmp[pos] + (reg >> UINT_BITS & NEGTIVE_HEAD);
				rs_tmp[pos] = (uint) (reg & NEGTIVE_HEAD);
			}
		}
	}
	bigi rs = bigi(rs_tmp, rs_size);
	delete[] rs_tmp;
	return _sign > 0 ? rs : -rs;
}
bigi bigi::operator /(const bigi& b) const {
	bigi q;
	divide(*this, b, &q, 0);
	return q;
}
bigi bigi::operator %(const bigi& b) const {
	bigi r;
	divide(*this, b, 0, &r);
	return r;
}
bigi bigi::pow(const bigi &n) const {
	if (n < 0)
		return 0;
	else if (n == 0)
		return 1;
	else if (n == 1)
		return *this;
	else {
		bigi q, r;
		divide(n, 2, &q, &r);
		bigi t = pow(q);
		if (r == 1) {
			return t * t * (*this);
		} else {
			return t * t;
		}
	}
}
void bigi::divide(const bigi& _n, const bigi& _m, bigi* q, bigi*r) {
	int sn = _n.sign(), sm = _m.sign();
	bigi _q, _r;
	if (sm == 0) {
		throw std::logic_error("divide 0 error");
	}
	bigi n = sn < 0 ? -_n : _n;
	bigi m = sm < 0 ? -_m : _m;
	if (n < m) {
		_q = bigi(0);
		_r = bigi(n);
	} else {
		uint * qtmp = new uint[n.l_size];
		bigi rr(0);
		for (int i = n.l_size - 1; i >= 0; i--) {
			bigi nn = (rr << UINT_BITS) + n.value(i);
			if (nn < m) {
				qtmp[i] = 0;
				rr = nn;
			} else {
				ulong lf = 1, rt = NEGTIVE_HEAD;
				while (lf < rt) {
					ulong mid = (lf + rt + 1) / 2;
					bigi qq = m * mid;
					rr = nn - qq;
					if (rr.sign() == 0) {
						lf = rt = mid;
					} else if (rr.sign() > 0) {
						lf = mid;
					} else {
						rt = mid - 1;
					}
				}
				qtmp[i] = (uint) (lf & NEGTIVE_HEAD);
				rr = nn - bigi(lf) * m;
			}
		}
		_q = bigi(qtmp, n.l_size, true);
		delete[] qtmp;
		_r = rr;
	}
	_q = sm * sn >= 0 ? _q : -_q;
	_r = sn >= 0 ? _r : -_r;
	if (q != 0)
		*q = _q;
	if (r != 0)
		*r = _r;
}
bigi bigi::operator <<(int n) const {
	int d_len = (n - 1) / UINT_BITS + 1;
	uint * tmp = new uint[l_size + d_len];
	int r = n % UINT_BITS;
	int q = n / UINT_BITS;
	for (int i = 0; i < l_size + d_len; i++) {
		tmp[i] = (value(i - q) << r) | (value(i - q - 1) >> (UINT_BITS - r));
	}
	bigi rs(tmp, l_size + d_len);
	delete[] tmp;
	return rs;
}
bigi bigi::operator >>(int n) const {
	uint * tmp = new uint[l_size];
	int r = n % UINT_BITS;
	int q = n / UINT_BITS;
	for (int i = 0; i < l_size; i++) {
		tmp[i] = (value(i + q) >> r) | (value(i + q + 1) << (UINT_BITS - r));
	}
	bigi rs(tmp, l_size);
	delete[] tmp;
	return rs;
}
bool bigi::operator ==(const bigi& b) const {
	bigi c = *this - b;
	return c.sign() == 0;
}
bool bigi::operator <(const bigi& b) const {
	bigi c = *this - b;
	return c.sign() < 0;
}
bool bigi::operator <=(const bigi& b) const {
	return *this < b || *this == b;
}
bool bigi::operator >(const bigi& b) const {
	return !(*this == b || *this < b);
}
bool bigi::operator >=(const bigi& b) const {
	return !(*this < b);
}
bool bigi::operator !=(const bigi& b) const {
	return !(*this == b);
}
int bigi::sign() const {
	if (data[l_size - 1] & SIGN_MARK)
		return -1;
	else if (l_size == 1 && data[0] == 0)
		return 0;
	return 1;
}
int bigi::intValue() const {
	int a = 0;
	int int_bytes = sizeof(int);
	int l_bytes = l_size * UINT_BYTES;
	memcpy(&a, data, min(int_bytes, l_bytes));
	return a;
}
std::ostream& operator <<(std::ostream& _out, const bigi& b) {
	std::_Ios_Fmtflags flag = _out.flags() & std::ios::basefield;
	if (flag & std::ios::hex) {
		std::stringstream o;
		o.setf(std::ios::hex, std::ios::basefield);
		for (int i = b.l_size - 1; i >= 0; i--) {
			o << std::setw(UINT_BYTES * 2) << std::setfill('0') << b.data[i];
		}
		std::string s;
		o >> s;
		_out << s;
	} else if (flag & std::ios::dec) {
		std::stringstream o;
		int _sign = b.sign();
		bigi d = _sign >= 0 ? b : -b, q = 0, r = 0;
		while (d > 0) {
			bigi::divide(d, 10, &q, &r);
			o << (r.intValue() % 10);
			d = q;
		}
		std::string s;
		o >> s;
		if (_sign < 0) {
			_out << '-';
		}
		bool ignore = true;
		for (std::string::reverse_iterator citer = s.rbegin();
				citer != s.rend(); ++citer) {
			if (*citer != '0') {
				ignore = false;
			}
			if (!ignore) {
				_out << *citer;
			}
		}
		if (ignore)
			_out << '0';
	} else if (flag & std::ios::oct) {
	} else {

	}
	return _out;
}
std::istream& operator >>(std::istream& in, bigi& b) {
	bigi r(0);
	std::_Ios_Fmtflags _old = in.setf(std::ios::skipws,std::ios::skipws);
	char ch;
	in>>ch;
	in.setf(std::ios::skipws, _old);
	if (!in.eof()) {
		bool is_negative = ch == '-' ? true : false;
		if (ch == '-' || ch == '+') {
		} else {
			in.putback(ch);
		}
		for (;;) {
			ch=in.get();
			if (in.eof() || ch > '9' || ch < '0')
				break;
			r = r * 10 + int(ch - '0');
		}
		if (is_negative)
			r = -r;
		b=r;
		if(!in.eof()){
			in.putback(ch);
		}
	}
	return in;
}

bool bigi::isEven() const {
	return (data[0] & 0x01) == 0;
}
bigi bigi::gcd(const bigi&a, const bigi&b) {
	if (a < 0 || b < 0)
		throw std::logic_error("gcd algorithm only for non-negative numbers");
	else if (a == 0)
		return b;
	else if (b == 0)
		return a;
	else if (a.isEven() && b.isEven())
		return gcd(a >> 1, b >> 1) << 1;
	else if (a.isEven() && !b.isEven())
		return gcd(a >> 1, b);
	else if (b.isEven() && !a.isEven())
		return gcd(a, b >> 1);
	else {
		return gcd(a < b ? b - a : a - b, min(a, b));
	}
}
bigi bigi::lcm(const bigi&a, const bigi&b) {
	if (a < 0 || b < 0)
		throw std::logic_error("lcm algorithm only for non-negative numbers");
	return a * b / gcd(a, b);
}
bigi bigi::abs(const bigi&a) {
	return a >= 0 ? a : -a;
}
