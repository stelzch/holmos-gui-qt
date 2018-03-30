#include <QtTest>
#include <QDebug>
#include "mimage.h"
#include "mcompleximage.h"
#include "fouriertransformer.h"

class MImageTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        img1 = new MGrayImage(4, 4);
        img2 = new MGrayImage(1024, 1024);
        cimg1 = new MComplexImage(8, 8);
        cimg6 = new MComplexImage(1024, 1024);

        ft.planFFTFor(*cimg6, true);
        ft.planFFTFor(*cimg6, false);
        //ft.planFFTFor(*cimg6, true);
        ft.print();
        qDebug() << "Map length: " << ft.fft_plans.size();

        img1->zeros();
        img2->zeros();
    }

    void testDimensions() {
        QVERIFY(img1->getWidth() == 4);
        QVERIFY(img1->getHeight() == 4);


        QVERIFY(img2->getWidth() == 1024);
        QVERIFY(img2->getHeight() == 1024);
    }

    void testCoSineInplace() {
        img1->sineInplace();
        for(unsigned int i=0; i<img1->getWidth()*img1->getHeight(); i++) {
            QVERIFY(img1->getAt(i) == 0.0);
        }

        img1->cosineInplace();
        for(unsigned int i=0; i<img1->getWidth()*img1->getHeight(); i++) {
            QVERIFY(img1->getAt(i) == 1.0);
        }
    }

    void testSineOperations() {
        img1->zeros();
        MGrayImage sin = img1->sine();
        for(unsigned int i=0; i<sin.getWidth() * sin.getHeight(); i++)
            QVERIFY(sin.getAt(i) == 0.0);

        QVERIFY(sin.getWidth() == img1->getWidth());
        QVERIFY(sin.getHeight() == img1->getHeight());
    }

    void testCosineOperations() {
        img1->zeros();
        MGrayImage cos = img1->cosine();
        for(unsigned int i=0; i<cos.getWidth() * cos.getHeight(); i++)
            QVERIFY(cos.getAt(i) == 1.0);

        QVERIFY(cos.getWidth() == img1->getWidth());
        QVERIFY(cos.getHeight() == img1->getHeight());
    }

    void benchmarkSine() {
        QBENCHMARK {
            img1->sineInplace();
        }
    }

    void benchmarkCosine() {
        QBENCHMARK {
            img1->cosineInplace();
        }
    }

    void testOperators() {
        img1->initValue(1);
        img2->initValue(2);

        /* PLUS OPERATOR */
        auto img3 = *img1 + *img2;
        QVERIFY(img3.getWidth() == img1->getWidth());
        QVERIFY(img3.getHeight() == img1->getHeight());
        for(unsigned int i=0; i<img3.getWidth()*img3.getHeight(); i++)
            QVERIFY(img3.getAt(i) == 3);

        /* MINUS OPERATOR */
        auto img4 = img3 - *img2;
        QVERIFY(img4.getWidth() == img3.getWidth());
        QVERIFY(img4.getHeight() == img3.getHeight());
        qDebug() << img4.getAt(2);
        for(unsigned int i=0; i<img4.getWidth()*img4.getHeight(); i++)
            QVERIFY(img4.getAt(i) == 1.0);

        /* MUL OPERATOR */
        auto img5 = img3 * img4;
        QVERIFY(img5.getWidth() == img1->getWidth());
        QVERIFY(img5.getHeight() == img1->getHeight());
        for(unsigned int i=0; i<img5.getWidth()*img5.getHeight(); i++)
            QVERIFY(img5.getAt(i) == 3.0);

        /* DIV OPERATOR */
        auto img6 = img5 / *img2;
        QVERIFY(img6.getWidth() == img5.getWidth());
        QVERIFY(img6.getHeight() == img5.getHeight());
        for(unsigned int i=0; i<img6.getWidth()*img6.getHeight(); i++)
            QVERIFY(img6.getAt(i) == 3.0/2.0);

    }

    void testComplexRW() {
        cimg1->initValue(std::complex<double>(1.0, 0.0));
        for(unsigned int i=0; i<cimg1->getWidth()*cimg1->getHeight(); i++) {
            QVERIFY(cimg1->getAt(i).real() == 1.0);
            QVERIFY(cimg1->getAt(i).imag() == 0.0);
        }
    }

    void testComplexOperators() {
        cimg1->initValue(std::complex<double>(1.0, 1.0));
        img1->initValue(2);

        auto temp1 = *cimg1 + *img1;
        QVERIFY(temp1.getWidth() == img1->getWidth());
        QVERIFY(temp1.getHeight() == img1->getHeight());
        for(unsigned int i=0; i<temp1.getWidth()*temp1.getHeight(); i++)
            QVERIFY(temp1.getAt(i) == std::complex<double>(3.0, 1.0));

        auto temp2 = *cimg1 - *img1;
        QVERIFY(temp2.getWidth() == img1->getWidth());
        QVERIFY(temp2.getHeight() == img1->getHeight());
        for(unsigned int i=0; i<temp2.getWidth()*temp2.getHeight(); i++)
            QVERIFY(temp2.getAt(i) == std::complex<double>(-1.0, 1.0));

        auto temp3 = *cimg1 * *img1;
        QVERIFY(temp3.getWidth() == img1->getWidth());
        QVERIFY(temp3.getHeight() == img1->getHeight());
        for(unsigned int i=0; i<temp3.getWidth()*temp3.getHeight(); i++)
            QVERIFY(temp3.getAt(i) == std::complex<double>(2.0, 2.0));

        auto temp4 = *cimg1 / *img1;
        QVERIFY(temp4.getWidth() == img1->getWidth());
        QVERIFY(temp4.getHeight() == img1->getHeight());
        for(unsigned int i=0; i<temp4.getWidth()*temp4.getHeight(); i++)
            QVERIFY(temp4.getAt(i) == std::complex<double>(.5, .5));

        /* ADD REAL NUMBER */
        cimg1->initValue(std::complex<double>(1.0, 1.0));
        auto temp5 = *cimg1 + 2.0;
        QVERIFY(temp5.getWidth() == cimg1->getWidth());
        QVERIFY(temp5.getHeight() == cimg1->getHeight());
        for(unsigned int i=0; i<temp5.getWidth()*temp5.getHeight(); i++)
            QVERIFY(temp5.getAt(i) == std::complex<double>(3.0, 1.0));

        /* SUB REAL NUMBER */
        cimg1->initValue(std::complex<double>(1.0, 1.0));
        auto temp6 = *cimg1 - 2.0;
        QVERIFY(temp6.getWidth() == cimg1->getWidth());
        QVERIFY(temp6.getHeight() == cimg1->getHeight());
        for(unsigned int i=0; i<temp6.getWidth()*temp6.getHeight(); i++)
            QVERIFY(temp6.getAt(i) == std::complex<double>(-1.0, 1.0));

        /* MUL REAL NUMBER */
        cimg1->initValue(std::complex<double>(1.0, 1.0));
        auto temp7 = *cimg1 * 2.0;
        QVERIFY(temp7.getWidth() == cimg1->getWidth());
        QVERIFY(temp7.getHeight() == cimg1->getHeight());
        for(unsigned int i=0; i<temp7.getWidth()*temp7.getHeight(); i++)
            QVERIFY(temp7.getAt(i) == std::complex<double>(2.0, 2.0));

        /* DIV REAL NUMBER */
        cimg1->initValue(std::complex<double>(1.0, 1.0));
        auto temp8 = *cimg1 / 2.0;
        QVERIFY(temp8.getWidth() == cimg1->getWidth());
        QVERIFY(temp8.getHeight() == cimg1->getHeight());
        for(unsigned int i=0; i<temp8.getWidth()*temp8.getHeight(); i++)
            QVERIFY(temp8.getAt(i) == std::complex<double>(.5, .5));
    }

    void testComplexMagspec() {
        cimg1->initValue(std::complex<double>(2.0, 0.0));

        auto magspec = cimg1->getMagnitudeSpectrum();
        QVERIFY(magspec.getWidth() == cimg1->getWidth());
        QVERIFY(magspec.getHeight() == cimg1->getHeight());
        qDebug() << magspec.getAt(0);
        for(unsigned int i=0; i<magspec.getWidth()*magspec.getHeight(); i++)
            QVERIFY(magspec.getAt(i) == log(2.0+1e-9));
    }

    void testComplexSectors() {
        cimg1->initValue(std::complex<double>(2.0, 0.0));
        cimg1->getData()[2*cimg1->getWidth()+2] = std::complex<double>(0.0,2.0);

        auto sector1 = cimg1->getSector(2,2,4);
        QVERIFY(sector1.getWidth() == 4);
        QVERIFY(sector1.getHeight() == 4);

        QVERIFY(sector1.getAt(0,0) == std::complex<double>(0.0, 2.0));
        QVERIFY(sector1.getAt(0,1) == std::complex<double>(2.0, 0.0));

        auto cimg2 = MComplexImage(80, 80);
        cimg2.initValue(std::complex<double>(0.0, 0.0));
        cimg2.setSector(40, 40, sector1);

        QVERIFY(cimg2.getAt(40, 40) == std::complex<double>(0.0, 2.0));
        QVERIFY(cimg2.getAt(40,41) == std::complex<double>(2.0, 0.0));
        QVERIFY(cimg2.getAt(39,41) == std::complex<double>(0.0, 0.0));
    }

    void testNormalize() {
        auto mimg = MGrayImage(50, 50);
        for(unsigned int i=0; i<mimg.getWidth()*mimg.getHeight(); i++)
            mimg.getData()[i] = i;
        mimg.normalize();

        QVERIFY(mimg.getMax() == 1.0);
        QVERIFY(mimg.getMin() == 0.0);
    }

    void benchmarkFFT() {
        auto cimg3 = MComplexImage(1024, 1024);
        QBENCHMARK {
            for(int i=0; i<5; i++)
            cimg3.fftInplace();
        }
    }
    void benchmarkFFT2() {
        auto cimg4 = MComplexImage(*cimg6);
        QBENCHMARK {
            for(int i=0; i<5; i++)
            ft.executeFFT(*cimg6, cimg4, true);
        }
    }

    void benchmarkInplaceOperator() {
        auto tmp = MComplexImage(*cimg6);

        QBENCHMARK {
            tmp *= 4.432534;
        }
    }

    void benchmarkOperator() {
        auto tmp = MComplexImage(*cimg6);

        QBENCHMARK {
            auto tmp2 = tmp * 2.23452345;
        }
    }

    void benchmarkCopyConstructor() {
        QBENCHMARK {
            auto tmp(*cimg6);
        }
    }

    void cleanupTestCase() {
        delete img1;
        delete img2;
        delete cimg1;
    }
private:
    MGrayImage *img1, *img2;
    MComplexImage *cimg1, *cimg6;
    FourierTransformer ft;
};

QTEST_MAIN(MImageTest)

#include "testmimage.moc"
