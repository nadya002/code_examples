#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <utility>
#include <vector>
using i32 = int32_t;

template <typename T>
class Polynomial {
private:
    std::map<i32, T> coef;
    void erase_zero() {
        std::vector<i32> er;
        for (auto i : coef) {
            if (i.second == T(0)) {
                er.push_back(i.first);
            }
        }
        for (auto i : er) {
            coef.erase(i);
        }
    }
    template<typename P>
    static P deg(P a, i32 b) {
        if (b == 0) {
            return P(1);
        } else {
            if (b % 2 == 0) {
                P res = deg(a, b / 2);
                return res * res;
            } else {
                P res = deg(a, b - 1);
                return res * a;
            }
        }
    }
    static std::pair<Polynomial, Polynomial> div_mod(const Polynomial& a,
        const Polynomial& b) {
        auto c = a;
        Polynomial result;
        while (c.Degree() >= b.Degree()) {
            auto mult = std::prev(c.coef.end())->second / std::prev(b.coef.end())->second;
            Polynomial tmp;
            tmp.coef[c.Degree() - b.Degree()] = mult;
            result += tmp;
            c -= (b * tmp);
        }
        return { result, c };
    }

public:
    Polynomial(const std::vector<T>& ar) {
        for (size_t i = 0; i < ar.size(); i++) {
            coef[i] = ar[i];
        }
        erase_zero();
    }
    Polynomial(const T& a = T()) {
        coef[0] = a;
        erase_zero();
    }
    template <typename iter>
    Polynomial(iter fir, iter sec) {
        i32 in = 0;
        while (fir != sec) {
            coef[in] = (*fir);
            fir++;
            in++;
        }
        erase_zero();
    }
    i32 Degree() const {
        if (coef.size() == 0) {
            return -1;
        }
        auto it = coef.end();
        it--;
        return it->first;
    }

    T operator[](size_t i) const {
        if (coef.find(i) == coef.end()) {
            return T();
        } else {
            return coef.at(i);
        }
    }

    auto begin() const {
        return coef.begin();
    }

    auto end() const {
        return coef.end();
    }

    Polynomial& operator+=(const Polynomial& a) {
        for (auto i : a) {
            coef[i.first] += i.second;
        }
        erase_zero();
        return *this;
    }

    Polynomial& operator-=(const Polynomial& a) {
        for (auto i : a) {
            coef[i.first] -= i.second;
        }
        erase_zero();
        return *this;
    }
    Polynomial& operator*=(const Polynomial& a) {
        std::map<i32, T> M;
        for (auto i : coef) {
            for (auto j : a.coef) {
                M[i.first + j.first] += i.second * j.second;
            }
        }
        coef = M;
        erase_zero();
        return *this;
    }
    T operator()(const T& a) const {
        T res = T();
        for (auto i : coef) {
            res += i.second * deg(a, i.first);
        }
        return res;
    }

    friend Polynomial<T> operator+(const Polynomial<T>& a, const Polynomial<T>& b) {
        return Polynomial<T>(a) += b;
    }
    friend Polynomial<T> operator-(const Polynomial<T>& a, const Polynomial<T>& b) {
        return Polynomial<T>(a) -= b;
    }
    friend bool operator==(const Polynomial& a, const Polynomial& b) {
        return a.coef == b.coef;
    }
    friend bool operator!=(const Polynomial& a, const Polynomial& b) {
        return a.coef != b.coef;
    }
    friend Polynomial<T> operator*(const Polynomial<T>& a, const Polynomial<T>& b) {
        return Polynomial<T>(a) *= b;
    }
    friend Polynomial<T> operator/(const Polynomial<T>& a, const Polynomial<T>& b) {
        return div_mod(a, b).first;
    }
    friend Polynomial<T> operator%(const Polynomial<T>& a, const Polynomial<T>& b) {
        return div_mod(a, b).second;
    }
    friend std::ostream& operator<<(std::ostream& out, const Polynomial<T>& ms) {
        if (ms.Degree() == -1) {
            out << '0';
        } else if (ms.Degree() == 0 && ms.coef.find(0) != ms.coef.end()) {
            out << ms[0];
        } else {
            auto i = ms.coef.end();
            while (i != ms.coef.begin()) {
                i--;
                if (i->second != T(0)) {
                    if (i->first > 0) {
                        if (i->second == T(-1)) {
                            out << '-';
                        } else {
                            if (i->first != ms.Degree() && i->second > T(0)) {
                                out << '+';
                            }
                            if (i->second != T(1)) {
                                out << i->second;
                                out << '*';
                            }
                        }
                        if (i->first > 1) {
                            out << "x^";
                            out << i->first;
                        } else {
                            out << 'x';
                        }
                    } else {
                        if (i->second > T(0)) {
                            out << '+';
                        }
                        out << i->second;
                    }
                }
            }
        }
        return out;
    }
    friend Polynomial<T> operator&(const Polynomial<T>& a, const Polynomial<T>& b) {
        Polynomial<T> res;
        for (auto i : a.coef) {
            res += Polynomial<T>(i.second) * deg(b, i.first);
        }
        return res;
    }
};

template <typename T>
Polynomial<T> operator,(const Polynomial<T>& a, const Polynomial<T>& b) {
    if (b == Polynomial<T>(0)) {
        return a / a[a.Degree()];
    } else {
        return (b, a % b);
    }
}
