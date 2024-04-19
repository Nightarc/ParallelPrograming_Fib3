// ParallelPrograming_Fib3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <execution>
#include <random>

using namespace std;
const int base = 1000000000;
const int base_digits = 9;
struct bigint {
	vector<int> a;
	int sign;
	/*<arpa>*/
	int size() {
		if (a.empty())return 0;
		int ans = (a.size() - 1) * base_digits;
		int ca = a.back();
		while (ca)
			ans++, ca /= 10;
		return ans;
	}
	bigint operator ^(const bigint& v) {
		bigint ans = 1, a = *this, b = v;
		while (!b.isZero()) {
			if (b % 2)
				ans *= a;
			a *= a, b /= 2;
		}
		return ans;
	}
	string to_string() {
		stringstream ss;
		ss << *this;
		string s;
		ss >> s;
		return s;
	}
	int sumof() {
		string s = to_string();
		int ans = 0;
		for (auto c : s)  ans += c - '0';
		return ans;
	}
	/*</arpa>*/
	bigint() :
		sign(1) {
	}

	bigint(long long v) {
		*this = v;
	}

	bigint(const string& s) {
		read(s);
	}

	void operator=(const bigint& v) {
		sign = v.sign;
		a = v.a;
	}

	void operator=(long long v) {
		sign = 1;
		a.clear();
		if (v < 0)
			sign = -1, v = -v;
		for (; v > 0; v = v / base)
			a.push_back(v % base);
	}

	bigint operator+(const bigint& v) const {
		if (sign == v.sign) {
			bigint res = v;

			for (int i = 0, carry = 0; i < (int)max(a.size(), v.a.size()) || carry; ++i) {
				if (i == (int)res.a.size())
					res.a.push_back(0);
				res.a[i] += carry + (i < (int)a.size() ? a[i] : 0);
				carry = res.a[i] >= base;
				if (carry)
					res.a[i] -= base;
			}
			return res;
		}
		return *this - (-v);
	}

	bigint operator-(const bigint& v) const {
		if (sign == v.sign) {
			if (abs() >= v.abs()) {
				bigint res = *this;
				for (int i = 0, carry = 0; i < (int)v.a.size() || carry; ++i) {
					res.a[i] -= carry + (i < (int)v.a.size() ? v.a[i] : 0);
					carry = res.a[i] < 0;
					if (carry)
						res.a[i] += base;
				}
				res.trim();
				return res;
			}
			return -(v - *this);
		}
		return *this + (-v);
	}

	void operator*=(int v) {
		if (v < 0)
			sign = -sign, v = -v;
		for (int i = 0, carry = 0; i < (int)a.size() || carry; ++i) {
			if (i == (int)a.size())
				a.push_back(0);
			long long cur = a[i] * (long long)v + carry;
			carry = (int)(cur / base);
			a[i] = (int)(cur % base);
			//asm("divl %%ecx" : "=a"(carry), "=d"(a[i]) : "A"(cur), "c"(base));
		}
		trim();
	}

	bigint operator*(int v) const {
		bigint res = *this;
		res *= v;
		return res;
	}

	void operator*=(long long v) {
		if (v < 0)
			sign = -sign, v = -v;
		if (v > base) {
			*this = *this * (v / base) * base + *this * (v % base);
			return;
		}
		for (int i = 0, carry = 0; i < (int)a.size() || carry; ++i) {
			if (i == (int)a.size())
				a.push_back(0);
			long long cur = a[i] * (long long)v + carry;
			carry = (int)(cur / base);
			a[i] = (int)(cur % base);
			//asm("divl %%ecx" : "=a"(carry), "=d"(a[i]) : "A"(cur), "c"(base));
		}
		trim();
	}

	bigint operator*(long long v) const {
		bigint res = *this;
		res *= v;
		return res;
	}

