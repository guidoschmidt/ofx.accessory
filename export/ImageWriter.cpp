#include "ImageWriter.hpp"

void ImageWriter::setOutputDir(std::string dirname) {
  _outputDir = dirname;
}

void ImageWriter::addFrame(ofPixels *pixels, int frameNumber) {
  if (!_isRunning)
    return;
  ImageWriterFrame frame;
  frame._pixels = *pixels;
  frame._number = frameNumber;
  _frameQueue.emplace(frame);
}

void ImageWriter::saveFrames() {
  while (!_frameQueue.empty()) {
    std::stringstream ss;
    std::unique_lock<std::mutex> lock(_mutex);
    ImageWriterFrame next = _frameQueue.front();
    ss << std::setw(4) << std::setfill('0') << next._number;
    std::string frameNumberString = ss.str();
    auto path = "export/png/" + _outputDir + "/" + frameNumberString + ".png";
    ofSaveImage(next._pixels, path, OF_IMAGE_QUALITY_BEST);
    _frameQueue.pop();
    _atomicCounter++;
    _queueSize = _frameQueue.size();
    lock.unlock();
  }
}

void ImageWriter::start() {
  _atomicCounter = 0;
  _queueSize = 0;
  _isRunning = true;
  _isFinished = false;
  const int THREAD_COUNT = 4;
  for (int t = 0; t < THREAD_COUNT; t++) {
    new std::thread(&ImageWriter::saveFrames, this);
  }
}

void ImageWriter::stop() {
  _isRunning = false;
}

void ImageWriter::draw(const glm::vec2 position, const int width) {
  if (!_isFinished && _queueSize > 1) {
    const int height = 6;
    float queueSize = _queueSize > 1 ? static_cast<float>(_queueSize) : 0.001;
    float progress = _atomicCounter / queueSize;
    int progressWidth = static_cast<int>(progress * width);
    ofPushMatrix();
    ofPushStyle();
    ofFill();
    ofSetColor(100, 100, 100, 255);
    ofDrawRectangle(position, width, height);
    ofSetColor(255, 86, 67, 255);
    ofDrawRectangle(position, progressWidth, height);
    ofPopStyle();
    ofPopMatrix();
  }
}
