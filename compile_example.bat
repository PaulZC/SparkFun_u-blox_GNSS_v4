if [%1]==[] goto useDefault

set EXAMPLE=%1
goto dockerBuild

:useDefault

set EXAMPLE=Example1_PositionVelocityTime

:dockerBuild

::Uncomment "docker builder prune -f" below to clear the build cache
::docker builder prune -f
docker build -t example_container --no-cache-filter deployment^
 --build-arg EXAMPLE=%EXAMPLE% .
docker create --name=example example_container:latest
docker cp example:/%EXAMPLE%.ino.bin .
docker cp example:/%EXAMPLE%.ino.elf .
docker cp example:/%EXAMPLE%.ino.bootloader.bin .
docker cp example:/%EXAMPLE%.ino.partitions.bin .
docker container rm example
