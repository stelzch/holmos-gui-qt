#ifndef FOURIERTRANSFORMER_H
#define FOURIERTRANSFORMER_H

#include <QObject>
#include <fftw3.h>
#include <unordered_map>
#include <cassert>
#include "mcompleximage.h"



struct plan_id {
    unsigned int n0, n1;
    bool forward;

    bool const operator==(const plan_id &other) const {
        return bool(n0 == other.n0 && n1 == other.n1 && forward == other.forward);
    }
};
struct KeyHasher
{
std::size_t operator()(const plan_id& k) const
{
  using std::size_t;
  using std::hash;

  return ((hash<unsigned int>()(k.n0)
           ^ (hash<unsigned int>()(k.n1) << 1)) >> 1)
           ^ (hash<bool>()(k.forward) << 1);
}
};

class FourierTransformer
{
public:
    explicit FourierTransformer() : fft_plans() {

    }
    ~FourierTransformer() {
        for(auto it : fft_plans) {
                //fftw_destroy_plan(it.second);
        }

    }

    void print();
    void planFFTFor(MComplexImage &img, bool);

    void executeFFT(MComplexImage &, MComplexImage &, bool);
    void executeFFT(MComplexImage &img, bool);

    std::unordered_map<plan_id, fftw_plan, KeyHasher> fft_plans;

signals:

public slots:

};


#endif // FOURIERTRANSFORMER_H
