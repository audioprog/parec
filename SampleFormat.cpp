#include "SampleFormat.h"

#include <QObject>

#include "SampleFormat.h"
//#include "Prefs.h"
#include "Dither.h"

Dither::DitherType gLowQualityDither = Dither::none;
Dither::DitherType gHighQualityDither = Dither::none;
Dither gDitherAlgorithm;

void InitDitherers()
{
   // Read dither preferences
   gLowQualityDither = Dither::none;//(Dither::DitherType)
   //gPrefs->Read(wxT("/Quality/DitherAlgorithm"), (long)Dither::none);

   gHighQualityDither = Dither::triangle;//(Dither::DitherType)
   //gPrefs->Read(wxT("/Quality/HQDitherAlgorithm"), (long)Dither::shaped);
}

const QString GetSampleFormatStr(sampleFormat format)
{
   switch(format) {
   case int16Sample:
      /* i18n-hint: Audio data bit depth (precision): 16-bit integers */
      return "16-bit PCM";
   case int24Sample:
      /* i18n-hint: Audio data bit depth (precision): 24-bit integers */
      return "24-bit PCM";
   case floatSample:
      /* i18n-hint: Audio data bit depth (precision): 32-bit floating point */
      return "32-bit float";
   }
   return "Unknown format"; // compiler food
}

samplePtr NewSamples(int count, sampleFormat format)
{
   return (samplePtr)malloc(count * SAMPLE_SIZE(format));
}

void DeleteSamples(samplePtr p)
{
   free(p);
}

// TODO: Risky?  Assumes 0.0f is represented by 0x00000000;
void ClearSamples(samplePtr src, sampleFormat format,
                  int start, int len)
{
   int size = SAMPLE_SIZE(format);
   memset(src + start*size, 0, len*size);
}

void CopySamples(samplePtr src, sampleFormat srcFormat,
                 samplePtr dst, sampleFormat dstFormat,
                 unsigned int len,
                 bool highQuality, /* = true */
                 unsigned int stride /* = 1 */)
{
   gDitherAlgorithm.Apply(
      highQuality ? gHighQualityDither : gLowQualityDither,
      src, srcFormat, dst, dstFormat, len, stride);
}

void CopySamplesNoDither(samplePtr src, sampleFormat srcFormat,
                 samplePtr dst, sampleFormat dstFormat,
                 unsigned int len,
                 unsigned int stride /* = 1 */)
{
   gDitherAlgorithm.Apply(
      Dither::none,
      src, srcFormat, dst, dstFormat, len, stride);
}
