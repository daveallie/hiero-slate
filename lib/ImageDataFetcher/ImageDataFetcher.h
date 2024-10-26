#ifndef IMAGEDATAFETCHER_H
#define IMAGEDATAFETCHER_H

class ImageDataFetcher {
public:
  void PullDataCb(void (*startCb)(), void (*processPartCb)(const uint8 data[], size_t size), void (*finishCb)()) const;
};

#endif
