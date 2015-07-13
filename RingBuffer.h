#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "SampleFormat.h"

class RingBuffer {
 public:
   RingBuffer(sampleFormat format, int size);
   ~RingBuffer();

   //
   // For the writer only:
   //

   int AvailForPut();
   int Put(samplePtr buffer, sampleFormat format, int samples);

   //
   // For the reader only:
   //

   int AvailForGet();
   int Get(samplePtr buffer, sampleFormat format, int samples);
   int Discard(int samples);

 private:
   int Len();

   sampleFormat  mFormat;
   int           mStart;
   int           mEnd;
   int           mBufferSize;
   samplePtr     mBuffer;
};

#endif // RINGBUFFER_H
