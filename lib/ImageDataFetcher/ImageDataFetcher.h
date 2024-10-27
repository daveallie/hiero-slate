#ifndef IMAGEDATAFETCHER_H
#define IMAGEDATAFETCHER_H

#include <c_types.h>

class ImageDataFetcher {
 public:
  void PullDataCb(void (*startCb)(),
                  void (*processPartCb)(const u8 data[], size_t size),
                  void (*finishCb)()) const;
};

#endif