	friend pair<bigint, bigint> divmod(const bigint& a1, const bigint& b1) {
		int norm = base / (b1.a.back() + 1);
		bigint a = a1.abs() * norm;
		bigint b = b1.abs() * norm;
		bigint q, r;
		q.a.resize(a.a.size());

		for (int i = a.a.size() - 1; i >= 0; i--) {
			r *= base;
			r += a.a[i];
			int s1 = r.a.size() <= b.a.size() ? 0 : r.a[b.a.size()];
			int s2 = r.a.size() <= b.a.size() - 1 ? 0 : r.a[b.a.size() - 1];
			int d = ((long long)base * s1 + s2) / b.a.back();
			r -= b * d;
			while (r < 0)
				r += b, --d;
			q.a[i] = d;
		}

		q.sign = a1.sign * b1.sign;
		r.sign = a1.sign;
		q.trim();
		r.trim();
		return make_pair(q, r / norm);
	}

	bigint operator/(const bigint& v) const {
		return divmod(*this, v).first;
	}

	bigint operator%(const bigint& v) const {
		return divmod(*this, v).second;
	}

	void operator/=(int v) {
		if (v < 0)
			sign = -sign, v = -v;
		for (int i = (int)a.size() - 1, rem = 0; i >= 0; --i) {
			long long cur = a[i] + rem * (long long)base;
			a[i] = (int)(cur / v);
			rem = (int)(cur % v);
		}
		trim();
	}

	bigint operator/(int v) const {
		bigint res = *this;
		res /= v;
		return res;
	}

	int operator%(int v) const {
		if (v < 0)
			v = -v;
		int m = 0;
		for (int i = a.size() - 1; i >= 0; --i)
			m = (a[i] + m * (long long)base) % v;
		return m * sign;
	}

	void operator+=(const bigint& v) {
		*this = *this + v;
	}
	void operator-=(const bigint& v) {
		*this = *this - v;
	}
	void operator*=(const bigint& v) {
		*this = *this * v;
	}
	void operator/=(const bigint& v) {
		*this = *this / v;
	}

	bool operator<(const bigint& v) const {
		if (sign != v.sign)
			return sign < v.sign;
		if (a.size() != v.a.size())
			return a.size() * sign < v.a.size() * v.sign;
		for (int i = a.size() - 1; i >= 0; i--)
			if (a[i] != v.a[i])
				return a[i] * sign < v.a[i] * sign;
		return false;
	}

	bool operator>(const bigint& v) const {
		return v < *this;
	}
	bool operator<=(const bigint& v) const {
		return !(v < *this);
	}
	bool operator>=(const bigint& v) const {
		return !(*this < v);
	}
	bool operator==(const bigint& v) const {
		return !(*this < v) && !(v < *this);
	}
	bool operator!=(const bigint& v) const {
		return *this < v || v < *this;
	}

	void trim() {
		while (!a.empty() && !a.back())
			a.pop_back();
		if (a.empty())
			sign = 1;
	}

	bool isZero() const {
		return a.empty() || (a.size() == 1 && !a[0]);
	}

	bigint operator-() const {
		bigint res = *this;
		res.sign = -sign;
		return res;
	}

	bigint abs() const {
		bigint res = *this;
		res.sign *= res.sign;
		return res;
	}

	long long longValue() const {
		long long res = 0;
		for (int i = a.size() - 1; i >= 0; i--)
			res = res * base + a[i];
		return res * sign;
	}

	friend bigint gcd(const bigint& a, const bigint& b) {
		return b.isZero() ? a : gcd(b, a % b);
	}
	friend bigint lcm(const bigint& a, const bigint& b) {
		return a / gcd(a, b) * b;
	}

	void read(const string& s) {
		sign = 1;
		a.clear();
		int pos = 0;
		while (pos < (int)s.size() && (s[pos] == '-' || s[pos] == '+')) {
			if (s[pos] == '-')
				sign = -sign;
			++pos;
		}
		for (int i = s.size() - 1; i >= pos; i -= base_digits) {
			int x = 0;
			for (int j = max(pos, i - base_digits + 1); j <= i; j++)
				x = x * 10 + s[j] - '0';
			a.push_back(x);
		}
		trim();
	}

