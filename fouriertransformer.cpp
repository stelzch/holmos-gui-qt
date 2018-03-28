#include <QDebug>
#include "fouriertransformer.h"

void FourierTransformer::print() {
    for(auto it : fft_plans) {
        qDebug() << "Plan for: " << it.first.n0 << "x" <<it.first.n1 << ":" << it.first.forward;
    }
}

void FourierTransformer::planFFTFor(MComplexImage &img, bool forward) {
    struct plan_id m_plan_id;
    m_plan_id.n0 = img.getHeight();
    m_plan_id.n1 = img.getWidth();
    m_plan_id.forward = forward;

    auto pos = FourierTransformer::fft_plans.find(m_plan_id);

    if(pos != FourierTransformer::fft_plans.end()) {
        qDebug() << "Plan already constructed...";
        return;
    }
    std::pair<plan_id, fftw_plan> plan;
    plan.first = m_plan_id;
    plan.second = fftw_plan_dft_2d(img.getHeight(), img.getWidth(),
            reinterpret_cast<fftw_complex*>(img.getData()),
            reinterpret_cast<fftw_complex*>(img.getData()),
            FFTW_FORWARD, FFTW_MEASURE);

    FourierTransformer::fft_plans.insert(plan);
}

void FourierTransformer::executeFFT(MComplexImage &img, bool forward = true) {
    struct plan_id m_plan_id;
    m_plan_id.n0 = img.getHeight();
    m_plan_id.n1 = img.getWidth();
    m_plan_id.forward = forward;

    fftw_plan &plan = FourierTransformer::fft_plans.at(m_plan_id);
    if(plan == 0) {
        qDebug() << "No such plan found";
        return;
    }

    fftw_execute_dft(plan, reinterpret_cast<fftw_complex*>(img.getData()),
                     reinterpret_cast<fftw_complex*>(img.getData()));
}
void FourierTransformer::executeFFT(MComplexImage &img, MComplexImage &img2, bool forward = true) {
    assert(img.getWidth() == img2.getWidth());
    assert(img.getHeight() == img2.getHeight());

    struct plan_id m_plan_id;
    m_plan_id.n0 = img.getHeight();
    m_plan_id.n1 = img.getWidth();
    m_plan_id.forward = forward;

    fftw_plan &plan = FourierTransformer::fft_plans.at(m_plan_id);

    fftw_execute_dft(plan, reinterpret_cast<fftw_complex*>(img.getData()),
                     reinterpret_cast<fftw_complex*>(img2.getData()));
}
