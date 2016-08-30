#include <Rcpp.h>
using namespace Rcpp;

class Comparator {
private:
    const Rcpp::NumericVector& ref;

    bool is_na(double x) const
    {
        return Rcpp::traits::is_na<REALSXP>(x);
    }

public:
    Comparator(const Rcpp::NumericVector& ref_)
        : ref(ref_)
    {}

    bool operator()(const int ilhs, const int irhs) const
    {
        double lhs = ref[ilhs], rhs = ref[irhs];
        if (is_na(lhs)) return false;
        if (is_na(rhs)) return true;
        return lhs < rhs;
    }
};

// [[Rcpp::export]]
NumericVector avg_rank(Rcpp::NumericVector x)
{
    R_xlen_t sz = x.size();
    Rcpp::IntegerVector w = Rcpp::seq(0, sz - 1);
    std::sort(w.begin(), w.end(), Comparator(x));

    Rcpp::NumericVector r = Rcpp::no_init_vector(sz);
    for (R_xlen_t n, i = 0; i < sz; i += n) {
        n = 1;
        while (i + n < sz && x[w[i]] == x[w[i + n]]) ++n;
        for (R_xlen_t k = 0; k < n; k++) {
            r[w[i + k]] = i + (n + 1) / 2.;
        }
    }

    return r;
}

// [[Rcpp::export]]
double auc_(NumericVector actual, NumericVector predicted) {

  double n = actual.size();

  NumericVector Ranks = avg_rank(predicted);
  double NPos = sum(actual == 1);
  double NNeg = (actual.size() - NPos);

  double sumranks = 0;

  for(int i = 0; i < n; ++i) {
    if (actual[i] == 1){
      sumranks = sumranks + Ranks[i];
    }
  }

  double p1 = (sumranks - NPos*( NPos + 1 ) / 2);
  double p2 = NPos*NNeg;

  double auc =  p1 / p2;
  return auc ;

}


