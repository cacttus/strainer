#ifndef __GU_23423490_H__
#define __GU_23423490_H__
#include "./TrainerHeader.h"
class Timer {
public:
  enum class State { Running,
                     Stop,
                     Pause };

  Timer() {}
  void init(const std::string& time, std::function<void()> tick = nullptr);
  void start();
  void update();
  void reset();
  void stop();
  void pause();
  void tick();
  void setTick(std::function<void()> tick) { _tick = tick; }
  static void parse(const std::string& timestr, int64_t& _hours_out, int64_t& _minutes_out, int64_t& _seconds_out);
  State state() { return _state; }
  std::string getString(bool remaining);
  double elapsedPct() { return (double)(_total) / (double)(_tick_seconds * 1000); }
  void add(const std::string& time);
  void sub(const std::string& time);

private:
  void updateWithNewTime();
  void normalizeTime();

  State _state = State::Stop;
  int64_t _hours = 0;
  int64_t _minutes = 2;
  int64_t _seconds = 0;
  int64_t _tick_seconds = 0;
  std::function<void()> _tick = nullptr;
  std::chrono::high_resolution_clock::time_point _last;
  uint64_t _total = 0;
};
class ImageFilters {
public:
  ImageFilters();
  //std::filesystem::path _searchPathRoot = "./";
  std::filesystem::path _defaultSearchPath;
  std::vector<std::filesystem::path> _includePaths;
  std::vector<std::filesystem::path> _excludePaths;
  std::vector<std::string> _types{ ".png", ".jpg", ".gif", ".jpeg" };
  int32_t _minWidth = -1;
  int32_t _maxWidth = -1;
  int32_t _minHeight = -1;
  int32_t _maxHeight = -1;
  bool _fullscreen = false;
  bool _verbose = false;
};
class ImageInfo {
public:
  enum class ImageType { Unset,
                         PNG,
                         JPG };
  int64_t _width = -1;
  int64_t _height = -1;
  ImageType _type = ImageType::Unset;
};
class Gu {
public:
  static void errorExit(const std::string& e);

  static std::string fmt(const std::string& aft, ...);
  static std::wstring fmtw(const std::wstring& aft, ...);
  static std::string formatVa(const std::string& aft, va_list args);
  static std::wstring formatVaw(const std::wstring& aft, va_list args);
  static std::string stripQuotes(const std::string& inputt);
  static bool getImageInfo(const std::filesystem::path& path, ImageInfo& out);
  static bool getImageSize(const char* fn, int* x, int* y);
  static void focusWindow();
  static bool hasFocus();
  static std::string executeReadOutput(const std::string& cmd);
  static std::string escapeSingleQuotes(const std::string& str);
  static std::string enquote(const std::string& inputt, bool use_single_quotes=false);
};
class Input {
public:
  enum class ButtonState { Press,
                           Hold,
                           Release,
                           Up };
  void update();
  ButtonState getState(KeySym key);
  void registerButton(KeySym key);
  bool pressed(KeySym key);
  bool pressOrHold(KeySym key);

private:
  std::map<KeySym, ButtonState> _keys;
};
class Trainer {
public:
  Trainer();
  void start(ImageFilters* filters, const std::string& time);

private:
  ImageFilters* _filters = nullptr;
  int64_t _prevIdx = 0;
  std::vector<std::string> _pastFiles;
  Input _input;
  std::unique_ptr<Timer> _timer;

  bool doInput();
  void getFileOrCount(const std::filesystem::path& indir, const ImageFilters* filters, std::wstring* file = nullptr, uint64_t* count = nullptr);
  void getFileOrCount(const ImageFilters* filters, std::wstring* file = nullptr, uint64_t* count = nullptr);
  bool filterWidthHeight(const std::filesystem::path& dirPath, const ImageFilters* filters);
  void showImage(const std::string& filecnv, bool print = true);
  std::string grabImage();
  void printSpinner();
  void printTimer();
};

#endif