#include "./Trainer.h"
#include <thread>
#include <random>
#include <fstream>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "X11/X.h"
#include "ncurses.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
std::string int_to_hex(T i) {
  std::stringstream stream;
  stream << "0x"
         << std::setfill('0') << std::setw(8)
         << std::hex << i;
  return stream.str();
}
void Gu::focusWindow() {
  //  std::string cmd = "(wmctrl -a -i 'trainer')"
}
bool Gu::hasFocus() {
  return true;
  // god fuck this noise "can't get pid from winid" fuck that. seriously?

  //wmctrl - l - p | grep 1648 | grep - o 0x03a0000f std::wcout << getpid() << std::endl;
  //std::wcout << ret << " ";
  auto display = XOpenDisplay(NULL);  //0:
  //XGetWindowAttributes(display,)
  Window focus;
  int revert;
  XGetInputFocus(display, &focus, &revert);
  std::string h = int_to_hex(focus);
  std::string cmd = std::string("echo $(wmctrl -l -p | grep \'.*" + h + ".*" + std::to_string(getpid()) + "\')");
  std::string ret = executeReadOutput(cmd.c_str());
  if (ret.length() && ret[ret.length() - 1] == '\n') {
    ret = ret.substr(0, ret.length() - 1);
  }

  //XWindowAttributes attribs;
  //XGetWindowAttributes(display, focus, &attribs);
  //xgetdis
  if (ret.length()) {
    return true;
  }
  return false;
}
std::string Gu::executeReadOutput(const std::string& cmd) {
  std::string data = "";
  //This works only if VSCode launches the proper terminal (some voodoo back there);
  const int MAX_BUFFER = 256;
  char buffer[MAX_BUFFER];
  memset(buffer, 0, MAX_BUFFER);
  std::string cmd_mod = cmd + std::string(" 2>&1");  //redirect stderr to stdout

  FILE* stream = popen(cmd_mod.c_str(), "r");
  if (stream) {
    while (fgets(buffer, MAX_BUFFER, stream) != NULL) {
      data.append(buffer);
    }
    if (ferror(stream)) {
      std::wcout << "Error executeReadOutput() " << std::endl;
    }
    clearerr(stream);
    pclose(stream);
  }

  return data;
}

std::string Gu::fmt(const std::string& aft, ...) {
  std::string strRet;
  va_list args;
  va_start(args, aft);
  strRet = formatVa(aft, args);
  va_end(args);
  return strRet;
}
std::string Gu::formatVa(const std::string& aft, va_list args) {
  //Yeah I don't know what's going on here. Linux bug maybe. first vsnprintf .. works .. second doesn't .. remove second .. first doesn't work.

  std::string strRet = "";
  //he vsnprintf function returns the number of characters written, not counting the terminating null character.
  char buf[4096];
  int nCount2 = vsnprintf(buf, 4096, aft.c_str(), args);
  std::string cv(buf, nCount2);
  int nCount = vsnprintf(nullptr, 0, aft.c_str(), args);

  if (nCount == 0) {
    //Empty, no format
  }
  else {
    std::unique_ptr<char[]> tmp(new char[nCount]);
    vsnprintf(tmp.get(), nCount, aft.c_str(), args);
    strRet = std::string((char*)tmp.get(), nCount);
  }

  return cv;
}
std::wstring Gu::fmtw(const std::wstring& aft, ...) {
  std::wstring strRet;
  va_list args;
  va_start(args, aft);
  strRet = formatVaw(aft, args);
  va_end(args);
  return strRet;
}
std::wstring Gu::formatVaw(const std::wstring& aft, va_list args) {
  std::wstring strRet(L"");
  //he vsnprintf function returns the number of characters written, not counting the terminating null character.
  std::unique_ptr<wchar_t[]> tmp(new wchar_t[4096]);
  int nCount = vswprintf(tmp.get(), 4096, aft.c_str(), args);
  strRet = std::wstring((wchar_t*)tmp.get(), nCount);

  return strRet;
}
// std::string Gu::fmt(const char* format, ...) {
//   va_list args;
// #define MAX_STR_LEN 2048
//   char buf[MAX_STR_LEN];
//   //memset(buf, 0, MAX_STR_LEN);
//   va_start(args, format);
//   snprintf(buf, MAX_STR_LEN, format, args);
//   va_end(args);
//   std::string err(buf);
//   return err;
// }
void Gu::errorExit(const std::string& e) {
  std::cerr << "Error: " << e << std::endl;
  exit(1);
}

