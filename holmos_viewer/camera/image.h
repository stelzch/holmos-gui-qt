#ifndef IMAGE_H
#define IMAGE_H

#include <cassert>
#include <vector>
#include <QImage>

template<typename T>
class Image
{
public:
    Image(unsigned int width = 0, unsigned int height = 0, unsigned short channel_count = 1) :
        m_width(width), m_height(height), m_channel_count(channel_count) {
        assert(m_channel_count > 0);
        m_data.resize(m_width * m_height * m_channel_count);
    }

    Image(const Image<T> &src) :
        m_width(src.getWidth()), m_height(src.getHeight()), m_channel_count(src.getChannelCount()),
        m_data(src.getVector()) {

    }

    ~Image() {}

    void resize(unsigned int new_width, unsigned int new_height, unsigned int new_channel_count) {
        m_data.resize(new_width * new_height * new_channel_count);
        m_width = new_width;
        m_height = new_height;
        m_channel_count = new_channel_count;
    }

    unsigned int getWidth() const {
        return m_width;
    }
    unsigned int getHeight() const {
        return m_height;
    }
    unsigned short getChannelCount() const {
        return m_channel_count;
    }

    std::vector<T> &modifyVector() {
        return m_data;
    }
    std::vector<T> getVector() const {
        return m_data;
    }

    T getAt(unsigned int x, unsigned int y, unsigned short channel=0) {
        return m_data.at(m_channel_count * (y * m_width + x) + channel);
    }

    QImage asQImage() const {
        return QImage();
    }



private:
    unsigned int m_width, m_height;
    unsigned short m_channel_count;
    std::vector<T> m_data;
};

#endif // IMAGE_H
