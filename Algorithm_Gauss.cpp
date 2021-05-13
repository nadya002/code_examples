#include <iostream>
#include<vector>
#include<algorithm>
#include<math.h>
#include<numeric>
using i32 = int32_t;
using namespace std;

class Rational {
private:
    i32 num, den;

public:
    Rational(i32 a = 0, i32 b = 1)
    {
        int no = gcd(abs(a), abs(b));
        if (b < 0) {
            b *= -1;
            a *= -1;
        }
        num = a / no;
        den = b / no;
    }
    i32 numerator() const
    {
        return num;
    }
    i32 denominator() const
    {
        return den;
    }
    Rational const operator-() const
    {
        return Rational(-num, den);
    }
    Rational const operator+() const
    {
        return Rational(num, den);
    }
    bool operator==(const Rational& a) const
    {
        return (num == a.num && den == a.den);
    }
    bool operator!=(const Rational& a) const
    {
        return (num != a.num || den != a.den);
    }
    Rational& operator+=(const Rational& a);
    Rational& operator-=(const Rational& a);
    Rational& operator*=(const Rational& a);
    Rational& operator/=(const Rational& a);
    Rational& operator++();
    Rational operator++(int);
    Rational& operator--();
    Rational operator--(int);
};

Rational operator+(const Rational& a, const Rational& b)
{
    return Rational(a.numerator() * b.denominator() + a.denominator() * b.numerator(), a.denominator() * b.denominator());
}

Rational operator-(const Rational& a, const Rational& b)
{
    return Rational(a.numerator() * b.denominator() - a.denominator() * b.numerator(), a.denominator() * b.denominator());
}

Rational operator*(const Rational& a, const Rational& b)
{
    return Rational(a.numerator() * b.numerator(), a.denominator() * b.denominator());
}

Rational operator/(const Rational& a, const Rational& b)
{
    return Rational(a.numerator() * b.denominator(), a.denominator() * b.numerator());
}

Rational& Rational::operator+=(const Rational& a)
{
    return *this = (*this + a);
}

Rational& Rational::operator-=(const Rational& a)
{
    return *this = (*this - a);
}

Rational& Rational::operator*=(const Rational& a)
{
    return *this = ((*this) * a);
}

Rational& Rational::operator/=(const Rational& a)
{
    return *this = ((*this) / a);
}

Rational& Rational::operator++()
{
    *this += 1;
    return *this;
}

Rational& Rational::operator--()
{
    *this -= 1;
    return *this;
}

Rational Rational::operator++(int)
{
    Rational b(this->numerator(), this->denominator());
    *this += 1;
    return b;
}

Rational Rational::operator--(int)
{
    Rational b(this->numerator(), this->denominator());
    *this -= 1;
    return b;
}



std::ostream& operator<<(std::ostream& out, const Rational& ms)
{
    if (ms.denominator() == 1) { 
        out << ms.numerator();
    } else {
        out << "\\frac{" << ms.numerator() << "}{" << ms.denominator() << "}";
    }
    return out;

}
int nod(int a, int b)
{
    if (a == 0 || b == 0) {
        return a + b;
    } else {
        if (a > b) {
            return nod(b, a % b);
        }
        return nod(a, b % a);
    }
}
int cons;

template<typename T>
void pri(vector<vector<T>>& mat)
{
    cout << "\\left(\\begin{array}{";
    for (int i = 0; i < cons; i++) {
        cout << "c";
    }
    if (mat[0].size() != cons) {
        cout << "|";
    }
    for (int i = 0; i < mat[0].size()  - cons; i++) {
        cout << "c";
    }
    cout << "}\n";
    for (int i = 0; i < mat.size(); i++) {
        for (int j = 0; j < mat[0].size(); j++) {
            if (j != 0) {
                cout << " & ";
            }
            cout << mat[i][j];
        }
        cout << "\\\\"
             << "\n";
    }
    cout << "\\end{array}\\right)\n";
}

void gaus(vector<vector<int>>& ar, int n) {
    int i = 0;
    for (int j = 0; j < n; j++) {
        int fl = 0;
        for (int u = i; u < ar.size(); u++) {
            if (ar[u][j] != 0) {
                fl = 1;
                if (u != i) {
                    cout << "\\xrightarrow[]{swap(a_"
                         << i + 1 << ',' << "a_" << u + 1 << ")" << "}\n";
                    swap(ar[u], ar[i]);
                    pri(ar);
                    //break;
                }
                break;
            }
        }
        if (fl == 1) {
            for (int u = i + 1; u < ar.size(); u++) {
                if (ar[u][j] != 0) {
                    int no = nod(abs(ar[i][j]), abs(ar[u][j]));
                    int a = ar[i][j] / no;
                    if (a != 1) {
                        cout << "\\xrightarrow[]{a_"
                             << u + 1 << '=' << a << "a_" << u + 1 << "}\n";
                        for (int k = 0; k < ar[u].size(); k++) {
                            ar[u][k] *= a;
                        }
                        pri(ar);
                    }
                }
            }
            for (int u = i + 1; u < ar.size(); u++) {
                if (ar[u][j] != 0) {
                    int no = nod(abs(ar[i][j]), abs(ar[u][j]));
                    int a = -ar[u][j] / no;
                    if (ar[i][j] < 0) {
                        a *= -1;
                    }
                    cout << "\\xrightarrow[]{a_"
                         << u + 1 << "+=" << a << "a_" << i + 1 << "}\n";
                    for (int k = 0; k < ar[u].size(); k++) {
                        ar[u][k] += a * ar[i][k];
                    }
                    pri(ar);
                }
            }
            i++;
        }
    }
    vector<vector<Rational>> mat(ar.size(), vector<Rational>(ar[0].size()));
    for (int u = 0; u < ar.size(); u++) {
        for (int k = 0; k < ar[0].size(); k++) {
            mat[u][k] = ar[u][k];
        }
    }
    for (i = 0; i < mat.size(); i++) {
        for (int k = 0; k < n; k++) {
            if (ar[i][k] != 0) {
                
                Rational r =  Rational(1, 1) / mat[i][k];
                //cout << "\n" << "\n" << r << "\n" << "\n";
                if (r != 1) {
                    cout << "\\xrightarrow[]{a_"
                         << i + 1 << "=" << r << "a_" << i + 1 << "}\n";
                    for (int t = 0; t < mat[0].size(); t++) {
                        mat[i][t] *= r;
                    }
                    pri(mat);
                }
                break;
            }
        }
    }
    for (i = 1; i < mat.size(); i++) {
        for (int u = 0; u < n; u++) {
            if (mat[i][u] != 0) {
                for (int j = i - 1; j >= 0; j--) {
                    //cout << i << " " << j << endl;
                    if (mat[j][u] != 0) {                      
                        Rational r = mat[j][u];
                        cout << "\\xrightarrow[]{a_"
                             << j + 1 << "+=" << -r << "a_" << i + 1 << "}\n";
                        for (int z = 0; z < mat[i].size(); z++) {
                            mat[j][z] += (-r) * mat[i][z];
                        }
                        pri(mat);
                    }
                }
                break;
            }
        }
    }
}

int main() {
    int n, m, k;
    cin >> n >> m >> k;
    cons = k;
    vector<vector<int>> ar(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> ar[i][j];
        }
    }
    pri(ar);
    gaus(ar, k);
   return 0;
}
