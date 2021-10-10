// -t 5m30s
// -p /root/dev
//
//
#include "./Trainer.h"

int main(int argc, char** argv) {
  ImageFilters filt;
  filt._types = std::vector<std::string>{ ".png", ".jpg", ".gif", ".jpeg" };

  //Help
  if (argc == 2 && ((std::string(argv[1]).compare("--help") == 0) || (std::string(argv[1]).compare("-h") == 0) || (std::string(argv[1]).compare("/help") == 0))) {
    std::cout << "Rapid sketch practice program for artists  by DSP." << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << " strain [options] -t [time] -p [path]" << std::endl;
    std::cout << "   -p [path]" << std::endl;
    std::cout << "     Search path. Without -p default is ./" << std::endl;
    std::cout << "   -e [path]" << std::endl;
    std::cout << "     Exclude path." << std::endl;
    std::cout << "   -t [time]" << std::endl;
    std::cout << "     Set timeout, default 2m" << std::endl;
    std::cout << "     Time Format: xxhxxmxxs" << std::endl;
    std::cout << "     example: 2h10m15s" << std::endl;
    std::cout << "   -wmin/-wmax [min/max]" << std::endl;
    std::cout << "      Set min and max width. default -inf,inf. Ex -wmin 0 -wmax 1000" << std::endl;
    std::cout << "   -hmin/-hmax [min/max]" << std::endl;
    std::cout << "      Set min and max height., default -inf,inf" << std::endl;
    std::cout << "   -f" << std::endl;
    std::cout << "      Fullscreen" << std::endl;
    std::cout << "   -v" << std::endl;
    std::cout << "      Do not redirect xviewer output (debug info)" << std::endl;
    std::cout << "   -o" << std::endl;
    std::cout << "      Do not redirect xviewer output (debug info)" << std::endl;
    std::cout << "      Do not redirect xviewer output (debug info)" << std::endl;
    std::cout << "Details:" << std::endl;
    std::cout << "   The purpose of this program is to aid artists in rapid figure drawing." << std::endl;
    std::cout << "   Cycles random images (xviewer) for the specified time in a directory." << std::endl;
    std::cout << "   Allows you to go back through the images and pause / play." << std::endl;
    std::cout << "   Image packs not included (cough, cough, torrents)." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "   spacebar : pause / resume" << std::endl;
    std::cout << "   r : reset timer" << std::endl;
    std::cout << "   left : previous image" << std::endl;
    std::cout << "   right : next image" << std::endl;
    std::cout << "   up : add 30 seconds" << std::endl;
    std::cout << "   down : subtract 30 seconds" << std::endl;
    std::cout << "   shift+up/down : add/sub 30 minutes" << std::endl;
    std::cout << "BUGS" << std::endl;
    std::cout << "   Spaces in paths. Linux std::filesystem::path appears to error on spaces. Google didn't find anything." << std::endl;
    std::cout << "   The KB Arrow input is laggy and confusing. We're using 2 kinds of input. Should fix the arrow input." << std::endl;
    std::cout << "   Window loses focus when xviewer shows. Annoying when cycling images .." << std::endl;
    exit(0);
  }

  //Get start path, or supplied as arg0
  filt._defaultSearchPath = argv[0];
  filt._defaultSearchPath.remove_filename();
  std::string time = "2m";
  try {
    for (int argi = 1; argi < argc; argi++) {
#define GetArg1OrFail(inarg, vari) \
  if (!v.compare(inarg)) {         \
    vari;                          \
  }
#define GetArg2OrFail(inarg, vari)                                    \
  if (!v.compare(inarg)) {                                            \
    if (argi < argc) {                                                \
      vari;                                                           \
    }                                                                 \
    else {                                                            \
      Gu::errorExit(Gu::fmt("No switch passed to parameter", inarg)); \
    }                                                                 \
  }
#define GetArg3OrFail(inarg, vari)                                              \
  if (!v.compare(inarg)) {                                                      \
    if (argi + 1 < argc) {                                                      \
      vari;                                                                     \
    }                                                                           \
    else {                                                                      \
      Gu::errorExit(Gu::fmt("Not enough switches passed to parameter", inarg)); \
    }                                                                           \
  }
      std::string v(argv[argi]);
      //My own personal
      GetArg2OrFail("-d", {filt._minHeight=1000; filt._minWidth=1000; time="2m"; filt._fullscreen=true; });
      GetArg2OrFail("-t", time = std::string(argv[argi + 1]));
      GetArg2OrFail("-p", filt._includePaths.push_back(
        std::filesystem::path(Gu::stripQuotes(argv[argi + 1]))
        ));
      GetArg2OrFail("-e", filt._excludePaths.push_back(Gu::stripQuotes(argv[argi + 1])));
      GetArg1OrFail("-f", filt._fullscreen = true);
      GetArg1OrFail("-v", filt._verbose = true);
      GetArg3OrFail("-w", filt._minWidth = std::stoi(Gu::stripQuotes(argv[argi + 1])); filt._maxWidth = std::stoi(Gu::stripQuotes(argv[argi + 2])));
      GetArg2OrFail("-hmin", filt._minHeight = std::stoi(Gu::stripQuotes(argv[argi + 1])));
      GetArg2OrFail("-hmax", filt._maxHeight = std::stoi(Gu::stripQuotes(argv[argi + 1])));
      GetArg2OrFail("-wmin", filt._minWidth = std::stoi(Gu::stripQuotes(argv[argi + 1])));
      GetArg2OrFail("-wmax", filt._maxWidth = std::stoi(Gu::stripQuotes(argv[argi + 1])));
    }

    Trainer t;
    t.start(&filt, time);
  }
  catch (std::exception e) {
    std::cout << "Error: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}