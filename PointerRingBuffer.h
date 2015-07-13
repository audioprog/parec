#ifndef POINTERRINGBUFFER_H
#define POINTERRINGBUFFER_H

class PointerRingBuffer {
public:
    /** init Stack class
     * @param p_size size of ring buffer
     */
    PointerRingBuffer (int p_size);
    ~PointerRingBuffer ();

    /** put to ring buffer
     * @param dat data
     * @return data / -1:error
     */
    int put (char dat);

    /** put to ring buffer
     * @param dat data
     * @param len length
     * @return put length
     */
    int put (char *dat, int len);

    /** get from ring buffer
     * @param dat data
     * @retval 0:ok / -1:error
     */
    int get (char *dat);

    /** get from ring buffer
     * @param dat data
     * @param len length
     * @return get length
     */
    int get (char *dat, int len);

    void clear ();
    int available ();
    int use ();

private:
    char *buf;
    int size;
    int addr_w, addr_r;
};
#endif // POINTERRINGBUFFER_H
