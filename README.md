# Sketch Trainer For Linux

strain --help

Rapid sketch practice program for artists.

### Usage:

 strain [options] -t [time] -p [path]

   -p [path]

     Search path. Without -p default is ./

   -e [path]

     Exclude path.

   -t [time]

     Set timeout, default 2m

     Time Format: xxhxxmxxs

     example: 2h10m15s

   -wmin/-wmax [min/max]

      Set min and max width. default -inf,inf. Ex -wmin 0 -wmax 1000

   -hmin/-hmax [min/max]

      Set min and max height., default -inf,inf

   -f

      Fullscreen

   -v

      Do not redirect xviewer output (debug info)

   -o

      Do not redirect xviewer output (debug info)

      Do not redirect xviewer output (debug info)

### Details:

   The purpose of this program is to aid artists in rapid figure drawing.

   Cycles random images (xviewer) for the specified time in a directory.

   Allows you to go back through the images and pause / play.

   Image packs not included (cough, cough, torrents).

### Controls:

   spacebar : pause / resume

   r : reset timer

   left : previous image

   right : next image

   up : add 30 seconds

   down : subtract 30 seconds

   shift+up/down : add/sub 30 minutes

### BUGS

   Spaces in paths. Linux std::filesystem::path appears to error on spaces. Google didn't find anything.

   The KB Arrow input is laggy and confusing. We're using 2 kinds of input. Should fix the arrow input.

   Window loses focus when xviewer shows. Annoying when cycling images ..