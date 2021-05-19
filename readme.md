# Simple alert application in C
- You must have sox and libsox-fmt-mp3 installed as this is how the mp3 files are played from the terminal
- sudo apt-get install sox libsox-fmt-mp3

Uses two threads, one thread to update all alerts' current_time and compare this with duration,
and another thread which displays all alerts (if in display mode, otherwise the menu will be showing)