	friend istream& operator>>(istream& stream, bigint& v) {
		string s;
		stream >> s;
		v.read(s);
		return stream;
	}

	friend ostream& operator<<(ostream& stream, const bigint& v) {
		if (v.sign == -1)
			stream << '-';
		stream << (v.a.empty() ? 0 : v.a.back());
		for (int i = (int)v.a.size() - 2; i >= 0; --i)
			stream << setw(base_digits) << setfill('0') << v.a[i];
		return stream;
	}

	static vector<int> convert_base(const vector<int>& a, int old_digits, int new_digits) {
		vector<long long> p(max(old_digits, new_digits) + 1);
		p[0] = 1;
		for (int i = 1; i < (int)p.size(); i++)
			p[i] = p[i - 1] * 10;
		vector<int> res;
		long long cur = 0;
		int cur_digits = 0;
		for (int i = 0; i < (int)a.size(); i++) {
			cur += a[i] * p[cur_digits];
			cur_digits += old_digits;
			while (cur_digits >= new_digits) {
				res.push_back(int(cur % p[new_digits]));
				cur /= p[new_digits];
				cur_digits -= new_digits;
			}
		}
		res.push_back((int)cur);
		while (!res.empty() && !res.back())
			res.pop_back();
		return res;
	}

	typedef vector<long long> vll;

	static vll karatsubaMultiply(const vll& a, const vll& b) {
		int n = a.size();
		vll res(n + n);
		if (n <= 32) {
			for (int i = 0; i < n; i++)
				for (int j = 0; j < n; j++)
					res[i + j] += a[i] * b[j];
			return res;
		}

		int k = n >> 1;
		vll a1(a.begin(), a.begin() + k);
		vll a2(a.begin() + k, a.end());
		vll b1(b.begin(), b.begin() + k);
		vll b2(b.begin() + k, b.end());

		vll a1b1 = karatsubaMultiply(a1, b1);
		vll a2b2 = karatsubaMultiply(a2, b2);

		for (int i = 0; i < k; i++)
			a2[i] += a1[i];
		for (int i = 0; i < k; i++)
			b2[i] += b1[i];

		vll r = karatsubaMultiply(a2, b2);
		for (int i = 0; i < (int)a1b1.size(); i++)
			r[i] -= a1b1[i];
		for (int i = 0; i < (int)a2b2.size(); i++)
			r[i] -= a2b2[i];

		for (int i = 0; i < (int)r.size(); i++)
			res[i + k] += r[i];
		for (int i = 0; i < (int)a1b1.size(); i++)
			res[i] += a1b1[i];
		for (int i = 0; i < (int)a2b2.size(); i++)
			res[i + n] += a2b2[i];
		return res;
	}

	bigint operator*(const bigint& v) const {
		vector<int> a6 = convert_base(this->a, base_digits, 6);
		vector<int> b6 = convert_base(v.a, base_digits, 6);
		vll a(a6.begin(), a6.end());
		vll b(b6.begin(), b6.end());
		while (a.size() < b.size())
			a.push_back(0);
		while (b.size() < a.size())
			b.push_back(0);
		while (a.size() & (a.size() - 1))
			a.push_back(0), b.push_back(0);
		vll c = karatsubaMultiply(a, b);
		bigint res;
		res.sign = sign * v.sign;
		for (int i = 0, carry = 0; i < (int)c.size(); i++) {
			long long cur = c[i] + carry;
			res.a.push_back((int)(cur % 1000000));
			carry = (int)(cur / 1000000);
		}
		res.a = convert_base(res.a, 6, base_digits);
		res.trim();
		return res;
	}
};

template <typename TFunc> void RunAndMeasure(const char* title, TFunc func)
{
	const auto start = std::chrono::steady_clock::now();
	auto ret = func();
	const auto end = std::chrono::steady_clock::now();
	cout << title << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms, res " << ret << "\n";
}

