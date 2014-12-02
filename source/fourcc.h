#ifndef _FOURCC_H_
#define _FOURCC_H_

/*! constexpr version to create a fourcc value
 *
 * \see https://gist.github.com/t-mat/f3caa7a44270a8767158
 */
constexpr uint32_t fourcc(const char* p) {
    return   (static_cast<uint32_t>(p[0]) << (8*0))
           | (static_cast<uint32_t>(p[1]) << (8*1))
           | (static_cast<uint32_t>(p[2]) << (8*2))
           | (static_cast<uint32_t>(p[3]) << (8*3));
}

#endif // _FOURCC_H_