::Uncomment "docker builder prune -f" below to clear the build cache
::docker builder prune -f
docker build -t example1_positionvelocitytime --no-cache-filter deployment .
::docker run --rm -it -e TERM=xterm-256color example1_positionvelocitytime:latest /bin/bash