template <typename TFunc> void RunAndMeasure(const char* title, TFunc func, int execTimes)
{
	double avgTime = 0;
	for (size_t i = 0; i < execTimes; i++)
	{
		const auto start = std::chrono::steady_clock::now();
		auto ret = func();
		const auto end = std::chrono::steady_clock::now();
		avgTime += std::chrono::duration <double, std::milli>(end - start).count();
		cout << title << ", att " << i + 1 << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms, res " << ret << "\n";
	}
	
	cout << "average time: " << avgTime / execTimes << " ms" << endl;
}

// 10e5 - 10e6
// (RAND() % 10e6) + 10e5 
vector<int> createVectorRandom(int n, int leftBorder, int rightBorder) {
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(leftBorder, rightBorder); // define the range

	vector<int> res;
	for (size_t i = 0; i < n; i++)
		res.push_back(distr(gen));

	return res;
}

int findFactors(int n) {
	int c = 0; // 1 and n are always factors, we won't be accounting for them
	if (n % (int)sqrt(n) == 0) c++;
	for (size_t i = 2; i < sqrt(n); i++)
		if (n % i == 0) c += 2;
	return c;
}

template <typename Policy>
int printNumberOfFactors(vector<int> vec, Policy policy) {
	//Применяем функцию нахождения количества делителей к каждому элементу вектора
	transform(policy, vec.begin(), vec.end(), vec.begin(), findFactors);
	return 0;
}



template <typename Policy>
bigint vectorSum(vector<int> vec, Policy policy) {
	//Применяем функцию reduce для нахождения суммы вектора
	return reduce(policy, vec.begin(), vec.end(), (bigint)0);
}

void printVector(vector<int> vec) {
	for (size_t i = 0; i < vec.size(); i++)
		cout << vec[i] << ' ';
	cout << endl;
}

void RunFindFactorTests() {
	srand(time(0));
	const int n1 = 5 * 1e5;
	const int n2 = 1e6;
	const int n3 = 2 * 1e6;
	const int leftBorder = 1e5;
	const int rightBorder = 1e6;
	vector<int> vec1 = createVectorRandom(n1, leftBorder, rightBorder);

	//onelaunch(vec);
	RunAndMeasure("printNumberOfFactors 5 * 10e5 seq", [&vec1] {
		return printNumberOfFactors(vec1, execution::seq);
		});

	RunAndMeasure("printNumberOfFactors 5 * 10e5 par", [&vec1] {
		return printNumberOfFactors(vec1, execution::par);
		});

	RunAndMeasure("printNumberOfFactors 5 * 10e5 par_unseq", [&vec1] {
		return printNumberOfFactors(vec1, execution::par_unseq);
		});

	vector<int> vec2 = createVectorRandom(n2, leftBorder, rightBorder);
	RunAndMeasure("printNumberOfFactors 10e6 seq", [&vec2] {
		return printNumberOfFactors(vec2, execution::seq);
		});

	RunAndMeasure("printNumberOfFactors 10e6 par", [&vec2] {
		return printNumberOfFactors(vec2, execution::par);
		});

	RunAndMeasure("printNumberOfFactors 10e6 par_unseq", [&vec2] {
		return printNumberOfFactors(vec2, execution::par_unseq);
		});


	vector<int> vec3 = createVectorRandom(n3, leftBorder, rightBorder);
	RunAndMeasure("printNumberOfFactors 2* 10e6 seq", [&vec3] {
		return printNumberOfFactors(vec3, execution::seq);
		});

	RunAndMeasure("printNumberOfFactors 2* 10e6 par", [&vec3] {
		return printNumberOfFactors(vec3, execution::par);
		});

	RunAndMeasure("printNumberOfFactors 2* 10e6 par_unseq", [&vec3] {
		return printNumberOfFactors(vec3, execution::par_unseq);
		});
}

