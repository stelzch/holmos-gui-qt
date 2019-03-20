#include <QtTest>
#include <QDebug>
#include <cvimage.h>
#include <cvimage.cpp>
#include <cvcompleximage.h>

// add necessary includes here

class TestImgLib : public QObject
{
    Q_OBJECT

public:
    TestImgLib();
    ~TestImgLib();

private slots:
    void test_case1();
    void benchConstructor();
    void benchForEach();
    void benchCvForEach();
    void benchR2SCreation();
    void benchInitValue();
    void testCreation();
    void testNormalization();
    void testFloatArr();
    void testQImageImport();
    void testComplexImage();
    void testComplexDft();

private:
    bool floatpEqual(floatp expected, floatp actual);
};

TestImgLib::TestImgLib()
{

}

TestImgLib::~TestImgLib()
{

}

bool TestImgLib::floatpEqual(floatp expected, floatp actual) {
    return (std::abs(expected - actual) < 1e-6);
}

void TestImgLib::test_case1()
{
    qDebug() << "Hello World!";
    CvGrayImage img(1024, 1024);

}

void TestImgLib::benchConstructor() {
    QBENCHMARK {
        CvGrayImage *img = new CvGrayImage(2048, 2048);

        delete img;
    }
}

void TestImgLib::testCreation() {
    CvGrayImage img(1024, 1024);
    img.forEach([] (int y, int x) {
        floatp value = sin(y);
        return value;
    });

    floatp checkValue = img.getAt(50, 0);
    QVERIFY(std::abs(checkValue - sin(50.0)) < 1e-6);
}

void TestImgLib::benchForEach() {
    QBENCHMARK {
        CvGrayImage img(2048, 2048);
        img.forEach([] (int y, int x) {
            return 0.0;
        });
    }
}

void TestImgLib::benchCvForEach() {
    QBENCHMARK {
        CvGrayImage img(2048, 2048);
        img.getMat()->forEach<floatp>([](floatp &pixel, const int *pos) {
            pixel = 0.0;
        });
    }

}

void TestImgLib::benchR2SCreation() {
    QBENCHMARK {
    const int n0 = 1024;
    const int n1 = 1024;
    std::vector<floatp> x2;
    std::vector<floatp> y2;

    for (int x = -n1; x < n1; x++)
        x2.push_back(pow(x, 2));

    for (int y = -n0; y < n0; y++)
        y2.push_back(pow(y, 2));

    std::rotate(x2.begin(), x2.begin() + n1, x2.end());
    std::rotate(y2.begin(), y2.begin() + n0, y2.end());

    CvGrayImage r2s(n0*2, n1*2);
    r2s.forEach([x2, y2] (int y, int x) {
        return x2.at(x) + y2.at(y);
    });
    }
}

void TestImgLib::benchInitValue() {
    QBENCHMARK {
        CvGrayImage img(2048, 2048);
        img.initValue(0.0);
    }
}

void TestImgLib::testNormalization() {
    CvGrayImage img(1024, 1024);

    img.forEach([] (int y, int x) {
        return (y % 2 == 0) ? 24.0 : -24.0;
    });
    img.getArr()[0] = 0.0;
    img.normalize();

    QVERIFY(floatpEqual(img.getAt(200, 0), 1.0));
    QVERIFY(floatpEqual(img.getAt(201, 20), 0.0));
    QVERIFY(floatpEqual(img.getAt(0, 0), 0.5));
}

void TestImgLib::testFloatArr() {
    const int n0 = 1024;
    const int n1 = 1024;
    CvGrayImage img(n0, n1);

    img.forEach([] (int y, int x) {
        return y / (x+2.0);
    });

    floatp *arr = img.getArr();
    for (int y = 0; y < n0; y++) {
        for (int x = 0; x < n1; x++) {
            floatp arrVal = arr[y * n0 + x];
            floatp atVal = img.getAt(y, x);

            QVERIFY(floatpEqual(arrVal, atVal));
        }
    }
}

void TestImgLib::testQImageImport() {
    Q_INIT_RESOURCE(testdata);

    QImage qimg(":/holmos_raw.png");
    CvGrayImage img = CvGrayImage::fromQImage(qimg);

    QVERIFY(floatpEqual(img.getAt(500, 200), qRed(qimg.pixel(200, 500))));
}

void TestImgLib::testComplexImage() {
    CvComplexImage cimg(1024, 1024);

    cimg.initValue(ComplexPixel(1.0, 0.0));

    ComplexPixel val = cimg.getAt(1000, 1);
    QVERIFY(floatpEqual(val.real(), 1.0));
    QVERIFY(floatpEqual(val.imag(), 0.0));
}

void TestImgLib::testComplexDft() {
    QImage img1(":/holmos_raw.png");
    QImage img2(":/holmos_raw_magspec.png");
    CvGrayImage expectedInput = CvGrayImage::fromQImage(img1);
    CvGrayImage expectedMagspec = CvGrayImage::fromQImage(img2);

    CvComplexImage cimg(1024, 1024);
    cimg.forEach([&expectedInput] (int y, int x) {
        ComplexPixel p;
        p.real(expectedInput.getAt(y, x));
        p.imag(0.0);

        return p;
    });

    CvGrayImage actualMagspec(1024, 1024);
    cimg.fft().magnitudeSpectrum(actualMagspec);

    actualMagspec.normalize();
    expectedMagspec.normalize();

    for (int x = 0; x < 1024; x++) {
        for (int y = 0; y < 1024; y++) {
            floatp actual = actualMagspec.getAt(y, x);
            floatp expected = actualMagspec.getAt(y, x);

            QVERIFY(floatpEqual(expected, actual));
        }
    }
}

QTEST_APPLESS_MAIN(TestImgLib)

#include "tst_testimglib.moc"
