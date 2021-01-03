#pragma once

#include <atomic>
#include <mutex>
#include <ofMain.h>

#include "./SharedQueue.hpp"

struct ImageWriterFrame {
  ofPixels _pixels;
  int _number;
};

class ImageWriter {
private:
  std::atomic<bool>             _isRunning;
  std::atomic<bool>             _isFinished;
  std::atomic<int>              _atomicCounter;
  std::atomic<int>              _queueSize;
  SharedQueue<ImageWriterFrame> _frameQueue;
  std::string                   _outputDir;
  std::mutex                    _mutex;

  void saveFrames();
  void onFinish();

public:
  void setOutputDir(std::string dirname);
  void addFrame(ofPixels* pixels, int number);
  void start();
  void stop();
  void draw(const glm::vec2 position, const int width);
};