std::string Gu::stripQuotes(const std::string& inputt) {
  std::string input = inputt;
  input.erase(std::remove(input.begin(), input.end(), '\"'), input.end());
  input.erase(std::remove(input.begin(), input.end(), '\''), input.end());
  return input;
}
std::string Gu::enquote(const std::string& inputt, bool use_single_quotes) {
  std::string out = "";
  if (!use_single_quotes) {
    out = std::string("\"") + inputt + std::string("\"");
  }
  else {
    out = std::string("\'") + inputt + std::string("\'");
  }
  return out;
}
bool Gu::getImageInfo(const std::filesystem::path& path, ImageInfo& out) {
  std::string dir_ext = path.extension().string();

  if (strncasecmp(".png", dir_ext.c_str(), 999) == 0) {
    out._type = ImageInfo::ImageType::PNG;
    //       std::ifstream in(path);
    //   unsigned int width = 0, height = 0;
    //
    //   in.seekg(16);
    //   in.read((char*)&width, 4);
    //   in.read((char*)&height, 4);
    //
    //   width = ntohl(width);
    //   height = ntohl(height);
  }
  else if ((strncasecmp(".jpg", dir_ext.c_str(), 999) == 0) || (strncasecmp(".jpeg", dir_ext.c_str(), 999) == 0)) {
    out._type = ImageInfo::ImageType::JPG;
  }
  int w = 0, h = 0;
  if (Gu::getImageSize(path.c_str(), &w, &h)) {
    out._height = h;
    out._width = w;
  }
  else {
    return false;
  }
  return true;
}
bool Gu::getImageSize(const char* fn, int* x, int* y) {
  FILE* f = fopen(fn, "rb");
  if (f == 0) return false;
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (len < 24) {
    fclose(f);
    return false;
  }
  // std::wcout << fn << std::endl;
  // Strategy:
  // reading GIF dimensions requires the first 10 bytes of the file
  // reading PNG dimensions requires the first 24 bytes of the file
  // reading JPEG dimensions requires scanning through jpeg chunks
  // In all formats, the file is at least 24 bytes big, so we'll read that always
  unsigned char buf[24];
  size_t v = fread(buf, 1, 24, f);

  // For JPEGs, we need to read the first 12 bytes of each chunk.
  // We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
  if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 && buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F') {
    long pos = 2;
    while (buf[2] == 0xFF) {
      if (buf[3] == 0xC0 || buf[3] == 0xC1 || buf[3] == 0xC2 || buf[3] == 0xC3 || buf[3] == 0xC9 || buf[3] == 0xCA || buf[3] == 0xCB) break;
      pos += 2 + (buf[4] << 8) + buf[5];
      if (pos + 12 > len) break;
      fseek(f, pos, SEEK_SET);
      v = fread(buf + 2, 1, 12, f);
    }
  }

  fclose(f);

  // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
  if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF) {
    *y = (buf[7] << 8) + buf[8];
    *x = (buf[9] << 8) + buf[10];
    //cout << *x << endl;
    return true;
  }

  // GIF: first three bytes say "GIF", next three give version number. Then dimensions
  if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F') {
    *x = buf[6] + (buf[7] << 8);
    *y = buf[8] + (buf[9] << 8);
    return true;
  }

  // PNG: the first frame is by definition an IHDR frame, which gives dimensions
  if (buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G' && buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A && buf[12] == 'I' && buf[13] == 'H' && buf[14] == 'D' && buf[15] == 'R') {
    *x = (buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0);
    *y = (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0);
    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Input::registerButton(KeySym key) {
  auto it = _keys.find(key);
  if (it == _keys.end()) {
    _keys.insert(std::make_pair(key, ButtonState::Up));
  }
}
Input::ButtonState Input::getState(KeySym key) {
  auto it = _keys.find(key);
  if (it == _keys.end()) {
    Gu::errorExit("Tried to get button without registering first.");
  }

  return it->second;
}
void Input::update() {
  Display* dpy = XOpenDisplay(":0");
  char keys_return[32];
  XQueryKeymap(dpy, keys_return);
  for (auto& it : _keys) {
    const KeySym ks = it.first;
    KeyCode kc2 = XKeysymToKeycode(dpy, ks);
    bool press = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
    ButtonState b = it.second;
    if (press) {
      if (b == ButtonState::Press) {
        it.second = ButtonState::Hold;
      }
      else if (b == ButtonState::Hold) {
      }
      else if (b == ButtonState::Release) {
        it.second = ButtonState::Press;
      }
      else if (b == ButtonState::Up) {
        it.second = ButtonState::Press;
      }
    }
    else {
      if (b == ButtonState::Press) {
        it.second = ButtonState::Release;
      }
      else if (b == ButtonState::Hold) {
        it.second = ButtonState::Release;
      }
      else if (b == ButtonState::Release) {
        it.second = ButtonState::Up;
      }
      else if (b == ButtonState::Up) {
      }
    }
  }

  XCloseDisplay(dpy);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Timer::init(const std::string& time, std::function<void()> tick, bool allowZero) {
  _tick = tick;
  _allowZero = allowZero;
  setDuration(time);
}
void Timer::setDuration(const std::string& time) {
  int64_t h, m, s, u;
  parse(time, h, m, s, u);
  _durationMs = toMs(h, m, s, u);
  checkValidTime();
}
void Timer::checkValidTime() {
  if (_durationMs < 0) {
    _durationMs = 0;
  }
  if (_allowZero == false && _durationMs < c_minTimeMS) {
    _durationMs = c_minTimeMS;
  }
}
void Timer::parse(const std::string& timestr, int64_t& _hours_out, int64_t& _minutes_out, int64_t& _seconds_out, int64_t& _u_out) {
  std::string timestr_strip = timestr;
  timestr_strip = Gu::stripQuotes(timestr_strip);

  _hours_out = _minutes_out = _seconds_out = _u_out = 0;

  std::string zh = "", zm = "", zs = "", us = "";
  std::string tok = "";
  for (int ci = 0; ci < timestr_strip.length(); ++ci) {
    char c = timestr_strip[ci];
    if (c == 'm') {
      if (zm.length()) {
        std::cerr << ("Warning: Time string invalid format, minutes already specified.") << std::endl;
      }
      zm = tok;
      tok = "";
    }
    else if (c == 'h') {
      if (zh.length()) {
        std::cerr << ("Warning: Time string invalid format, hours already specified.") << std::endl;
      }
      zh = tok;
      tok = "";
    }
    else if (c == 's') {
      if (zm.length()) {
        std::wcout << ("Warning: Time string invalid format, seconds already specified.") << std::endl;
      }
      zs = tok;
      tok = "";
    }
    else if (c == 'u') {
      if (us.length()) {
        std::cerr << ("Warning: Time string invalid format, milliseconds (u) already specified.") << std::endl;
      }
      us = tok;
      tok = "";
    }
    else if (isdigit(c)) {
      tok += c;
    }
    else {
      Gu::errorExit(Gu::fmt("Invalid time value %s", timestr_strip.c_str()));
    }
  }
  if (tok.compare("")) {
    std::wcout << "Warning - time format incorrect." << std::endl;
  }
  try {
    if (zh.compare("")) {
      _hours_out = std::stol(zh);
    }
    if (zm.compare("")) {
      _minutes_out = std::stol(zm);
    }
    if (zs.compare("")) {
      _seconds_out = std::stol(zs);
    }
    if (us.compare("")) {
      _u_out = std::stol(us);
    }
  }
  catch (std::exception e) {
    std::wcout << "Exception in conversion for time: " << e.what() << std::endl;
  }
}
void Timer::start() {
  _state = State::Running;
  _last = std::chrono::high_resolution_clock::now();
}
void Timer::pause() {
  _state = State::Pause;
}
void Timer::reset() {
  _last = std::chrono::high_resolution_clock::now();
  _total = 0;
}
void Timer::stop() {
  reset();
  _state = State::Stop;
}
void Timer::tick() {
  if (_tick) {
    _tick();
  }
}
void Timer::update() {
  if (_state == State::Running) {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> d = t2 - _last;
    uint64_t millis = d.count();
    _last = t2;
    _total += millis;
    while (_total > _durationMs) {
      _total -= _durationMs;
      tick();
    }
  }
}
void Timer::parseMicroseconds(int64_t microseconds, int64_t& h, int64_t& m, int64_t& s, int64_t& u) {
  h = m = s = u = 0;
  h = microseconds / (60 * 60 * 1000);
  microseconds %= (60 * 60 * 1000);
  m = microseconds / (60 * 1000);
  microseconds %= (60 * 1000);
  s = microseconds / (1000);
  microseconds %= (1000);
  u = microseconds;
}
std::string Timer::getString(bool remaining) {
  std::string out = "";
  int64_t h = 0, m = 0, s = 0, u = 0;
  if (remaining) {
    parseMicroseconds(_durationMs - _total, h, m, s, u);
  }
  else {
    parseMicroseconds(_total, h, m, s, u);
  }
  if (h > 0) {
    out += std::to_string(h);
    out += ":";
  }
  if (h > 0 || m > 0) {
    out += addPlaceholders(std::to_string(m), (h > 0) ? 2 : 0);
    out += ":";
  }
  out += addPlaceholders(std::to_string(s), (h > 0 || m > 0) ? 2 : 0);
  out += ".";
  out += addPlaceholders(std::to_string(u), 3);

  return out;
}
std::string Timer::addPlaceholders(const std::string& sz_in, int count, char p) {
  std::string sz_out = sz_in;
  while (sz_out.length() < count) {
    sz_out = std::string((char*)&p, 1) + sz_out;
  }
  return sz_out;
}
int64_t Timer::toMs(int64_t h, int64_t m, int64_t s, int64_t u) {
  int64_t ms = h * 60 * 60 * 1000 + m * 60 * 1000 + s * 1000 + u;
  return ms;
}
void Timer::add(const std::string& time) {
  int64_t h = 0, m = 0, s = 0, u = 0;
  parse(time, h, m, s, u);
  int64_t ms = toMs(h, m, s, u);
  _durationMs += ms;
  checkValidTime();
}
void Timer::sub(const std::string& time) {
  int64_t h = 0, m = 0, s = 0, u = 0;
  parse(time, h, m, s, u);
  int64_t ms = toMs(h, m, s, u);
  _durationMs -= ms;
  checkValidTime();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageFilters::ImageFilters() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Trainer::Trainer() {
}
void Trainer::getFileOrCount(const ImageFilters* filters, std::wstring* file, uint64_t* count) {
  std::vector<std::filesystem::path> paths;
  if (filters->_includePaths.size() == 0) {
    //Default to ./ if no path specified
    paths = std::vector<std::filesystem::path>{ filters->_defaultSearchPath };
  }
  else {
    paths = filters->_includePaths;
  }

  for (auto& path : filters->_includePaths) {
    getFileOrCount(path, filters, file, count);
  }
}
bool Trainer::filterWidthHeight(const std::filesystem::path& dirPath, const ImageFilters* filters) {
  bool size_pass = true;
  if (filters->_maxHeight != -1 || filters->_minHeight != -1 || filters->_maxWidth != -1 || filters->_minWidth != -1) {
    //If we set any size constraints.
    ImageInfo inf;
    Gu::getImageInfo(dirPath, inf);
    if (filters->_maxHeight != -1) {
      if (inf._height > filters->_maxHeight) {
        size_pass = false;
      }
    }
    if (filters->_minHeight != -1) {
      if (inf._height < filters->_minHeight) {
        size_pass = false;
      }
    }
    if (filters->_maxWidth != -1) {
      if (inf._width > filters->_maxWidth) {
        size_pass = false;
      }
    }
    if (filters->_minHeight != -1) {
      if (inf._width < filters->_minWidth) {
        size_pass = false;
      }
    }
  }
  return size_pass;
}
void Trainer::getFileOrCount(const std::filesystem::path& indir,
                             const ImageFilters* filters, std::wstring* file, uint64_t* count) {
  //Return:
  // If file is null && count != null -> count = total number of files that match the filter.
  // file != null && count != null -> count is the file index to return, file will be the path to the selected file.
  if (file != nullptr && file->compare(L"") && (*count) <= 0) {
    //Grabbed file, exit
    return;
  }
  if (filters == nullptr) {
    Gu::errorExit("Filters was not set.");
  }

  if (std::filesystem::exists(indir)) {
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ indir }) {
      bool bExclude = false;
      for (auto const& exclude_dir : filters->_excludePaths) {
        if (dir_entry.path().root_path().compare(exclude_dir.root_path())) {
          bExclude = true;
          break;
        }
      }

      if (dir_entry.is_regular_file() && bExclude == false) {
        std::string dir_ext = dir_entry.path().extension().string();
        for (auto& ext : filters->_types) {
          if (strncasecmp(ext.c_str(), dir_ext.c_str(), 999) == 0) {
            //Filter on image size.
            bool size_pass = filterWidthHeight(dir_entry.path(), filters);

            if (size_pass) {
              if (file == nullptr) {
                //Searching for the random file.
                (*count)++;
              }
              else {
                //We're using count to determine which file.
                if ((*count) <= 0) {
                  *file = dir_entry.path().wstring();
                  return;
                }
                (*count)--;
              }
            }
            else {
              //std::wcout<<"size for"<<dir_entry.path().string()<<" did not pass"<<std::endl;
            }
          }
        }
      }
    }
  }
  else {
    std::wcout << "Error '" << indir << "' does not exist" << std::endl;
  }
}
//https://web.archive.org/web/20180401093525/http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
void nonblock(bool state) {
  struct termios ttystate;

  //get the terminal state
  tcgetattr(STDIN_FILENO, &ttystate);

  if (state) {
    //turn off canonical mode
    ttystate.c_lflag &= ~ICANON;
    //minimum of number input read.
    ttystate.c_cc[VMIN] = 1;
  }
  else {
    //turn on canonical mode
    ttystate.c_lflag |= ICANON;
  }
  //set the terminal attributes.
  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}
int kbhit() {
  static bool bb = false;
  if (!bb) {
    bb = true;
    nonblock(true);
  }
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);  //STDIN_FILENO is 0
  select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}
