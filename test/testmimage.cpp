#include <QtTest>
#include <QDebug>
#include "mimage.h"

class MImageTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        img1 = new MGrayImage(4, 4);
        img2 = new MGrayImage(1024, 1024);

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

    void cleanupTestCase() {
        delete img1;
        delete img2;
    }
private:
    MGrayImage *img1, *img2;
};

QTEST_MAIN(MImageTest);

#include "testmimage.moc"
