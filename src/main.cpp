// -t 5m30s
// -p /root/dev
//
//
#include "./Trainer.h"

int main(int argc, char** argv) {
  //fwide -- Here we set everything to use utf8
  std::setlocale(LC_ALL, "");
  wprintf(L"");

  ImageFilters filt;
  filt._types = std::vector<std::string>{ ".png", ".jpg", ".gif", ".jpeg" };

  //Help
  if (argc == 2 && ((std::string(argv[1]).compare("--help") == 0) || (std::string(argv[1]).compare("-h") == 0) || (std::string(argv[1]).compare("/help") == 0))) {
    std::wcout << "Rapid sketch practice program for artists  by DSP." << std::endl;
    std::wcout << "Usage:" << std::endl;
    std::wcout << " strain [options] -t [time] -p [path]" << std::endl;
    std::wcout << "   -p [path]" << std::endl;
    std::wcout << "     Search path. Without -p default is ./" << std::endl;
    std::wcout << "   -e [path]" << std::endl;
    std::wcout << "     Exclude path." << std::endl;
    std::wcout << "   -t [time]" << std::endl;
    std::wcout << "     Set timeout, default 2m" << std::endl;
    std::wcout << "     Time Format: xxhxxmxxs" << std::endl;
    std::wcout << "     example: 2h10m15s" << std::endl;
    std::wcout << "   -wmin/-wmax [min/max]" << std::endl;
    std::wcout << "      Set min and max width. default -inf,inf. Ex -wmin 0 -wmax 1000" << std::endl;
    std::wcout << "   -hmin/-hmax [min/max]" << std::endl;
    std::wcout << "      Set min and max height., default -inf,inf" << std::endl;
    std::wcout << "   -f" << std::endl;
    std::wcout << "      Fullscreen" << std::endl;
    std::wcout << "   -v" << std::endl;
    std::wcout << "      Verbose output." << std::endl;
    std::wcout << "   -a [program]" << std::endl;
    std::wcout << "      Use [program] instead of xviewer to display images." << std::endl;
    std::wcout << "      Program runs in a fork subshell and outputs to /dev/null." << std::endl;
    std::wcout << "      The -f switch will not apply." << std::endl;
    std::wcout << "   -x" << std::endl;
    std::wcout << "      Allow time to be less than 5s (min 0s). [Use with caution]" << std::endl;
    std::wcout << "   -u" << std::endl;
    std::wcout << "      Timer will count up to the given time instead of down to zero." << std::endl;
    std::wcout << "Details:" << std::endl;
    std::wcout << "   The purpose of this program is to aid artists in drawing." << std::endl;
    std::wcout << "   Cycles random images (xviewer or custom app) for the specified time in a directory." << std::endl;
    std::wcout << "   Allows you to go back through the images and pause and play." << std::endl;
    std::wcout << "Controls:" << std::endl;
    std::wcout << "   spacebar : pause / resume" << std::endl;
    std::wcout << "   r : reset timer" << std::endl;
    std::wcout << "   left : previous image" << std::endl;
    std::wcout << "   right : next image" << std::endl;
    std::wcout << "   up : add 30 seconds" << std::endl;
    std::wcout << "   down : subtract 30 seconds" << std::endl;
    std::wcout << "   shift+up/down : add/sub 30 minutes" << std::endl;
    std::wcout << "BUGS" << std::endl;
    std::wcout << "   Spaces in paths. Linux std::filesystem::path appears to error on spaces. Google didn't find anything." << std::endl;
    std::wcout << "   The KB Arrow input is laggy and confusing. We're using 2 kinds of input. Should fix the arrow input." << std::endl;
    std::wcout << "   Window loses focus when xviewer shows. Annoying when cycling images .." << std::endl;
    exit(0);
  }

  //Get start path, or supplied as arg0
  filt._defaultSearchPath = argv[0];
  filt._defaultSearchPath.remove_filename();
  std::string time = "2m";
  try {
    for (int argi = 1; argi < argc; argi++) {
      bool handled = false;
#define GetArg1OrFail(inarg, vari) \
  if (!v.compare(inarg)) {         \
    vari;                          \
    handled = true;                \
  }
#define GetArg2OrFail(inarg, vari)                                    \
  if (!v.compare(inarg)) {                                            \
    if (argi < argc) {                                                \
      vari;                                                           \
      argi++;                                                         \
      handled = true;                                                 \
    }                                                                 \
    else {                                                            \
      Gu::errorExit(Gu::fmt("No switch passed to parameter", inarg)); \
    }                                                                 \
  }
#define GetArg3OrFail(inarg, vari)                                              \
  if (!v.compare(inarg)) {                                                      \
    if (argi + 1 < argc) {                                                      \
      vari;                                                                     \
      argi++;                                                                   \
      argi++;                                                                   \
      handled = true;                                                           \
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
                            std::filesystem::path(Gu::stripQuotes(argv[argi + 1]))));
      GetArg2OrFail("-e", filt._excludePaths.push_back(Gu::stripQuotes(argv[argi + 1])));
      GetArg1OrFail("-f", filt._fullscreen = true);
      GetArg1OrFail("-v", filt._verbose = true);
      GetArg3OrFail("-w", filt._minWidth = std::stoi(Gu::stripQuotes(argv[argi + 1])); filt._maxWidth = std::stoi(Gu::stripQuotes(argv[argi + 2])));
      GetArg2OrFail("-hmin", filt._minHeight = std::stoi(Gu::stripQuotes(argv[argi + 1])));
      GetArg2OrFail("-hmax", filt._maxHeight = std::stoi(Gu::stripQuotes(argv[argi + 1])));
      GetArg2OrFail("-wmin", filt._minWidth = std::stoi(Gu::stripQuotes(argv[argi + 1])));
      GetArg2OrFail("-wmax", filt._maxWidth = std::stoi(Gu::stripQuotes(argv[argi + 1])));
      GetArg1OrFail("-x", filt._allowZeroTime = true);
      GetArg1OrFail("-u", filt._countUp = true);
      GetArg2OrFail("-a", filt._customProgram = Gu::stripQuotes(argv[argi + 1]));
      if (handled == false) {
        std::wcout << "Unrecognized argument '" << Gu::ansiToUtf8(v) << "'." << std::endl;
      }
    }

    Trainer t;
    t.start(&filt, time);
  }
  catch (std::exception e) {
    std::wcout << "Error: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}