bool Trainer::doInput() {
  if (kbhit()) {
    std::wcout << '\b';
    char c = fgetc(stdin);

    if (c == ' ') {
      if (_timer->state() == Timer::State::Pause || _timer->state() == Timer::State::Stop) {
        _timer->start();
      }
      else {
        _timer->pause();
        _prevIdx = _pastFiles.size() - 1;
      }
    }
    else if (c == 'r') {
      _timer->reset();
    }
    //These don't use that non blocking thing.
    else if (_input.pressed(XK_Right)) {
      _timer->pause();
      if (_prevIdx < _pastFiles.size() - 1) {
        _prevIdx++;
        showImage(_pastFiles[_prevIdx], false);
      }
      else {
        showImage(grabImage());
      }
    }
    else if (_input.pressed(XK_Left)) {
      _timer->pause();
      if (_prevIdx > 0) {
        _prevIdx--;
      }
      showImage(_pastFiles[_prevIdx], false);
    }
    else if (_input.pressed(XK_Up)) {
      if (_input.pressOrHold(XK_Shift_L)) {
        _timer->add("30m");
      }
      else {
        _timer->add("30s");
      }
    }
    else if (_input.pressed(XK_Down)) {
      if (_input.pressOrHold(XK_Shift_L)) {
        _timer->sub("30m");
      }
      else {
        _timer->sub("30s");
      }
    }
    else if (_input.pressed(XK_C) || _input.pressed(XK_Z) || (c == 3) || (c == 26)) {
      //Not sure why but c==26 and 3 seem to be escape sequences input
      if (_input.pressOrHold(XK_Control_L) || _input.pressOrHold(XK_Control_R)) {
        return true;
      }
    }
  }
  return false;
}
bool Input::pressed(KeySym key) {
  bool b = getState(key) == Input::ButtonState::Press;
  return b;
}
bool Input::pressOrHold(KeySym key) {
  bool b = (getState(key) == Input::ButtonState::Press) || (getState(key) == Input::ButtonState::Hold);
  return b;
}
std::string Gu::escapeSingleQuotes(const std::string& str) {
  std::string out = "";
  for (auto i = 0; i < str.length(); ++i) {
    if (str[i] == '\'') {
      out += "\\'";
    }
    else {
      out += str[i];
    }
  }
  return out;
}
void Trainer::showImage(const std::string& filecnv, bool print) {
  std::string view = "";

  //Clear the progress bar before printing a newline.
  wprintf(L"%ls%ls", std::wstring(c_PBSizeChars+1, ' ').c_str(),
          std::wstring(c_PBSizeChars+1, '\b').c_str());

  if (filecnv.length() == 0) {
    std::wcout << "No file could be found that matched the given filter."
               << "\r" << std::endl;
    fflush(stdout);
  }
  else {
    if (_filters->_customProgram.length()) {
      view = _filters->_customProgram + " ";
    }
    else {
      view = "xviewer ";

      view += "-g -w ";  //No gallery, single window
      if (_filters->_fullscreen) {
        view += "-f ";
      }
      //view += "--display='HDMI-0' ";
    }

    if (!_filters->_verbose) {
      view = std::string("2>/dev/null 1>/dev/null ") + view;
    }
    view = view + std::string("\"") + (filecnv) + std::string("\"");
    if (!_filters->_verbose) {
      view = view + std::string(" &");
    }

    view = std::string("( ") + view + std::string(" )");
    fflush(stdout);

    //How to stop focus shift I have no idea.

    //Print file so we can grab it if we want
    if (print) {
      if (_filters->_verbose) {
        std::wcout << Gu::ansiToUtf8(view);
      }
      std::wcout << Gu::ansiToUtf8(filecnv) << "\r" << std::endl;
    }

    FILE* fv = popen(view.c_str(), "r");
    if (fv == nullptr) {
      std::cerr << "Error POPEN failed." << std::endl;
    }
    pclose(fv);
    fflush(stdout);

    //focus ...
    //$! gets this process ID
    //xwininfo get window ID
    //wmctrl -Ri {xdotool search --pid {$!}}
    //shows display IDs
    //xrandr -q | egrep "\sconnected" | egrep -o "^\S*"
  }
}
void Trainer::printSpinner() {
}
std::wstring repeatWstr(int n, const std::wstring& st) {
  std::wostringstream os;
  for (int i = 0; i < n; i++) {
    os << st;
  }
  return os.str();
}
std::string Gu::utf8ToAnsi(const std::wstring& in) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::string out = converter.to_bytes(in);
  return out;
}
std::wstring Gu::ansiToUtf8(const std::string& in) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::wstring out = converter.from_bytes(in);
  return out;
}
void Trainer::printTimer() {
  fflush(stdout);

  bool color = false;

  //Progress Bar
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  int row_width = ((w.ws_col >= 2) ? (w.ws_col - 2) : (w.ws_col)) - 5;
  if (row_width > c_PBSizeChars - 2) {
    row_width = c_PBSizeChars - 2;
  }

  double epct = _timer->elapsedPct();
  double pb_dchars = (double)(row_width)*epct;
  int pb_chars = (int)(pb_dchars);
  double charpct = pb_dchars - (double)pb_chars;
  wchar_t final_code = 0x258F - (int)round(7.0 * charpct);
  std::wstring pb = std::wstring(pb_chars, 0x2588) + std::wstring(1, final_code);  // repeatWstr(pb_chars, L"\u2588");  //std::wstring(pb_chars, '=');//
  std::wstring pb_sp = std::wstring(row_width - pb_chars == 0 ? row_width - pb_chars : row_width-pb_chars -1, ' ');
  pb = std::wstring( color ? L"\33[0;32m" : L"") + std::wstring(1, 0x2595) + pb + pb_sp + std::wstring(1, 0x258F) + std::wstring(color ? L"\33[0;30m" : L"");

  //Time
  std::wstring time_st = Gu::ansiToUtf8(_timer->getString(!_filters->_countUp));

  //Superimpose time onto progress bar.
  auto epos = pb.length() / 2 - time_st.length() / 2;
  pb.erase(epos, time_st.length());
  pb = pb.substr(0, epos) + std::wstring(color ? L"\33[0;34m" : L"")+time_st +std::wstring(color ? L"\33[0;32m" : L"")+ pb.substr(epos, pb.length() - epos);

  wprintf(L"%ls%ls", pb.c_str(), std::wstring(pb.length(), '\b').c_str());
  fflush(stdout);

  return;
  /*
  fflush(stdout);

  //Progress Bar
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  int row_width = ((w.ws_col >= 2) ? (w.ws_col - 2) : (w.ws_col)) - 5;
  if (row_width > c_PBSizeChars - 2) {
    row_width = c_PBSizeChars - 2;
  }

  double epct = _timer->elapsedPct();
  int pb_chars = (int)((double)row_width * epct);
  std::string pb = std::string(pb_chars, '=');// repeatWstr(pb_chars, "\u2500");  //"="std::string(pb_chars, '='); //
  std::string pb_sp = std::string(row_width - pb_chars, ' ');
  pb = std::string("[") + pb + pb_sp + std::string("]");

  //Time
  std::string time_st = _timer->getString(!_filters->_countUp);

  //Superimpose time onto progress bar.
  auto epos = pb.length() / 2 - time_st.length() / 2;
  pb.erase(epos, time_st.length());
  pb = pb.substr(0, epos) + time_st + pb.substr(epos, pb.length() - epos);

  printf("%s%s", pb.c_str(), std::string(pb.length(), '\b').c_str());
  fflush(stdout);
 */
}
std::random_device rd;
std::mt19937 mt(rd());
std::string Trainer::grabImage() {
  //Returns empty string if no filters matched the file.
  uint64_t count = 0;
  getFileOrCount(_filters, nullptr, &count);
  std::uniform_int_distribution<int64_t> dist(0, count);
  std::wstring filee = L"";
  uint64_t at = dist(mt);
  getFileOrCount(_filters, &filee, &at);
  std::string filecnv = Gu::utf8ToAnsi(filee);
  _pastFiles.push_back(filecnv);
  _prevIdx = _pastFiles.size() - 1;
  return filecnv;
}
void Trainer::start(ImageFilters* filters, const std::string& time) {
  assert(filters != nullptr);

  //Prevent any character input into the terminal
  struct termios oldt;
  if (tcgetattr(0, &oldt)) {
    fwprintf(stderr, L"Error getting term attribs\n");
  }
  cfmakeraw(&oldt);
  if (tcsetattr(0, TCSANOW, &oldt)) {
    fwprintf(stderr, L"Error setting term attribs\n");
  }

  _filters = filters;

  _timer = std::make_unique<Timer>();
  _timer->init(time, nullptr, _filters->_allowZeroTime);

  _timer->setTick([&]() {
    //Removing this in favor of -w switch which is much less annoying. If a future problem we can kill it.
    if (filters->_verbose) {
      //  popen("pkill xviewer", "r");
    }
    else {
      //   popen("2>/dev/null 1>/dev/null pkill xviewer &", "r");
    }

    std::string filecnv = grabImage();
    showImage(filecnv);
  });

  _timer->tick();
  _timer->start();

  _input.registerButton(XK_P);
  _input.registerButton(XK_R);
  _input.registerButton(XK_space);
  _input.registerButton(XK_Right);
  _input.registerButton(XK_Left);
  _input.registerButton(XK_Up);
  _input.registerButton(XK_Down);
  _input.registerButton(XK_Shift_L);
  _input.registerButton(XK_Control_L);
  _input.registerButton(XK_Control_R);
  _input.registerButton(XK_C);
  _input.registerButton(XK_Z);

  while (true) {
    if (doInput()) {
      break;
    }
    _timer->update();
    _input.update();

    printTimer();
    if (_timer->state() == Timer::State::Running) {
      printSpinner();
    }

    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(10));
  }
}