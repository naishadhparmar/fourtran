#ifndef __DSOWRAPPER_H__
#define __DSOWRAPPER_H__

#include "boost/thread.hpp"
#include "IOWrapper/Output3DWrapper.h"
#include "util/MinimalImage.h"

#include "FullSystem/HessianBlocks.h"
#include "util/FrameShell.h"

namespace dso {

  class FrameHessian;
  class CalibHessian;
  class FrameShell;

  namespace IOWrap {

    class DSOWrapper : public Output3DWrapper {
    public:
      inline DSOWrapper() {
          printf("Hello DSO\n");
      }

      virtual ~DSOWrapper() {
          printf("Goodbye DSO\n");
      }

      virtual void publishCamPose(FrameShell* frame, CalibHessian* HCalib) override {
        printf("Current Frame %d (time %f, internal ID %d). CameraToWorld:\n",
               frame->incoming_id,
               frame->timestamp,
               frame->id);
        std::cout << frame->camToWorld.matrix3x4() << "\n";
      }

      virtual void pushDepthImageFloat(MinimalImageF* image, FrameHessian* KF ) override
      {
        printf("Predicted depth for KF %d (id %d, time %f, internal frame-ID %d). CameraToWorld:\n",
               KF->frameID,
               KF->shell->incoming_id,
               KF->shell->timestamp,
               KF->shell->id);
        std::cout << KF->shell->camToWorld.matrix3x4() << "\n";

        int maxWrite = 5;
        for(int y=0;y<image->h;y++)
        {
          for(int x=0;x<image->w;x++)
          {
            if(image->at(x,y) <= 0) continue;

            printf("Example Idepth at pixel (%d,%d): %f.\n", x,y,image->at(x,y));
            maxWrite--;
            if(maxWrite==0) break;
          }
          if(maxWrite==0) break;
        }
      }

      virtual bool needPushDepthImage() override {
        return false;
      }

      virtual void publishGraph(const std::map<uint64_t, Eigen::Vector2i, std::less<uint64_t>, Eigen::aligned_allocator<std::pair<const uint64_t, Eigen::Vector2i>>> &connectivity) override {}

      virtual void publishKeyframes( std::vector<FrameHessian*> &frames, bool final, CalibHessian* HCalib) override {}

      virtual void pushLiveFrame(FrameHessian* image) override {}

      virtual void pushDepthImage(MinimalImageB3* image) override {}
    };
  }
}

#endif
