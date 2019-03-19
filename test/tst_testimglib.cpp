#include <QtTest>
#include <QDebug>
#include <cvgrayimage.h>

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

};

TestImgLib::TestImgLib()
{

}

TestImgLib::~TestImgLib()
{

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

QTEST_APPLESS_MAIN(TestImgLib)

#include "tst_testimglib.moc"
