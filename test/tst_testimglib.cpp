#include <QtTest>

// add necessary includes here

class TestImgLib : public QObject
{
    Q_OBJECT

public:
    TestImgLib();
    ~TestImgLib();

private slots:
    void test_case1();

};

TestImgLib::TestImgLib()
{

}

TestImgLib::~TestImgLib()
{

}

void TestImgLib::test_case1()
{

}

QTEST_APPLESS_MAIN(TestImgLib)

#include "tst_testimglib.moc"
