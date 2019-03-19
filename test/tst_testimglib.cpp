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

QTEST_APPLESS_MAIN(TestImgLib)

#include "tst_testimglib.moc"
