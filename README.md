# Timed App Trainer For Linux

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

      Verbose Output

  -a [program]

      Use [program] instead of xviewer to display images.

      Program runs in a fork subshell and outputs to /dev/null.

      The -f switch will not apply.

  -x

      Allow time to be less than 5s (min 0s). [Use with caution]

  -u

      Timer will count up to the given time instead of down to zero.



### Details:

   The purpose of this program is to aid artists in drawing but it can be used for other purposes. 

   The program simply counts down a timer and calls the specified program on a random image (png,jpg,gif) with xviewer by default.
   
   Controls allows you to navigate the past files and change time

### Controls:

   spacebar : pause / resume

   r : reset timer

   left : previous image

   right : next image

   up : add 30 seconds

   down : subtract 30 seconds

   shift+up/down : add/sub 30 minutes

   Ctr+X:  exit.

### BUGS

   Spaces in paths. Linux std::filesystem::path appears to error on spaces. Google didn't find anything.

   The KB Arrow input is laggy and confusing. We're using 2 kinds of input. Should fix the arrow input.

   Window loses focus when xviewer shows. Annoying when cycling images ..