void RunSingleTestReduce() {
	int n1 = 2;
	const int leftBorder = 1e5; //100.000
	const int rightBorder = 1e6; //1.000.000
	vector<int> vec1 = createVectorRandom(n1, leftBorder, rightBorder);
	if (n1 < 500)printVector(vec1);
	cout << vectorSum(vec1, execution::seq) << endl;
}
void RunFindReduceTests() {
	srand(time(0));
	const unsigned long int n0 = 1e5;
	const unsigned long int n1 = 5 * 1e8;
	const unsigned long int n2 = 1e9;
	const unsigned long int n3 = 2 * 1e9;
	const unsigned long int leftBorder = 10e5;
	const unsigned long int rightBorder = 10e6;
	vector<int> vec1 = createVectorRandom(n1, leftBorder, rightBorder);

	RunAndMeasure("vectorSum 5 * 10e8 PAR", [&vec1] {
		return vectorSum(vec1, execution::par);
		}, 3);
	RunAndMeasure("vectorSum 5 * 10e8 SEQ", [&vec1] {
		return vectorSum(vec1, execution::seq);
		}, 3);
	RunAndMeasure("vectorSum 5 * 10e8 PAR_UNSEQ", [&vec1] {
		return vectorSum(vec1, execution::par_unseq);
		}, 3);

	vec1.clear();
	vector<int> vec2 = createVectorRandom(n2, leftBorder, rightBorder);
	RunAndMeasure("vectorSum 10e9 SEQ", [&vec2] {
		return vectorSum(vec2, execution::seq);
		}, 3);
	RunAndMeasure("vectorSum 10e9 PAR", [&vec2] {
		return vectorSum(vec2, execution::par);
		}, 3);
	RunAndMeasure("vectorSum 10e9 PAR_UNSEQ", [&vec2] {
		return vectorSum(vec2, execution::par_unseq);
		}, 3);
	vec2.clear();

	vector<int> vec3 = createVectorRandom(n1, leftBorder, rightBorder);
	RunAndMeasure("vectorSum 2 * 10e9 SEQ", [&vec3] {
		return vectorSum(vec3, execution::seq);
		}, 3);
	RunAndMeasure("vectorSum 2 * 10e9 PAR", [&vec3] {
		return vectorSum(vec3, execution::par);
		}, 3);
	RunAndMeasure("vectorSum 2 * 10e9 PAR_UNSEQ", [&vec3] {
		return vectorSum(vec3, execution::par_unseq);
		}, 3);
	vec3.clear();

}
vector<vector<int>> CreateMatrix(int n) {
	vector<vector<int>> M;
	for (size_t i = 0; i < n; i++)
	{
		vector<int> filVec;
		for (size_t i = 0; i < n; i++)
			filVec.push_back(0);
		M.push_back(filVec);
	}
	return M;

}

vector<vector<int>> CreateRandomMatrix(int n, unsigned long int leftBorder, unsigned long int rightBorder) {
	vector<vector<int>> M;
	for (size_t i = 0; i < n; i++)
		M.push_back(createVectorRandom(n, leftBorder, rightBorder));
	return M;
}


int MultiplyStep(const vector<vector<int>> &M1, const vector<vector<int>> &M2, int rowPtr, int colPtr) {
	int sum = 0;
	for (size_t i = 0; i < M1.size(); i++)
		sum += M1[rowPtr][i] * M2[i][colPtr];

	return sum;
}

template <typename Policy>
vector<vector<int>> MultiplyMatrices(Policy policy, const vector<vector<int>> &M1, const vector<vector<int>> &M2) {
	vector<vector<int>> R = CreateMatrix(M1.size());

	for (size_t i = 0; i < M1.size(); i++)
		for (size_t j = 0; j < M1.size(); j++)
			transform(policy, R[i].begin(), R[i].end(), MultiplyStep(M1, M2, i, j));
}

void RunMatricesMultiplicationTest() {
	const unsigned long int leftBorder = 10;
	const unsigned long int rightBorder = 20;

	int n = 4;

	vector<vector<int>> M1 = CreateRandomMatrix(n, leftBorder, rightBorder);
	vector<vector<int>> M2 = CreateRandomMatrix(n, leftBorder, rightBorder);
	vector<vector<int>> R = MultiplyMatrices(execution::par, M1, M2);

	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
			cout << R[i][j] << ' ';
		cout << endl;
	}
}

int main() {
	RunMatricesMultiplicationTest();
	return 0;
}
