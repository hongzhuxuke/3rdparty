#pragma once

namespace vhall {
  class I_DShowDevieEvent {
  public:
    virtual void OnAudioNotify(long eventCode, long param1, long param2) = 0;
    virtual void OnVideoNotify(long eventCode, long param1, long param2) = 0;
    virtual void OnVideoCaptureFail() {};
  };
}